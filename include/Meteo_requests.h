#pragma once

#include "cpr/cpr.h"
#include <nlohmann/json.hpp>

namespace meteo_requests {
    const std::string kRequestLink = "https://api.open-meteo.com/v1/forecast";

    std::string GetWeatherNameByCode(int weather_code);

    enum class ResponseResult {
        NO_CONNECTION,
        SERVER_ERROR,
        OK,
    };

    struct PartOfDayWeather {
        std::string weather_state;
        float apparent_temperature;
        float actual_temperature;
        float wind_speed;
        float precipitation;
        float humidity;
    };

    struct DayWeather {
        PartOfDayWeather morning;
        PartOfDayWeather afternoon;
        PartOfDayWeather evening;
        PartOfDayWeather night;
        time_t date = 0;
    };

    struct CityWeather {
        std::vector<DayWeather> days_data;
        PartOfDayWeather current;
        ResponseResult result;
    };

    cpr::Response CityWeatherRequest(const std::string& latitude, const std::string& longitude, const std::string& days_count);

    ResponseResult GetResponseResult(cpr::Response& response);

    void ParsePartOfDay(nlohmann::json& data, PartOfDayWeather& weather_data, int index);

    void ParseCurrentWeather(nlohmann::json& data, PartOfDayWeather& weather_data);

    void ParseWeather(const std::string& json_string, CityWeather& city_weather_data);

    CityWeather GetCityWeather(const std::string& latitude, const std::string& longitude, const std::string& days_count);
}






