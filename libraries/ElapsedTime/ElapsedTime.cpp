/*  
  ElapsedTime.cpp - Library for measuring elapsed times in place of delay();
  Created by Mark R Dornan, 07/06/2014.
  Released into the public domain.
*/

#include "Arduino.h"
#include "ElapsedTime.h"

ElapsedTime::ElapsedTime(void)
{
  _targetTimestamp = 0;
}

ElapsedTime::ElapsedTime(unsigned long duration)
{
  startTimer(duration);
}

void ElapsedTime::startTimer(unsigned long duration)
{
  _targetTimestamp = millis() + duration;
}

boolean ElapsedTime::timeHasElapsed(void) {
  return ((long)(millis() - _targetTimestamp) >= 0);
}

long ElapsedTime::timeRemaining(void){
  return ((long)(millis() -_targetTimestamp));
}
