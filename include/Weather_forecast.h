#pragma once

#include "Meteo_requests.h"
#include "Ninja_requests.h"

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/loop.hpp"


class City {
public:
    City(const std::string& latitude, const std::string& longitude, const std::string& name, int view_days);

    City(const std::string& name);

    meteo_requests::ResponseResult UpdateCityWeatherData(int days_count);

    void DecrementViewDays();

    bool IsOutdated(time_t max_delta) const;

    const meteo_requests::CityWeather& GetCityWeather() const;

    time_t GetUpdateTime() const;

    const std::string name;
    const bool is_valid_name;
    static const int kMaxViewDays = 15;
    int view_days;

private:
    meteo_requests::CityWeather city_weather_;
    std::string latitude_;
    std::string longitude_;
    time_t update_time_ = 0;
};

class WeatherForecast {
public:
    WeatherForecast(const std::vector<std::string>& city_names, time_t update_frequency, int default_forecast_days);

    void Start();
private:
    void ArrowUpPress();

    void ArrowDownPress();

    void PlusPress();

    void MinusPress();

    void NPress();

    void PPress();

    bool InitializeFirstCity();

    void CatchError(meteo_requests::ResponseResult response_result);

    void CatchError(ninja_requests::ResponseResult response_result);

    ninja_requests::ResponseResult AddCity(int city_index);

    std::vector<City> cities_;
    std::vector<std::string> city_names_;
    time_t update_frequency_;
    int default_forecast_days_;
    int current_city_index_ = 0;
    int shift_count = 0;
    std::string error_message;
};