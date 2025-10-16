// Catch2 single-header alias for tests; attempt system include else vendor shim
#pragma once
#if defined(__has_include)
#if __has_include(<catch2/catch_all.hpp>)
#include <catch2/catch_all.hpp>
#else
#include "catch2_shim.hpp"
#endif
#else
#include "catch2_shim.hpp"
#endif
