#include "Meteo_requests.h"

namespace meteo_requests {
    const int kHoursInDay = 24;
    const int kHoursInPartOfDay = kHoursInDay / 4;

    std::string GetWeatherNameByCode(int weather_code) {
        switch (weather_code) {
            case 00:
                return "Clear sky";
            case 01:
                return "Mainly clear";
            case 02:
                return "Partly cloudy";
            case 03:
                return "Overcast";
            case 45:
                return "Fog";
            case 48:
                return "Depositing rime fog";
            case 51:
                return "Light drizzle";
            case 53:
                return "Moderate drizzle";
            case 55:
                return "Dense drizzle";
            case 56:
                return "Freezing light drizzle";
            case 57:
                return "Freezing dense drizzle";
            case 61:
                return "Slight rain";
            case 63:
                return "Moderate rain";
            case 65:
                return "Heavy rain";
            case 66:
                return "Freezing light rain";
            case 67:
                return "Freezing heavy rain";
            case 71:
                return "Slight snow fall";
            case 73:
                return "Moderate snow fall";
            case 75:
                return "Heavy snow fall";
            case 77:
                return "Snow grains";
            case 80:
                return "Slight rain shower";
            case 81:
                return "Moderate rain shower";
            case 82:
                return "Violent rain shower";
            case 85:
                return "Slight snow shower";
            case 86:
                return "Heavy snow shower";
            case 95:
                return "Thunderstorm";
            case 96:
                return "Thunderstorm with slight hail";
            case 99:
                return "Thunderstorm with heavy hail";
            default:
                return "Unknown weather code";
        }
    }

    cpr::Response CityWeatherRequest(const std::string& latitude, const std::string& longitude, const std::string& days_count) {
        return cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"},
                        cpr::Parameters{
                                {"latitude",        latitude},
                                {"longitude",       longitude},

                                {"timezone",        "auto"},
                                {"wind_speed_unit", "ms"},
                                {"timeformat", "unixtime"},
                                {"forecast_days",   days_count},

                                {"hourly",          "weathercode"},
                                {"current",         "weathercode"},
                                {"hourly",          "temperature_2m"},
                                {"current",         "temperature_2m"},
                                {"hourly",          "apparent_temperature"},
                                {"current",         "apparent_temperature"},
                                {"hourly",          "windspeed_10m"},
                                {"current",         "windspeed_10m"},
                                {"hourly",          "relativehumidity_2m"},
                                {"current",         "relativehumidity_2m"},
                                {"hourly",          "precipitation"},
                                {"current",         "precipitation"},
                                {"hourly",          "relative_humidity_2m"},
                                {"current",         "relative_humidity_2m"},
                        }
        );
    }

    ResponseResult GetResponseResult(cpr::Response& response) {
        if (response.status_code == 0) {
            return ResponseResult::NO_CONNECTION;
        }
        if (response.status_code >= 500) {
            return ResponseResult::SERVER_ERROR;
        }

        return ResponseResult::OK;
    }

    void ParsePartOfDay(nlohmann::json& data, PartOfDayWeather& weather_data, int index) {
        weather_data.weather_state = GetWeatherNameByCode(data["hourly"]["weathercode"][index]);
        weather_data.apparent_temperature = data["hourly"]["apparent_temperature"][index];
        weather_data.actual_temperature = data["hourly"]["temperature_2m"][index];
        weather_data.wind_speed = data["hourly"]["windspeed_10m"][index];
        weather_data.precipitation = data["hourly"]["precipitation"][index];
        weather_data.humidity = data["hourly"]["relativehumidity_2m"][index];
    }

    void ParseCurrentWeather(nlohmann::json& data, PartOfDayWeather& weather_data) {
        weather_data.weather_state = GetWeatherNameByCode(data["current"]["weathercode"]);
        weather_data.apparent_temperature = data["current"]["apparent_temperature"];
        weather_data.actual_temperature = data["current"]["temperature_2m"];
        weather_data.wind_speed = data["current"]["windspeed_10m"];
        weather_data.precipitation = data["current"]["precipitation"];
        weather_data.humidity = data["current"]["relativehumidity_2m"];
    }

    void ParseWeather(const std::string& json_string, CityWeather& city_weather_data) {
        nlohmann::json data = nlohmann::json::parse(json_string);

        ParseCurrentWeather(data, city_weather_data.current);

        city_weather_data.days_data.clear();

        int days_count = data["hourly"]["weathercode"].size() / kHoursInDay;

        for (int i = 0; i < days_count; ++i) {
            int start_index = i * 24;
            DayWeather day;
            day.date = data["hourly"]["time"][start_index];
            ParsePartOfDay(data, day.night, start_index);
            ParsePartOfDay(data, day.morning, start_index + kHoursInPartOfDay);
            ParsePartOfDay(data, day.afternoon, start_index + kHoursInPartOfDay * 2);
            ParsePartOfDay(data, day.evening, start_index + kHoursInPartOfDay * 3);

            city_weather_data.days_data.push_back(day);
        }
    }

    CityWeather GetCityWeather(const std::string& latitude, const std::string& longitude, const std::string& days_count) {
        cpr::Response response = CityWeatherRequest(latitude, longitude, days_count);
        ResponseResult response_result = GetResponseResult(response);
        CityWeather city_weather_data;
        city_weather_data.result = response_result;
        if (response_result == ResponseResult::OK) {
            ParseWeather(response.text, city_weather_data);
        }
        return city_weather_data;
    }
}

