#include "Ninja_requests.h"

namespace ninja_requests {
    cpr::Response CityCoordinatesRequest(const std::string& city_name) {
        return cpr::Get(cpr::Url{kRequestLink},
                        cpr::Header{{kHeaderNameForApiKey, kApiKey}},
                        cpr::Parameters{{"name", city_name}}
        );
    }

    ResponseResult GetResponseResult(cpr::Response& response) {
        switch (response.status_code) {
            case 400:
                return ResponseResult::INVALID_API;
            case 0:
                return ResponseResult::NO_CONNECTION;
        }
        if (response.status_code >= 500) {
            return ResponseResult::SERVER_ERROR;
        }
        if (response.text == "[]") {
            return ResponseResult::INVALID_CITY_NAME;
        }

        return ResponseResult::OK;
    }

    void ParseCoordinates(const std::string& json_string, CityCoordinates& city_coordinates) {
        nlohmann::json data = nlohmann::json::parse(json_string);
        city_coordinates.latitude = data["latitude"].dump();
        city_coordinates.longitude = data["longitude"].dump();
    }

    CityCoordinates GetCityCoordinates(const std::string& city_name) {
        cpr::Response response = CityCoordinatesRequest(city_name);
        ResponseResult response_result = GetResponseResult(response);
        CityCoordinates city_coordinates;
        city_coordinates.result = response_result;
        if (response_result == ResponseResult::OK) {
            ParseCoordinates(response.text.substr(1, response.text.size() - 2), city_coordinates); //substr is bad (makes a copy)
        }
        return city_coordinates;
    }
}
