#include "Weather_forecast.h"

#include "Meteo_requests.h"
#include "Ninja_requests.h"
#include "Interface.h"

City::City(const std::string& latitude, const std::string& longitude, const std::string& name, int view_days):
    latitude_(latitude), longitude_(longitude), name(name), view_days(view_days), is_valid_name(true) {}

City::City(const std::string& name): name(name), is_valid_name(false) {}

meteo_requests::ResponseResult City::UpdateCityWeatherData(int days_count) {
    meteo_requests::CityWeather city_weather = meteo_requests::GetCityWeather(latitude_, longitude_, std::to_string(days_count));

    if (city_weather.result == meteo_requests::ResponseResult::OK) {
        city_weather_ = city_weather;
        update_time_ = std::time(nullptr);
    }

    return city_weather.result;
}

void City::DecrementViewDays() {
    if (view_days) {
        --view_days;
        city_weather_.days_data.pop_back();
    }
}

bool City::IsOutdated(time_t max_delta) const {
    if (!is_valid_name) {
        return false;
    }
    return std::time(nullptr) - update_time_ > max_delta;
}

const meteo_requests::CityWeather& City::GetCityWeather() const {
    return city_weather_;
}

time_t City::GetUpdateTime() const {
    return update_time_;
}


WeatherForecast::WeatherForecast(const std::vector<std::string>& city_names, time_t update_frequency, int default_forecast_days):
                city_names_(city_names), update_frequency_(update_frequency), default_forecast_days_(default_forecast_days) {}

ninja_requests::ResponseResult WeatherForecast::AddCity(int city_index) {
    ninja_requests::CityCoordinates city_coordinates = ninja_requests::GetCityCoordinates(city_names_[city_index]);

    switch (city_coordinates.result) {
        case ninja_requests::ResponseResult::OK:
            cities_.push_back(City(city_coordinates.latitude, city_coordinates.longitude, city_names_[city_index], default_forecast_days_));
            break;
        case ninja_requests::ResponseResult::INVALID_CITY_NAME:
            cities_.push_back(City(city_names_[city_index]));
            break;
        }

        return city_coordinates.result;
}

void WeatherForecast::ArrowDownPress() {
    if ((shift_count + 1) * 3 < cities_[current_city_index_].view_days) {
        ++shift_count;
    }

}

void WeatherForecast::ArrowUpPress() {
    if (shift_count) {
        --shift_count;
    }
}

void WeatherForecast::PlusPress() {
    City& city = cities_[current_city_index_];

    if (!city.is_valid_name || City::kMaxViewDays < city.view_days + 1) {
        return;
    }

    meteo_requests::ResponseResult response = city.UpdateCityWeatherData(city.view_days + 1);
    CatchError(response);

    switch(response) {
        case meteo_requests::ResponseResult::OK:
            ++(city.view_days);
    }
}

void WeatherForecast::MinusPress() {
    if (!cities_[current_city_index_].is_valid_name || cities_[current_city_index_].view_days == 1) {
        return;
    }

    if ((shift_count + 1) * 3 - 2 >= cities_[current_city_index_].view_days) {
        --shift_count;
    }

    cities_[current_city_index_].DecrementViewDays();
}

void WeatherForecast::NPress() {
    if (current_city_index_ + 1 == city_names_.size()) {
        return;
    }

    if (current_city_index_ + 2 <= cities_.size()) {
        if (cities_[current_city_index_ + 1].IsOutdated(update_frequency_)) {
            CatchError(cities_[current_city_index_ + 1].UpdateCityWeatherData(cities_[current_city_index_ + 1].view_days));
        }
        ++current_city_index_;
        shift_count = 0;
        return;
    }

    ninja_requests::ResponseResult ninja_response = AddCity(current_city_index_ + 1);
    CatchError(ninja_response);

    switch(ninja_response) {
        case ninja_requests::ResponseResult::INVALID_CITY_NAME:
            ++current_city_index_;
            shift_count = 0;
            return;
        case ninja_requests::ResponseResult::NO_CONNECTION:
            return;
        case ninja_requests::ResponseResult::SERVER_ERROR:
            return;
        case ninja_requests::ResponseResult::INVALID_API:
            return;
    }

    ++current_city_index_;
    meteo_requests::ResponseResult meteo_response = cities_[current_city_index_].UpdateCityWeatherData(cities_[current_city_index_].view_days);
    CatchError(meteo_response);

    switch(meteo_response) {
        case meteo_requests::ResponseResult::NO_CONNECTION:
            cities_.pop_back();
            --current_city_index_;
            return;
        case meteo_requests::ResponseResult::SERVER_ERROR:
            cities_.pop_back();
            --current_city_index_;
            return;
        case meteo_requests::ResponseResult::OK:
            shift_count = 0;
            return;
    }
}

