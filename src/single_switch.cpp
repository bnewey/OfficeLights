//============================================================================
// Name        : switch.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : Handles light switch related functions
//============================================================================


// C library headers


#include "./single_switch.hpp"



SingleSwitch::SingleSwitch(){};

// Constructor with int  current_mode passed
SingleSwitch::SingleSwitch( int id, int array_i, short value, short mode,short type, string name, string desc)
    : Switch(id, array_i, value,  mode, type,  name, desc ) {
    //init(id, array_i, value, mode, type, name, desc);  
}


//Copy constructor
SingleSwitch::SingleSwitch( const SingleSwitch &cp) : Switch(cp) // if we add params to child use ex ', newParam(cp.newParam)'
{}

//Copy Constructor Assignment
SingleSwitch& SingleSwitch::operator=(const SingleSwitch& cp){
    Switch::operator= (cp);

    //Do the rest of assignment operator here, things are arent done in base class
    return *this;
}

//Deconstructor
SingleSwitch::~SingleSwitch(){
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

void SingleSwitch::updateSwitch(short value){
    cout<<"Single Switch"<<endl;
    if(!checkLightsInit()){
        return;
    }

    this->value = value;

    if(value == 1 && mode ==0){
        this->setMoveTimer(float(900.00));
        this->setLight(short(1));
        cout<<"Updating SingleSwitch 1 Lights to 1"<<endl;
    }
    if(mode == 1 && value == 1){
        //Reset Toggle Timer if movement
        this->setToggleTimer(float(900.00));
    }
    if(value == 2 && this->delay_timers[0]->getIsTimeUp()){
        this->mode = 1; //toggle mode
        //Clear timer and toggle
        this->setMoveTimer(0.00);
        this->setToggleTimer(float(900.00));
        this->toggleLight();
        this->delay_timers[0]->setTimerValue(float(1.00));
        cout<<"Updating SingleSwitch 1 Lights to TOGGLE"<<endl;
    }

    
}

void SingleSwitch::updateSwitch(short value, short value2){
    cout<<"This should not run, use updateSwitch(short value) instead."<<endl;

    
}

