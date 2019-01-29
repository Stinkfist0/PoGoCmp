/**
    @file MathUtils.h
    @brief Math utilities. */
#pragma once

#include <numeric>

namespace MathUtils
{

static inline bool Equals(float a, float b, float eps = 1e-5f) { return std::abs(a - b) < eps; }
static inline bool IsZero(float a, float eps = 1e-5f) { return Equals(a, 0, eps); }

}
