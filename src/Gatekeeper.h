/**
 * Copyright (C) 2016 Samuel Parkinson <sam.james.parkinson@gmail.com>
 *
 * @license MIT
 * @author Samuel Parkinson <sam.james.parkinson@gmail.com>
 */

#ifndef Gatekeeper_H_
#define Gatekeeper_H_

#include "application.h"

//
// A library for interacting with the Gatekeeper system.
//
class Gatekeeper
{
    public:

        Gatekeeper();

        void begin();
        void tick();

        bool unlock();
        bool prime();

        bool isDoorOpen();
        bool isHandsetActive();
        bool isPrimed();

        void callToneInteruptHandler();

    private:

        int _unlock(String command);
        int _prime(String command);
};

#endif // Gatekeeper_H_
