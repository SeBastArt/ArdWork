/*--------------------------------------------------------------------
Thread is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Thread is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

See GNU Lesser General Public License at <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#pragma once

// if you need finer timing resolution than a millisecond
// then enable the below, but this will limit the max interval 
// to just over 70 minutes
// #define TASK_MICRO_RESOLUTION

#if defined THREAD_MICRO_RESOLUTION
uint32_t badThreadTimeValue(void)
    __attribute__((error("constant is too large for 'ThreadTime' type: max is 4,294,967 milliseconds")));
#define ThreadTimePerMs 1000
#define ThreadTimeAccuracy 10
#define GetThreadTime micros
#define ThreadTimeValidate(ms) ((ms > (uint32_t(-1) / (uint32_t)ThreadTimePerMs)) ? badThreadTimeValue() : ms)
#define MsToThreadTime(ms) (ThreadTimeValidate(ms) * (uint32_t)ThreadTimePerMs)
#define UsToThreadTime(us) ((uint32_t)us)
#define ThreadTimeToMs(t) (t / (uint32_t)ThreadTimePerMs)
#define ThreadTimeToUs(t) ((uint32_t)t)
#else
//uint32_t badThreadTimeValue(void)
//    __attribute__((error("constant is too large for 'microseconds' conversion: max is 4,294,967 milliseconds")));
#define badThreadTimeValue() 1
#define ThreadTimePerMs 1
#define ThreadTimeAccuracy 1
#define GetThreadTime millis
#define MsToThreadTime(ms) ((uint32_t)ms)
#define UsToThreadTime(ms) (ms / (uint32_t)1000)
#define ThreadTimeToMs(t) ((uint32_t)t)
#define ThreadTimeValidate(t) ((t > (uint32_t(-1) / (uint32_t)1000)) ? badThreadTimeValue() : t)
#define ThreadTimeToUs(t) (ThreadTimeValidate(t) * (uint32_t)1000)
#endif

enum ThreadState
{
    ThreadState_Stopped,
    ThreadState_Running,
    ThreadState_Stopping
};

class Thread
{
public:
    Thread(uint32_t timeInterval) :
            _timeInterval(timeInterval),
            _remainingTime(0),
            _taskState(ThreadState_Stopped)
    {
    }

    void setTimeInterval(uint32_t timeInterval)
    {
        _timeInterval = timeInterval;
        if (_taskState == ThreadState_Running)
        {
            _remainingTime = timeInterval;
        }
    }

    uint32_t getTimeInterval()
    {
        return _timeInterval;
    }

protected:
    virtual bool OnStart() { return true; };
    virtual void OnStop() {};
    virtual void OnUpdate(uint32_t deltaTime) {};

    uint32_t _remainingTime;
    uint32_t _timeInterval;

private:
    friend class ThreadManager;
    Thread* _pNext; // next task in list
    ThreadState _taskState;

    void Start()
    {
        _remainingTime = _timeInterval;
        if (OnStart())
        {
            _taskState = ThreadState_Running;
        }
        else
        {
            _taskState = ThreadState_Stopping;
        }
    }
    void Stop()
    {
        OnStop();
        _taskState = ThreadState_Stopping;
    }
};

#include "ThreadManager.h"

