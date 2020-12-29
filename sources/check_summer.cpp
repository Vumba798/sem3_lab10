// Copyright 2020 Vumba798 <alexandrov32649@gmail.com>
#include <check_summer.hpp>
#include <boost/asio/post.hpp>
#include <picosha2.h>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <iostream>

CheckSummer::CheckSummer(const uint32_t& amountOfThreads,
        const std::string& input,
        const std::string& output) :
    _pool(amountOfThreads),
    _input(input), _output(output) {}

auto CheckSummer::_write_db() -> void {
    try {
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::DB* db;
        rocksdb::WriteBatch batch;
        std::vector<rocksdb::ColumnFamilyHandle*> handles;
        handles.resize(_columnNames.size());

        rocksdb::Status s = db->Open(options, _output, &db);
        if (!s.ok()) {
            BOOST_LOG_TRIVIAL(error) << "An error has occured while opening"
                << " a database to write: "
                << s.ToString() << std::endl;
        }

        for (uint32_t i = 0; i < _columnNames.size(); ++i) {
            BOOST_LOG_TRIVIAL(info) << "Creating column families...";
            BOOST_LOG_TRIVIAL(info)
                << "Amount of columns: "
                << _columnNames.size();
            db->CreateColumnFamily(
                    rocksdb::ColumnFamilyOptions(),
                    _columnNames[i], &handles[i]);
            for (auto it = _data[i].begin(); it != _data[i].end(); ++it) {
                BOOST_LOG_TRIVIAL(info) << "Preparing to write an element: "
                    << it->first << " : " << it->second;
                batch.Put(handles[i],
                        rocksdb::Slice(it->first),
                        rocksdb::Slice(it->second));
            }
        }
        BOOST_LOG_TRIVIAL(info) << "Writing data...";
        db->Write(rocksdb::WriteOptions(), &batch);

        BOOST_LOG_TRIVIAL(info) << "Closing database...";
        for (auto handle : handles) {
            db->DestroyColumnFamilyHandle(handle);
        }
        delete db;
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error)
            << "A terminal error has occured while writing databese: "
            << e.what();
        throw e;
    }
}

auto CheckSummer::write_test_db() -> void {
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::DB* db;
    rocksdb::Status s = rocksdb::DB::Open(options, _input, &db);
    if (!s.ok()) {
        std::cerr << s.ToString() << std::endl;
        return;
    }

    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    handles.resize(3);
    for (uint32_t i = 0; i < 3; ++i) {
        std::string name = "Column family ";
        name += std::to_string(i);
        s = db->CreateColumnFamily(
                rocksdb::ColumnFamilyOptions(), name, &handles[i]);
    }
    rocksdb::WriteBatch batch;
    batch.Put(handles[0], rocksdb::Slice("key 1"), rocksdb::Slice("value 1"));
    batch.Put(handles[0], rocksdb::Slice("key 2"), rocksdb::Slice("value 2"));
    batch.Put(handles[0], rocksdb::Slice("key 3"), rocksdb::Slice("value 3"));
    batch.Put(handles[1], rocksdb::Slice("key 4"), rocksdb::Slice("value 4"));
    batch.Put(handles[1], rocksdb::Slice("key 5"), rocksdb::Slice("value 5"));
    batch.Put(handles[2], rocksdb::Slice("key 6"), rocksdb::Slice("value 6"));
    batch.Put(handles[2], rocksdb::Slice("key 7"), rocksdb::Slice("value 7"));
    s = db->Write(rocksdb::WriteOptions(), &batch);
    assert(s.ok());

    for (auto handle : handles) {
        db->DestroyColumnFamilyHandle(handle);
    }
    delete db;
}

auto CheckSummer::_read_db() -> void {
    try {
        BOOST_LOG_TRIVIAL(warning) << "Reading database, path: " << _input;
        rocksdb::Options options;
        options.create_if_missing = false;
        rocksdb::DB* db;

        std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies;
        std::vector<rocksdb::ColumnFamilyHandle*> handles;

        rocksdb::DB::ListColumnFamilies(
                rocksdb::DBOptions(), _input, &_columnNames);
        _data.resize(_columnNames.size());
        BOOST_LOG_TRIVIAL(info) << "Amount of columns: " << _columnNames.size();
        BOOST_LOG_TRIVIAL(info) << "Reading list of columns...";

        for (auto name : _columnNames) {
            BOOST_LOG_TRIVIAL(info) << "Name of column: " << name;
            columnFamilies.push_back(rocksdb::ColumnFamilyDescriptor(
                        name, rocksdb::ColumnFamilyOptions()));
        }
        auto s = db->OpenForReadOnly(rocksdb::Options(), _input,
                columnFamilies, &handles, &db, false);
        if (!s.ok()) {
            BOOST_LOG_TRIVIAL(error)
                << "An error has occured while openning a database: "
                << s.ToString();
            return;
        }
        for (uint32_t i = 0; i < _columnNames.size(); ++i) {
            rocksdb::Iterator* iterator = db->NewIterator(
                    rocksdb::ReadOptions(), handles[i]);
            BOOST_LOG_TRIVIAL(info) << "Reading column family \""
                << _columnNames[i] << "\"...";
            for (iterator->Seek("k"); iterator->Valid(); iterator->Next()) {
                auto key = iterator->key().data();
                auto value = iterator->value().data();

                BOOST_LOG_TRIVIAL(info) << "Key: " << key
                    << "\tValue: " << value;

                KeyValue keyValue(key, value);
                boost::asio::post(_pool,
                        std::bind(&CheckSummer::_calculate_hash, this, i,
                            keyValue));
            }
        }
        BOOST_LOG_TRIVIAL(warning) << "Successfully read database";
        for (auto handle : handles) {
            db->DestroyColumnFamilyHandle(handle);
        }

        delete db;
        BOOST_LOG_TRIVIAL(warning) << "Joining threads...";
        _pool.join();
    } catch(const std::exception &e) {
        BOOST_LOG_TRIVIAL(error)
            << "A terminal error has occured while writing database: "
            << e.what();
        throw e;
    }
}

auto CheckSummer::start() -> void {
    BOOST_LOG_TRIVIAL(error) << "Starting CheckSummer...";
    _read_db();
    BOOST_LOG_TRIVIAL(warning) << "Writing database...";
    _write_db();
    BOOST_LOG_TRIVIAL(error) << "CheckSummer executed successfully!";
}

auto CheckSummer::_calculate_hash(const uint32_t i,
        const KeyValue keyValue) -> void {
    std::string hash_hex_str;
    picosha2::hash256_hex_string(keyValue.value, hash_hex_str);
    _mutex.lock();
    _data[i][keyValue.key] = hash_hex_str;
    _mutex.unlock();
    BOOST_LOG_TRIVIAL(info)
        << "For key " << keyValue.key
        << " hash was calculated successfully: "
        << keyValue.value  << " -> " << _data[i][keyValue.key];
}

