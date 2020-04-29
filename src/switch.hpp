#ifndef SWITCH_HPP
#define SWITCH_HPP

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <ctime>

#include "./light.hpp"
#include "./timer.hpp"


using namespace std;

// Fraction class
class Switch {

  // private: no access from outside
  protected:
    
    
    int id; //index of readbuf
    int array_index; //position in read_buf
    short value; // 0==no movement 1=movement 2=toggle
    short mode; // 0==no overrider 1=toggle override 
    string name;
    string description;

    vector<Light *> lights;
    vector<Timer *> move_timers; 
    vector<Timer *> toggle_timers;  
    Timer * delay_timer;

    bool checkLightsInit();

    public:
    // default constructor
    Switch();

    // constructor with params
    Switch( int, int, short, short,  string, string, vector<Light *> );

    void init(int , int , short, short , string , string, vector<Light *> );

    //Copy constructor
    Switch( const Switch &cp);

    //Copy assignment
    Switch& operator=(const Switch& cp);

    //Destructor
    ~Switch();

    // update
    virtual void updateSwitch(short) =0;
    //update with two values
    virtual void updateSwitch(short, short) = 0;

    //Timer 
    void updateTimer(float);
    void setMoveTimer(float);
    void setToggleTimer(float);
    void setDelayTimer(float);

    //Lights
    void setLight(short );
    void toggleLight();
    

    //getters
    int getSwitchId();
    int getSwitchArrayIndex();
    short getSwitchValue();
    string getSwitchName();
    string getSwitchDesc();

    //setters 
    void setSwitchId(int);
    void setSwitchArrayIndex(int);
    void setSwitchValue(short);
    void setSwitchName(string);
    void setSwitchDescription(string); 

};

#endif    /* SWITCH_HPP */