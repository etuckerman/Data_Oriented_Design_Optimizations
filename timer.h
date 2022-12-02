#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
using namespace std;
using namespace std::chrono;

class timer 
{
public:
    time_point<steady_clock>/*<system_clock>*/ start, end;
    void startTimer()
    {
        start = steady_clock/*<system_clock>*/::now();
    }

    void endTimer()
    {
        end = steady_clock/*<system_clock>*/::now();
    }

    void evaluateTimer()
    {
        duration<double> elapsed_seconds = end - start;
        duration<double, milli> elapsed_milliseconds = duration_cast<milliseconds>(end - start);

        cout << "Elapsed time: " << elapsed_milliseconds.count() << "ms\n";
    }
};

