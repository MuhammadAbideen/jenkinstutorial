#include "mathlib/mathlib.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

int main()
{
    try
    {
        // Initialize logger
        spdlog::set_level(spdlog::level::info);
        spdlog::info("Starting math operations demo");

        // Test addition
        double sum = mathlib::add(5.0, 3.0);
        spdlog::info("5 + 3 = {}", sum);

        // Test subtraction
        double difference = mathlib::subtract(10.0, 4.0);
        spdlog::info("10 - 4 = {}", difference);

        // Test division
        double quotient = mathlib::divide(15.0, 3.0);
        spdlog::info("15 / 3 = {}", quotient);

        // Test division by zero error handling
        try
        {
            mathlib::divide(10.0, 0.0);
        }
        catch (const std::invalid_argument &e)
        {
            spdlog::error("Error occurred: {}", e.what());
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Unexpected error: {}", e.what());
        return 1;
    }

    spdlog::info("Demo completed successfully");
    return 0;
}
