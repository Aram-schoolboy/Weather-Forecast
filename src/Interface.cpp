#include "Interface.h"


using namespace ftxui;

std::string FloatToCutString(float number, int digits_after_point) {
    std::string string_number = std::to_string(number);
    string_number = string_number.substr(0, string_number.find('.') + digits_after_point + 1);
    return string_number;
}

std::string GetStringTimeFromUnix(std::time_t time, bool is_full = false) {
    if (is_full) {
        return std::ctime(&time);
    }

    std::string date = std::ctime(&time);
    return date.substr(0, 10);
}

ftxui::Color GetColor(float number, float minimum, float maximum) {
    float step = (maximum - minimum) / 4;

    if (number <= minimum) {
        return Color::BlueViolet;
    }
    if (number < minimum + step) {
        return Color::Blue;
    }
    if (number < minimum + 2 * step) {
        return Color::LightSkyBlue1;
    }
    if (number < minimum + 3 * step) {
        return Color::Orange1;
    }
    if (number < minimum + 4 * step) {
        return Color::Red;
    }
    return Color::DeepPink1;
}

ftxui::Element MakeInvalidNameCity(const std::string& city_name, const std::string& error_message) {
    Element box = hbox(text(city_name) | color(Color::Purple),
                       text(" - is an invalid name of city") | color(Color::RedLight));

    return error_message.empty() ? box | border: window(text(error_message) | color(Color::Red) | bold, box);
}

ftxui::Element MakePartOfDayWeatherBox(const std::string& title,
                                       const std::string& weather_state,
                                       float apparent_temperature,
                                       float actual_temperature,
                                       float wind_speed,
                                       float precipitation,
                                       float humidity) {
    std::string apparent_temperature_ = FloatToCutString(apparent_temperature, 1);
    std::string actual_temperature_ = FloatToCutString(actual_temperature, 1);
    std::string wind_speed_ = FloatToCutString(wind_speed, 1);
    std::string precipitation_ = FloatToCutString(precipitation, 1);
    std::string humidity_ = FloatToCutString(humidity, 1);

    Color actual_temperature_color = GetColor(actual_temperature, -30, 30);
    Color apparent_temperature_color = GetColor(apparent_temperature, -30, 30);
    Color wind_speed_color = GetColor(wind_speed, 0, 30);
    Color humidity_color = GetColor(humidity, 25, 75);

    Element basic_weather_box = vbox({
                                            hbox({text(weather_state)}) | color(Color::Yellow3),
                                             hbox({text(actual_temperature_) | color(actual_temperature_color), text( " (" + apparent_temperature_ + ") °C") | color(apparent_temperature_color)}),
                                             hbox({text(wind_speed_), text(" m/s") | bold}) | color(wind_speed_color),
                                             hbox({text(precipitation_), text(" mm") | bold}) | color(Color::White),
                                             hbox({text(humidity_), text("%") | bold}) | color(humidity_color),
                                     });
    return window(text(title) | hcenter | bold, basic_weather_box) | xflex_grow;
}

ftxui::Element MakeDayWeatherBox(const meteo_requests::DayWeather& day_weather) {
    ftxui::Element morning = MakePartOfDayWeatherBox(" Morning ",
                                                     day_weather.morning.weather_state,
                                                     day_weather.morning.apparent_temperature,
                                                     day_weather.morning.actual_temperature,
                                                     day_weather.morning.wind_speed,
                                                     day_weather.morning.precipitation,
                                                     day_weather.morning.humidity);

    ftxui::Element afternoon = MakePartOfDayWeatherBox(" Afternoon ",
                                                       day_weather.afternoon.weather_state,
                                                       day_weather.afternoon.apparent_temperature,
                                                       day_weather.afternoon.actual_temperature,
                                                       day_weather.afternoon.wind_speed,
                                                       day_weather.afternoon.precipitation,
                                                       day_weather.afternoon.humidity);

    ftxui::Element evening = MakePartOfDayWeatherBox(" Evening ",
                                                     day_weather.evening.weather_state,
                                                     day_weather.evening.apparent_temperature,
                                                     day_weather.evening.actual_temperature,
                                                     day_weather.evening.wind_speed,
                                                     day_weather.evening.precipitation,
                                                     day_weather.evening.humidity);

    ftxui::Element night = MakePartOfDayWeatherBox(" Night ",
                                                   day_weather.night.weather_state,
                                                   day_weather.night.apparent_temperature,
                                                   day_weather.night.actual_temperature,
                                                   day_weather.night.wind_speed,
                                                   day_weather.night.precipitation,
                                                   day_weather.night.humidity);

    ftxui::Element day_weather_box = vbox({
                                                  hbox({night, morning, afternoon, evening})

                                          });

    return window(text(" " + GetStringTimeFromUnix(day_weather.date) + " ") | bold | hcenter, day_weather_box);
}

ftxui::Element MakeCityWeatherHeader(const meteo_requests::PartOfDayWeather& current, time_t date, const std::string& city_name) {
    ftxui::Element header = hbox({
                                         MakePartOfDayWeatherBox("",
                                                                 current.weather_state,
                                                                 current.apparent_temperature,
                                                                 current.actual_temperature,
                                                                 current.wind_speed,
                                                                 current.precipitation,
                                                                 current.humidity),
                                         text(" " + GetStringTimeFromUnix(date, true) + " ") | bold

                                 });

    return window(text(" " + city_name + " ") | color(Color::Purple) | hcenter | bold, header);
}

ftxui::Element MakeCityWeather(const meteo_requests::CityWeather& city_weather,
                               time_t date, const std::string& city_name, const std::string& error_message,
                               int start_day_index, int end_day_index) {
    ftxui::Element header = MakeCityWeatherHeader(city_weather.current, date, city_name);
    Elements days;
    for (int i = start_day_index; i < end_day_index; ++i) {
        days.push_back(MakeDayWeatherBox(city_weather.days_data[i]));
    }

    ftxui::Element body = vbox(days);
    ftxui::Element city_weather_box = vbox({
                                                   header,
                                                   body,
                                           });

    return error_message.empty() ? city_weather_box | border: window(text(error_message) | color(Color::Red) | bold, city_weather_box);
}
