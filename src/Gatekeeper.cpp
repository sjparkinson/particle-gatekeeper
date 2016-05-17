/**
 * Copyright (C) 2016 Samuel Parkinson <sam.james.parkinson@gmail.com>
 *
 * @license MIT
 * @author Samuel Parkinson <sam.james.parkinson@gmail.com>
 */

#include "Gatekeeper.h"

const int pin_dr_relay = D0;
const int pin_s1_relay = D1;
const int pin_s2_relay = D2;
const int pin_ds_line = D3;
const int pin_bz_line = D4;

// When the system was last primed, unix timestamp.
int last_primed;

// The variables we will publish to the Particle service.
int is_door_open;
int is_handset_active;
int is_primed;

// When we last saw a rise on the BZ line, handled with an ISR.
volatile int last_bz_interrupt;

Gatekeeper::Gatekeeper() {}

void Gatekeeper::begin() {
    // Register Particle functions.
    Particle.function("unlock", &Gatekeeper::_unlock, this);
    Particle.function("prime", &Gatekeeper::_prime, this);

    // Register Particle variables.
    Particle.variable("isActive", is_handset_active);
    Particle.variable("isDoorOpen", is_door_open);
    Particle.variable("isPrimed", is_primed);

    // Setup the relay signals.
    pinMode(pin_dr_relay, OUTPUT);
    pinMode(pin_s1_relay, OUTPUT);
    pinMode(pin_s2_relay, OUTPUT);

    // Setup the DS line to read the door status.
    pinMode(pin_ds_line, INPUT);

    // Setup the BZ line to read the call tone with an interrupt.
    pinMode(pin_bz_line, INPUT);
    attachInterrupt(pin_bz_line, &Gatekeeper::callToneInteruptHandler, this, RISING);

    // Initalise the output pins.
    digitalWrite(pin_dr_relay, LOW);
    digitalWrite(pin_s1_relay, LOW);
    digitalWrite(pin_s2_relay, LOW);

    Particle.publish("gatekeeper/setup", PRIVATE);
}

void Gatekeeper::tick() {
    bool cachedValue;

    // Update the `is_door_open` variable.
    cachedValue = isDoorOpen();
    if (! is_door_open && cachedValue) {
        is_door_open = 1;
        Particle.publish(String("gatekeeper/door-opened"), PRIVATE);
    } else if (is_door_open && ! cachedValue) {
        is_door_open = 0;
        Particle.publish(String("gatekeeper/door-closed"), PRIVATE);
    }

    // Update the `is_handset_active` variable.
    cachedValue = isHandsetActive();
    if (! is_handset_active && cachedValue) {
        is_handset_active = 1;
        Particle.publish(String("gatekeeper/handset-activated"), PRIVATE);
    } else if (is_handset_active && ! cachedValue) {
        is_handset_active = 0;
        Particle.publish(String("gatekeeper/handset-deactivated"), PRIVATE);
    }

    // Update the `is_primed` variable.
    cachedValue = isPrimed();
    if (! is_primed && cachedValue) {
        is_primed = 1;
        Particle.publish(String("gatekeeper/primed"), PRIVATE);
    } else if (is_primed && ! cachedValue) {
        is_primed = 0;
        Particle.publish(String("gatekeeper/unprimed"), PRIVATE);
    }

    // Unlock the door if the handset is active and primed.
    if (is_handset_active && is_primed) {
        unlock();
    }
}

bool Gatekeeper::unlock() {
    // If the handset is not active, or the door's already open we can't unlock it.
    if (! isHandsetActive() || isDoorOpen()) {
        return false;
    }

    // "Pickup" the handset and "press" the door release button.
    digitalWrite(pin_s1_relay, HIGH);
    digitalWrite(pin_s2_relay, HIGH);
    digitalWrite(pin_dr_relay, HIGH);

    delay(250);

    // Finally resetting all the relays.
    digitalWrite(pin_dr_relay, LOW);
    digitalWrite(pin_s1_relay, LOW);
    digitalWrite(pin_s2_relay, LOW);

    // Ensure we unprime the system.
    last_primed = NULL;

    Particle.publish("gatekeeper/unlocked", PRIVATE);

    return true;
}

bool Gatekeeper::prime() {
    // If the door's open, what's the point in priming?
    if (isDoorOpen()) {
        return false;
    }

    last_primed = Time.now();

    return true;
}

bool Gatekeeper::isDoorOpen() {
    return (digitalRead(pin_ds_line) == LOW);
}

bool Gatekeeper::isHandsetActive() {
    return (last_bz_interrupt > Time.now() - 1 && pulseIn(pin_bz_line, HIGH) > 0);
}

bool Gatekeeper::isPrimed() {
    return (last_primed > Time.now() - 120);
}

void Gatekeeper::callToneInteruptHandler() {
    last_bz_interrupt = Time.now();
}

int Gatekeeper::_unlock(String command) {
    return unlock() ? 0 : -1;
}

int Gatekeeper::_prime(String command) {
    return prime() ? 0 : -1;
}
