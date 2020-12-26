// Copyright 2020 Vumba798 <alexandrov32649@gmail.com>

#include <check_summer.hpp>
#include <iostream>
#include <thread>
#include <cstdint>
#include <boost/program_options.hpp>

namespace po=boost::program_options;

int main(int argc, char** argv) {
    std::string logLevel = "";
    std::string output = "";
    uint32_t threadCount = 0;

    po::options_description desc("Allowed options:");
    desc.add_options()
        ("help", "produce help message")
        ("log-level", po::value<std::string>()->default_value("error"),
             "\"info\"|\"warning\"|\"debug\"")
        ("thread-count", po::value<uint32_t>()->default_value(
             std::thread::hardware_concurrency()), "")
        ("output", po::value<std::string>()->default_value(
            "path/to/input/dbcs-storage.db"), "<path/to/output/storage.db>");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }
    if (vm.count("log-level")) {
        std::cout << "log-level was set to "
            << vm["log-level"].as<std::string>() << std::endl;
        logLevel = vm["log-level"].as<std::string>();
    } else {
        std::cout << "log-level was set to default" << std::endl;
        logLevel = vm["log-level"].as<std::string>();
        std::cout << logLevel << std::endl;
    }
    if (vm.count("thread-count")) {
        std::cout << "thread-count was set to "
            << vm["thread-count"].as<uint32_t>() << std::endl;
        threadCount = vm["thread-count"].as<uint32_t>();
    } else {
        std::cout << "thread-count was set to default" << std::endl;
        threadCount = vm["thread-count"].as<uint32_t>();
    }
    if (vm.count("output")) {
        std::cout << "output was set to "
            << vm["output"].as<std::string>() << std::endl;
        output = vm["output"].as<std::string>();
    } else {
        std::cout << "output was set to default" << std::endl;
        output = vm["output"].as<std::string>();
    }

    std::cout << "\nTHREADS: " << threadCount
        << "\nLOGS: " << logLevel
        << "\nOUTPUT: " << output << std::endl;

    return 0;
}
