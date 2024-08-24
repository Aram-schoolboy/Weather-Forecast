#pragma once

#include <cpr\cpr.h>
#include <nlohmann/json.hpp>

namespace ninja_requests {
    const std::string kRequestLink = "https://api.api-ninjas.com/v1/city";
    const std::string kHeaderNameForApiKey = "X-Api-Key";
    const std::string kApiKey = "A3MzJoYoxFwFsr3to6+K8A==sCGk0DxFou4360ec";

    enum class ResponseResult {
        INVALID_API,
        INVALID_CITY_NAME,
        NO_CONNECTION,
        SERVER_ERROR,
        OK,
    };

    struct CityCoordinates {
        std::string latitude;
        std::string longitude;
        ResponseResult result;
    };

    cpr::Response CityCoordinatesRequest(const std::string& city_name);

    ResponseResult GetResponseResult(cpr::Response& response);

    void ParseCoordinates(const std::string& json_string, CityCoordinates& city_coordinates);

    CityCoordinates GetCityCoordinates(const std::string& city_name);
}




