#include "Config_parser.h"

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>


bool Options::IsValid() const {
    switch (parser_result) {
        case ParserResult::INVALID_CONFIG_PATH:
            std::cerr << "Invalid config path";
            return false;
        case ParserResult::INVALID_ARGUMENTS_COUNT:
            std::cerr << "Invalid arguments count\n"
                         "There should be only one argument - path to config file";
            return false;
    }

    bool is_valid = true;
    if (forecast_days <= 0 || forecast_days > 16) {
        std::cerr << "forecast_days_count should be from 1 to 16\n";
        is_valid = false;
    }
    if (update_frequency <= 0) {
        std::cerr << "updating_frequency should be > 0\n";
        is_valid = false;
    }
    if (city_names.empty()) {
        std::cerr << "No cities to view\n";
        is_valid = false;
    }

    return is_valid;
}

Options ParseConfig(int argc, char** argv) {
    Options options;

    if (argc != 2) {
        options.parser_result = ParserResult::INVALID_ARGUMENTS_COUNT;
        return options;
    }

    std::ifstream config(argv[1]);

    if (!config.is_open()) {
        options.parser_result = ParserResult::INVALID_CONFIG_PATH;
        return options;
    }

    nlohmann::json data = nlohmann::json::parse(config);
    options.forecast_days = data["forecast_days_count"];
    options.update_frequency = data["updating_frequency"];
    options.city_names = data["cities"];

    return options;
}