void WeatherForecast::PPress() {
    if (current_city_index_ == 0) {
        return;
    }
    --current_city_index_;
    shift_count = 0;
    if (cities_[current_city_index_].IsOutdated(update_frequency_)) {
        CatchError(cities_[current_city_index_].UpdateCityWeatherData(cities_[current_city_index_].view_days));
    }
}

void WeatherForecast::CatchError(meteo_requests::ResponseResult response_result) {
    switch (response_result) {
        case meteo_requests::ResponseResult::NO_CONNECTION:
            error_message = "Offline";
            break;
        case meteo_requests::ResponseResult::SERVER_ERROR:
            error_message = "Server error (open-meteo.com)";
            break;
        case meteo_requests::ResponseResult::OK:
            error_message = "";
            break;
    }
}

void WeatherForecast::CatchError(ninja_requests::ResponseResult response_result) {
    switch (response_result) {
        case ninja_requests::ResponseResult::NO_CONNECTION:
            error_message = "Offline";
            break;
        case ninja_requests::ResponseResult::SERVER_ERROR:
            error_message = "Server error (api-ninjas.com)";
            break;
        case ninja_requests::ResponseResult::INVALID_API:
            error_message = "Invalid api key";
            break;
        case ninja_requests::ResponseResult::OK:
            error_message = "";
            break;
    }
}

bool WeatherForecast::InitializeFirstCity() {
    ninja_requests::ResponseResult ninja_response = AddCity(current_city_index_);

    switch(ninja_response) {
        case ninja_requests::ResponseResult::INVALID_API:
            std::cerr << "Invalid api key";
            return false;
        case ninja_requests::ResponseResult::NO_CONNECTION:
            std::cerr << "No connection";
            return false;
        case ninja_requests::ResponseResult::SERVER_ERROR:
            std::cerr << "Server error (api-ninjas.com)";
            return false;
    }

    meteo_requests::ResponseResult meteo_response = cities_[current_city_index_].UpdateCityWeatherData(default_forecast_days_);

    switch(meteo_response) {
        case meteo_requests::ResponseResult::SERVER_ERROR:
            std::cerr << "Server error (open-meteo.com)";
            return false;
        case meteo_requests::ResponseResult::NO_CONNECTION:
            std::cerr << "No connection";
            return false;
    }

    return true;
}

void WeatherForecast::Start() {
    if (!InitializeFirstCity()) {
        return;
    }

    using namespace ftxui;

    auto screen = ScreenInteractive::FitComponent();

    auto render = Renderer( [&] {
        City& city = cities_[current_city_index_];
        if (!city.is_valid_name) {
            return MakeInvalidNameCity(city.name, error_message);
        }

        int start_day_index = shift_count * 3;

        return MakeCityWeather(city.GetCityWeather(),
                               city.GetUpdateTime(), city.name, error_message,
                               start_day_index, std::min(start_day_index + 3, city.view_days));
    });

    auto component = CatchEvent(render, [&] (Event event) {
        if (event == Event::Character('+')) {
            PlusPress();
            return true;
        }
        if (event == Event::Character('-')) {
            MinusPress();
            return true;
        }
        if (event == Event::Character('n')) {
            NPress();
            return true;
        }
        if (event == Event::Character('p')) {
            PPress();
            return true;
        }
        if (event == Event::ArrowUp) {
            ArrowUpPress();
            return true;
        }
        if (event == Event::ArrowDown) {
            ArrowDownPress();
            return true;
        }
        if (event == Event::Escape) {
            screen.Exit();
            return true;
        }
        return false;
    });

    Loop loop(&screen, component);

    while (!loop.HasQuitted()) {
        loop.RunOnce();
        if (cities_[current_city_index_].IsOutdated(update_frequency_)) {
            CatchError(cities_[current_city_index_].UpdateCityWeatherData(cities_[current_city_index_].view_days));
        }
    }

}


