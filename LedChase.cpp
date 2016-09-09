/*
   LedChase.cpp

    Created on: 31 août 2016
        Author: sylvainbellier
 */


/*
  Led chase library
  input:
  array of pin numbers (the pins where the LED are attached)
  optional: period of the chase movement  (duration of a period in ms)
  optional: movement pattern ID
 FIXED_OFF 0x0   all LEDs OFF
 FIXED_ON  0x1   all LEDs ON
 BLINKING  0x2   blinking  , optional: % of time when LED is ON vs OFF  //TODO or chase direction (left or right)
 CHASE_CYCLE 0x3 chase pattern,  optional number of LEDs ON at any given time (default=1)
 VUMETER 0x4     VUMETER view, input : %  (0% = no LED, 100% = all LEDs ON,...)





 */

// This is optional, but set to false to disable
#define LED_DEBUG true
#include <LedDebug.h>
#define SERIAL_DEBUG true
#include <SerialDebug.h>


#include "LedChase.h"



//<<constructor>>
LedChase::LedChase() {
	//SERIAL_DEBUG_SETUP(9600);
	//DEBUG("LedChase constructor BEGIN -Available memory", FREE_RAM);
	int i;
	_sizeLedPins = 0;
	for (i = 0; i < NBMAXPIN - 1; i++)
	{
		_ledPins[i] =0;
		_ledStatus[i]=0;

	}
	_previousMillis= millis();
	_nextMillis=0;
	_currentPattern= FIXED_OFF;
	//DEBUG("LedChase constructor END -Available memory", FREE_RAM);

}

//<<destructor>>

LedChase::~LedChase() {  //destruction = turning all Leds OFF

	uint8_t i = 0;
	for (i = 0; i < _sizeLedPins - 1; i++) {



		turnOff(i);// turn LED off


	}
	Serial.println("Destruction END");

}
//


void leftRotatebyOne(int arr[NBMAXPIN], uint8_t n)
{
	int i, temp;
	temp = arr[0];
	for (i = 0; i < n-1; i++)
		arr[i] = arr[i+1];
	arr[i] = temp;
}
void leftRotate(int arr[NBMAXPIN], uint8_t n,uint8_t d)
{
	//n = size of the array
	//d = nb of steps to rotate
	int i;
	for (i = 0; i < d; i++)
		leftRotatebyOne(arr, n);
}

void rightRotate(int arr[NBMAXPIN], uint8_t n,uint8_t d)
{
	//n = size of the array
	//d = nb of steps to rotate
	// Rotate to the right (d) steps = rotate to the left array size - d
	while (d>n) d-=n;
	d= n-d;
	int i;
	for (i = 0; i < d; i++)
		leftRotatebyOne(arr, n);
}

//


void LedChase::attach(uint8_t ledPins[],uint8_t nbpin ) {

	Serial.println("Attach BEGIN.....");

	PULSE(5);

	_sizeLedPins=nbpin;

	Serial.println("attaching a LedChase, with Leds connected to pins: ");

	int i = 0;

	for (i = 0; i < _sizeLedPins; ++i) {
		_ledPins[i] = uint8_t(ledPins[i]);

		pinMode(_ledPins[i], OUTPUT);  //declare the pin as OUTPUT

		turnOff(i); //turn LED off
	}

	_currentPattern=FIXED_OFF;
	_previousMillis=millis();
	_nextMillis=0;

	Serial.println("attach END");
}




//turnOn();
void LedChase::turnOn(uint8_t ledIndex) {
	digitalWrite(_ledPins[ledIndex], HIGH);//turn on the Led
	//Serial.print("turning ON pin #");
	//Serial.println(_ledPins[ledIndex]);
	//_ledStatus[ledIndex]=LED_ON; //update status...
}

//turnOff()
void LedChase::turnOff(uint8_t ledIndex) {
	digitalWrite(_ledPins[ledIndex], LOW);
	//Serial.print("turning OFF pin #");
	//Serial.println(_ledPins[ledIndex]);
	//_ledStatus[ledIndex]=LED_OFF; //update status...
}



