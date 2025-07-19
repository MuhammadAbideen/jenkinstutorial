#pragma once

#include <stdexcept>

namespace mathlib
{

/**
 * @brief Adds two numbers
 * @param a First number
 * @param b Second number
 * @return Sum of a and b
 */
double add(double a, double b);

/**
 * @brief Subtracts two numbers
 * @param a First number (minuend)
 * @param b Second number (subtrahend)
 * @return Difference of a and b
 */
double subtract(double a, double b);

/**
 * @brief Divides two numbers
 * @param a Numerator
 * @param b Denominator
 * @return Result of division a/b
 * @throws std::invalid_argument if b is zero
 */
double divide(double a, double b);

} // namespace mathlib
