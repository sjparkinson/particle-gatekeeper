/**
 * Copyright (C) 2016 Samuel Parkinson <sam.james.parkinson@gmail.com>
 *
 * @license MIT
 * @author Samuel Parkinson <sam.james.parkinson@gmail.com>
 */

 #include "Gatekeeper.h"

// Create an instance of the Gatekeeper library.
Gatekeeper gatekeeper;

void setup() {
    gatekeeper.begin();
}

void loop() {
    gatekeeper.tick();
}
