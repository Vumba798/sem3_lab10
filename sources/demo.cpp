// Copyright 2020 Vumba798 <alexandrov32649@gmail.com>

#include <check_summer.hpp>
#include <iostream>
#include <thread>
#include <cstdint>
#include <boost/program_options.hpp>

namespace po=boost::program_options;

int main(int argc, char** argv) {
    std::string input = "";
    std::string logLevel = "";
    std::string output = "";
    uint32_t threadCount = 0;

    po::options_description visible("Options:");
    visible.add_options()
        ("help", "produce help message")
        ("log-level", po::value<std::string>()->default_value("error"),
             "\"info\"|\"warning\"|\"debug\"")
        ("thread-count", po::value<uint32_t>()->default_value(
             std::thread::hardware_concurrency()), "")
        ("output", po::value<std::string>()->default_value(
            "path/to/input/dbcs-storage.db"), "<path/to/output/storage.db>");

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input", po::value<std::string>()->default_value("path/to/input/storage.db"), "");

    po::positional_options_description pos;
    pos.add("input", -1);

    po::options_description all("Allowed options:");
    all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(all).positional(pos).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage:\n\n\tdbcs [options] <path/to/input/storage.db>\n\n\n"
            << visible << std::endl;;
        return 1;
    }
    if (vm.count("input")) {
        std::cout << "input was set to"
            << vm["input"].as<std::string>() << std::endl;
        input = vm["input"].as<std::string>();
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
        << "\nINPUT: " << input
        << "\nOUTPUT: " << output << std::endl;

    return 0;
}
