#include <check_summer.hpp>
#include <boost/asio/post.hpp>
#include <picosha2.h>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <iostream>

CheckSummer::CheckSummer(const uint32_t& amountOfThreads, const std::string& input,
        const std::string& output) :
    _pool(amountOfThreads),
    _input(input), _output(output) {}

auto CheckSummer::_write_db() -> void {
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::DB* db;
    rocksdb::WriteBatch batch;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;
    handles.resize(_columnNames.size());

    rocksdb::Status s = db->Open(options, _output, &db);
    if (!s.ok()) {
        std::cerr << s.ToString() << std::endl;
    }

    for (uint32_t i = 0; i < _columnNames.size(); ++i) {
        db->CreateColumnFamily(
                rocksdb::ColumnFamilyOptions(), _columnNames[i], &handles[i]);
        for (auto it = _data[i].begin(); it != _data[i].end(); ++it) {
            batch.Put(handles[i],
                    rocksdb::Slice(it->first),
                    rocksdb::Slice(it->second));
        }
    }
    db->Write(rocksdb::WriteOptions(), &batch);

    for (auto handle : handles) {
        db->DestroyColumnFamilyHandle(handle);
    }
    delete db;
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
        //handles.emplace_back(new rocksdb::ColumnFamilyHandle);
        std::string name = "Column family ";
        name += std::to_string(i);
        std::cout << "Name writes: " << name << std::endl;
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

    // db->Put(rocksdb::WriteOptions(), handles[0],
            // rocksdb::Slice("key 1"), rocksdb::Slice("value 1"));
    // db->Put(rocksdb::WriteOptions(), handles[0],
            // rocksdb::Slice("key 2"), rocksdb::Slice("value 2"));
    // db->Put(rocksdb::WriteOptions(), handles[0],
            // rocksdb::Slice("key 3"), rocksdb::Slice("value 3"));
    // db->Put(rocksdb::WriteOptions(), handles[1],
            // rocksdb::Slice("key 4"), rocksdb::Slice("value 4"));
    // db->Put(rocksdb::WriteOptions(), handles[1],
            // rocksdb::Slice("key 5"), rocksdb::Slice("value 5"));
    // db->Put(rocksdb::WriteOptions(), handles[2],
            // rocksdb::Slice("key 6"), rocksdb::Slice("value 6"));
    // db->Put(rocksdb::WriteOptions(), handles[2],
            // rocksdb::Slice("key 7"), rocksdb::Slice("value 7"));

    for (uint32_t i = 0; i < 3; ++i) {
        rocksdb::Iterator* iterator = db->NewIterator(rocksdb::ReadOptions(), handles[i]);
        for (iterator->Seek("k"); iterator->Valid(); iterator->Next()) {
            std::cout << i << " Key: " << iterator->key().data()
                << "\tValue: " << iterator->value().data() << std::endl;
        }
    }
    for (auto handle : handles) {
        db->DestroyColumnFamilyHandle(handle);
    }
    delete db;
}

auto CheckSummer::read_db() -> void {
    rocksdb::Options options;
    options.create_if_missing = false;
    rocksdb::DB* db;

    std::vector<std::string> columnNames;
    std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;


    // auto s = db->OpenForReadOnly(options, "new1",
           // columnFamilies, &handles, &db);

    rocksdb::DB::ListColumnFamilies(rocksdb::DBOptions(), "new1", &columnNames);

    for (auto name : columnNames) {
        std::cout << "Name: " << name << std::endl;
        columnFamilies.push_back(rocksdb::ColumnFamilyDescriptor(
                    name, rocksdb::ColumnFamilyOptions()));
    }
    auto s = db->OpenForReadOnly(rocksdb::Options(), "new1",
            columnFamilies, &handles, &db, false);
    if (!s.ok()) {
        std::cerr << s.ToString() << std::endl;
        return;
    }
    for (uint32_t i = 0; i < columnNames.size(); ++i) {
        rocksdb::Iterator* iterator = db->NewIterator(rocksdb::ReadOptions(), handles[i]);
        std::cout << "I = " << i << std::endl;
        for (iterator->Seek("k"); iterator->Valid(); iterator->Next()) {
            std::cout << "Key: " << iterator->key().data()
                << "\tValue: " << iterator->value().data() << std::endl;
        }
    }
    for (auto handle : handles) {
        db->DestroyColumnFamilyHandle(handle);
    }

    delete db;
}


