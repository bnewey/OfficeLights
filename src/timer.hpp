#ifndef TIMER_HPP
#define TIMER_HPP

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <ctime>


// Fraction class
class Timer {

  // private: no access from outside
  private:
    
    
    int id; //index of readbuf
    int switch_id;
    float value; // timer value
    bool isTimeUp;


  public:
    // default constructor
    Timer();

    // constructor with params
    Timer( int, int, float );

    //Copy constructor
    Timer(const Timer &cp);

    // update
    bool updateTimer(float);
    

    //getters
    int getTimerId();
    int getSwitchId();
    short getTimerValue();
    bool getIsTimeUp();

    //setters 
    void setTimerId(int);
    void setSwitchId(int);
    void setTimerValue(short);

};

#endif    /* TIMER_HPP */