#include "Weather_forecast.h"
#include "Config_parser.h"

int main(int argc, char** argv) {
    Options options = ParseConfig(argc, argv);

    if (!options.IsValid()) {
        return 1;
    }

    WeatherForecast wf(options.city_names, options.update_frequency, options.forecast_days);
    wf.Start();
}