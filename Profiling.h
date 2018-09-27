#pragma once

#include "Common.h"

#include <chrono>
#include <iostream>

#if defined(_DEBUG)
#define measure_time() Scope_Timer make_unique_name(timer_){__func__}
#else
#define PROFILE()
#endif

// start timer on construction, stop and publish to std::cout in destructor
struct Scope_Timer {
    using Clock = std::chrono::steady_clock;

    const char* function_name = {};
    const Clock::time_point start = {};


    Scope_Timer(const char* func_name) : function_name{func_name}, start{Clock::now()} {}
    ~Scope_Timer()
    {
        using namespace std::chrono;

        auto end = Clock::now();
        auto duration_in_ms = duration_cast<milliseconds>(end - start).count();
        if (duration_in_ms != 0) {
            std::cout << function_name << ": " << duration_in_ms << " ms\n";
            return;
        }

        auto duration_in_ns = duration_cast<nanoseconds>(end - start).count();
        std::cout << function_name << ": 0." << duration_in_ns << " ms\n";
    }

    no_copy_and_assign(Scope_Timer);
    no_move_and_assign(Scope_Timer);

};