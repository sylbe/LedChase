/*
  Led chase library

  array of pin numbers (the pins where the LED are attached)
  input:

  optional: period of the chase movement  (duration of a period in ms)
  optional: movement pattern ID
  0 :fixed OFF (permanent OFF)
  1 :fixed ON (permanent ON)
  2 :blinking
  3 :chase cycle 1
  optional: % of time when LED is ON vs OFF

  multitasking using Scheduler library (tbc)
 */

#ifndef LedChase_H
#define LedChase_H

#include <Arduino.h>

#define NBMAXPIN 20   //maximum number of Pins
#define LED_OFF 0x0
#define LED_ON 0x1

#define FIXED_OFF 0x0
#define FIXED_ON  0x1
#define BLINKING  0x2
#define CHASE_CYCLE 0x3
#define VUMETER 0x4

class LedChase {



public:

	LedChase(); //    LedChase();

	~LedChase();
	void attach(uint8_t*, uint8_t);

	void animateLeds(int , int , int );     //void animateLeds(int chasePeriod, int chasePattern, int chaseParam);

private:
	uint8_t _ledPins[NBMAXPIN];
	int _ledStatus[NBMAXPIN];
	uint8_t _sizeLedPins;
	uint8_t _currentPattern;
	long _previousMillis;
	long _nextMillis;
	void turnOn(uint8_t); //void turnOn(uint8_t ledIndex);
	void turnOff(uint8_t); //void turnOff(uint8_t ledIndex);
	void switchOnOff(uint8_t);

};

#endif