//animateLeds();
void LedChase::animateLeds(int chasePeriod = 1000, int chasePattern = BLINKING, int chaseParam = 1) {
	//default values: int chasePeriod = 1000, int chasePattern = BLINKING, int chaseParam = 1

	long currentMillis;
	uint8_t j;
	currentMillis= millis();
	int interval=0;

	if (!(chasePattern==_currentPattern)) {  //the pattern has changed
		attach(_ledPins,_sizeLedPins);   //resetting all LEDs
		_nextMillis=0;
	}
	_currentPattern=chasePattern;
	if (currentMillis >_nextMillis) { //it's time to do something
		for (j = 0; j < _sizeLedPins; j++) { //turning OFF all LEDs status to update from a clean state

						Serial.println(currentMillis);
						_ledStatus[j]=LED_OFF;  //
					}

		switch (chasePattern) {

		case FIXED_OFF:  // chasePattern: FIXED OFF  --> all LEDs must be switched OFF

			Serial.println("FIXED_OFF");

			for (j = 0; j < _sizeLedPins; j++) { //turning OFF all LEDs

				Serial.println(currentMillis);
				_ledStatus[j]=LED_OFF;  //
			}

			_previousMillis= currentMillis;
			_nextMillis=currentMillis+1000000; //next update in 1000s

			break;

		case FIXED_ON: // chasePattern: FIXED ON  --> all LEDs must be switched ON

			Serial.println("FIXED_ON");

			for (j = 0; j < _sizeLedPins; j++) { //turning ON all LEDs

				Serial.println(currentMillis);
				_ledStatus[j]=LED_ON;
			}

			_previousMillis= currentMillis;
			_nextMillis=currentMillis+1000000; //next update in 1000s

			break;

		case BLINKING: // chasePattern: BLINKING
			Serial.println("BLINKING");

			for (j = 0; j < _sizeLedPins; j++) { //switch ON if led OFF and vice versa
				if (_ledStatus[j]==LED_OFF)
					_ledStatus[j]=LED_ON;
				else
					_ledStatus[j]=LED_OFF;

			}


			_previousMillis= currentMillis;
			if (_ledStatus[1]==LED_OFF){
				//chaseParam is the chase ON/OFF Ratio ex: if chaseParam=20, the leds wil be 20% of the time ON, 80% OFF
				_nextMillis=currentMillis+(chasePeriod*(1-(chaseParam/100))); //next update in xxxx

			}
			else
			{
				_nextMillis=currentMillis+(chasePeriod*((chaseParam/100))); //next update in xxxx

			}

			break;
		case CHASE_CYCLE: // chasePattern: CHASE_CYCLE

			if (_nextMillis==0) { //first time ==> need to initialize the pattern
				//as this is a new pattern, all Leds are already set to OFF
				//chaseParam is the number of Leds  must be ON at a given time, default: 1
				if (chaseParam==1) {
					_ledStatus[0]=LED_ON; //Only the first LED will be on

				}
				else
				{
					if (chaseParam>_sizeLedPins) chaseParam= _sizeLedPins-1; // at least every LED is ON but one
					//TODO répartition homogene de x leds alluméees parmi y
					interval= int((_sizeLedPins)/(chaseParam ))-1;
					Serial.print("chaseParam= ");
					Serial.println(chaseParam);
					Serial.print("interval= ");
					Serial.println(interval);
					j=0;
					while (chaseParam>0){
						if (_ledStatus[j]!=LED_ON) {
							_ledStatus[j]=LED_ON; //all other LEDs are OFF
							chaseParam--;
							j+=(interval+1);
						}
						else
							j++;

						if (j>=_sizeLedPins) j=j-_sizeLedPins;

					}
					Serial.println("CHASE_CYCLE");
					for (j = 0; j < _sizeLedPins; j++) {
						Serial.print(_ledStatus[j]);
						Serial.print(" ,");
						//TODO passer en mode debug
					}
				}
			}
			//rotate the ON Led 1 step
			Serial.println("");
			rightRotate (_ledStatus, _sizeLedPins , 1);
			for (j = 0; j < _sizeLedPins; j++) {
				Serial.print(_ledStatus[j]);
				Serial.print(" ,");
			}

			_nextMillis=currentMillis+(chasePeriod/_sizeLedPins); //next change in (chasePeriod/number of pins) ms
			_previousMillis= currentMillis;


			break;
		case VUMETER :  //chasePattern: VUMETER
			//
			//chaseParam is the % of Leds that must be ON
			//calculate the number of Leds to switch ON
			interval= int((_sizeLedPins*chaseParam)/100 );
			if ((interval==0)&&(chaseParam>0)) interval=1; //at least 1 Led is ON if % of vumeter is not 0
			Serial.print("interval= ");
								Serial.println(interval);

			for (j = 0; j < interval; j++) {
				_ledStatus[j]=LED_ON; //all other LEDs are OFF
									}
			for (j = 0; j < _sizeLedPins; j++) {
							Serial.print(_ledStatus[j]);
							Serial.print(" ,");
						}
			Serial.println("VUMETER");
			_nextMillis=currentMillis+chasePeriod;
						_previousMillis= currentMillis;
			break;
		default:
			Serial.println("_______DEFAULT______");
			break;
		}
		//apply LED status to all LEDs
		for (j = 0; j < _sizeLedPins; j++) {
			if (_ledStatus[j]==LED_OFF)
				turnOff(j);
			else
				turnOn(j);
		}
	} //end if currentMillis >_nextMillis
}
//


