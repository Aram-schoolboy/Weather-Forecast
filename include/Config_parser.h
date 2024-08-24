#pragma once

#include <vector>
#include <string>

enum class ParserResult {
    INVALID_CONFIG_PATH,
    INVALID_ARGUMENTS_COUNT,
    OK,
};

struct Options {
    int forecast_days = 0;
    time_t update_frequency = 0;
    std::vector<std::string> city_names;
    ParserResult parser_result = ParserResult::OK;

    bool IsValid() const;
};

Options ParseConfig(int argc, char** argv);



