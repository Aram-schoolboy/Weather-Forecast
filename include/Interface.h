#pragma once

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/loop.hpp"

#include "Meteo_requests.h"

using namespace ftxui;

std::string FloatToCutString(float number, int digits_after_point);

std::string GetStringTimeFromUnix(std::time_t time, bool is_full);

ftxui::Color GetColor(float number, float minimum, float maximum);

ftxui::Element MakeInvalidNameCity(const std::string& city_name, const std::string& error_message);

ftxui::Element MakePartOfDayWeatherBox(const std::string& title,
                                       const std::string& weather_state,
                                       float apparent_temperature,
                                       float actual_temperature,
                                       float wind_speed,
                                       float precipitation,
                                       float humidity);

ftxui::Element MakeDayWeatherBox(const meteo_requests::DayWeather& day_weather);

ftxui::Element MakeCityWeatherHeader(const meteo_requests::PartOfDayWeather& current, time_t date, const std::string& city_name);

ftxui::Element MakeCityWeather(const meteo_requests::CityWeather& city_weather,
                               time_t date, const std::string& city_name, const std::string& error_message,
                               int start_day_index, int end_day_index);