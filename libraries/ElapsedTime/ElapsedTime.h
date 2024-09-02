/*
  ElapsedTime.h - Library for measuring elapsed times in place of dealay();
  Created by Mark R Dornan, 07/06/2014.
  Released into the public domain.
*/
#ifndef  ElapsedTime_h
#define  ElapsedTime_h

#include "Arduino.h"

class  ElapsedTime
{
  public:
    ElapsedTime(void);
    ElapsedTime(unsigned long duration);
    void startTimer(unsigned long duration);
    boolean timeHasElapsed(void);
    long timeRemaining(void);
  private:
    unsigned long _targetTimestamp;	
};

#endif
