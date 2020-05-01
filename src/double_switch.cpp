//============================================================================
// Name        : double_switch.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : Handles light switch related functions
//============================================================================


// C library headers


#include "./double_switch.hpp"



DoubleSwitch::DoubleSwitch(){};

// Constructor with params
DoubleSwitch::DoubleSwitch( int id, int array_i, short value, short mode, short type, string name, string desc)
    : Switch(id, array_i, value,  mode, type, name, desc ) {
    //init(id, array_i, value, mode, type, name, desc);  
}


//Copy constructor
DoubleSwitch::DoubleSwitch( const DoubleSwitch &cp) : Switch(cp) // if we add params to child use ex ', newParam(cp.newParam)'
{}

//Copy Constructor Assignment
DoubleSwitch& DoubleSwitch::operator=(const DoubleSwitch& cp){
    Switch::operator= (cp);

    //Do the rest of assignment operator here, things are arent done in base class
    return *this;
}

//Deconstructor
DoubleSwitch::~DoubleSwitch(){
    cout<<"Destructing"<<endl;
    //Delete all move_timers
    auto iter2 = move_timers.begin();
    for ( ; iter2 !=  move_timers.end(); iter2++){
        delete (*iter2);
    }
    //Delete all toggle_timers
    auto iter3 = toggle_timers.begin();
    for ( ; iter3 !=  toggle_timers.end(); iter3++){
        delete (*iter3);
    }

    auto iter4 = delay_timers.begin();
    for ( ; iter4 !=  delay_timers.end(); iter4++)
    {
        delete (*iter4);
    }
    delay_timers.clear();
}

void DoubleSwitch::updateSwitch(short value){
    cout<<"This should not run, use updateSwitch(short value, short value2) "<<endl;
    
}

void DoubleSwitch::updateSwitch(short value, short value2){
    cout<<"Dbl Switch"<<value2<<endl;
    if(!checkLightsInit()){
        return;
    }

    this->value = value;

    if((value == 1 || value2 == 1) && mode ==0){
        this->setMoveTimer(float(1800.00));
        this->setLight(short(1));
        cout<<"Updating DoubleSwitch 1 Lights to 1"<<endl;
    }
    if((mode == 1 || value2 == 1)&& value == 1){
        //Reset Toggle Timer if movement
        this->setToggleTimer(float(1800.00));
    }
    if(value == 2 && this->delay_timers[0]->getIsTimeUp()){
        this->mode = 1; //toggle mode
        //Clear timer and toggle
        this->setMoveTimer(0.00);
        this->setToggleTimer(float(1800.00));
        this->toggleLight();
        this->delay_timers[0]->setTimerValue(float(1.00));
        cout<<"Updating DoubleSwitch 1 Lights to TOGGLE"<<endl;
    }

    
}

