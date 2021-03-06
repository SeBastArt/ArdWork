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

// must also check for arm due to Teensy incorrectly having ARDUINO_ARCH_AVR set
#if defined(__arm__)
#define WDTO_500MS 500

#elif defined(ARDUINO_ARCH_AVR)
#include <avr/sleep.h>
#include <avr/wdt.h>
#endif

#include "Thread.h"

class ThreadManager
{
public:
    ThreadManager();

    void Setup();
    // for esp8266, its always 3.2 seconds
    void Loop(uint16_t watchdogTimeOutFlag = WDTO_500MS);
    void StartThread(Thread* pThread);
    void StopThread(Thread* pThread);
    ThreadState StatusThread(Thread* pThread);
    void ResetThread(Thread* pThread)
    {
        StopThread(pThread);
        StartThread(pThread);
    }

#if defined(ARDUINO_ARCH_ESP8266)
    // must have GPIO16 tied to RST
    void EnterSleep(uint32_t microSeconds,
        void* state = NULL,
        uint16_t sizeofState = 0,
        WakeMode mode = WAKE_RF_DEFAULT);
    bool RestartedFromSleep(void* state = NULL,
        uint16_t sizeofState = 0 );
#elif defined(__arm__)
    // Arm support for sleep not implemented yet
#elif defined(ARDUINO_ARCH_AVR)
    void EnterSleep(uint8_t sleepMode = SLEEP_MODE_PWR_DOWN);
#endif

    uint32_t CurrentThreadTime()
    {
        return _lastTick;
    }

private:
    friend class Thread;

    uint32_t _lastTick;
    Thread* _pFirstThread;
    Thread* _pLastThread;

    uint32_t ProcessThreads(uint32_t deltaTime);
    void RemoveStoppedThreads();
};

