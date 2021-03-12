#include <Arduino.h>
#include "Actuator.h"

Actuator::Actuator(int pinRelay1, int pinRelay2) {
  _pinRelay1 = pinRelay1;
  _pinRelay2 = pinRelay2;
  _initialized = false;
}

void Actuator::initialize() {
    if (!_initialized) {
      pinMode(_pinRelay1, OUTPUT);// set pin as output for relay 1
      pinMode(_pinRelay2, OUTPUT);// set pin as output for relay 2   
      
      // keep the motor off
      turnOFF();

      _initialized = true;
    }
}

void Actuator::push() {
  digitalWrite(_pinRelay1, LOW);// turn relay 1 ON
  digitalWrite(_pinRelay2, HIGH);// turn relay 2 OFF 
}
    
void Actuator::pull() {
  digitalWrite(_pinRelay1, HIGH);// turn relay 1 OFF
  digitalWrite(_pinRelay2, LOW);// turn relay 2 ON 
}

void Actuator::turnOFF() {
  digitalWrite(_pinRelay1, HIGH);// turn relay 1 OFF
  digitalWrite(_pinRelay2, HIGH);// turn relay 2 OFF
}
