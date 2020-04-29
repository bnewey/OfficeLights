//============================================================================
// Name        : switch_handler.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : Handles light switch related functions
//============================================================================


// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "functional"
#include "algorithm"

#include <ctime>

#include "./switch_handler.hpp"
#include "./light.hpp"
#include "./switch.hpp"
#include "./single_switch.hpp"
#include "./double_switch.hpp"


// Functor for deleting pointers in vector.
template<class T> class DeleteVector
{
    public:
    // Overloaded () operator.
    // This will be called by for_each() function.
    bool operator()(T x) const
    {
        // Delete pointer.
        delete x;
        return true;
    }
};

//MONDAY:
//RECAP:
// i just moved lights vector from Switch to SwitchHandler, causing memory problems,
// get All in One working to further test

void SwitchHandler::init(int number_of_switches){
    if(number_of_switches > 0 && number_of_switches < 251){
        //Lights
        vector<Light *> tmpLightVec(2);
        tmpLightVec.clear();

        lights.push_back( new Light(1, 251, 1,  0, "light1", "desc1") );
        tmpLightVec.push_back( lights[0] );
        switches.push_back( new SingleSwitch(1, 0, 0, 0, "switch1", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(1, 252, 1,  0, "light2", "desc2") );
        tmpLightVec.push_back( lights[1] );
        switches.push_back( new SingleSwitch(2, 2, 0, 0, "switch2", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(1, 253, 1,  0, "light3", "desc3") );
        tmpLightVec.push_back( lights[2] );
        switches.push_back( new SingleSwitch(3, 1, 0, 0, "switch3", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(1, 254, 1,  0, "light4", "desc4") );
        tmpLightVec.push_back( lights[3] );
        switches.push_back( new SingleSwitch(4, 3, 0, 0, "switch4", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(1, 255, 1,  0, "light5", "desc5") );
        tmpLightVec.push_back( lights[4] );
        switches.push_back( new SingleSwitch(5, 4, 0, 0, "switch5", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(1, 256, 1,  0, "light6", "desc6") );
        tmpLightVec.push_back( lights[5] );
        switches.push_back( new SingleSwitch(6, 5, 0, 0, "switch6", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();
        
        lights.push_back( new Light(1, 257, 1,  0, "light7", "desc7") );
        tmpLightVec.push_back( lights[6] );
        switches.push_back( new SingleSwitch(7, 6, 0, 0, "switch7", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(1, 258, 1,  0, "light8", "desc8") );
        tmpLightVec.push_back( lights[7] );
        switches.push_back( new SingleSwitch(8, 7, 0, 0, "switch8", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(1, 259, 1,  0, "light9", "desc9") );
        tmpLightVec.push_back( lights[8] );
        switches.push_back( new SingleSwitch(9, 8, 0, 0, "switch9", "SingleSwitch for office next to Ben's", tmpLightVec) );
        tmpLightVec.clear();

        cout<<"SingleSwitch1 made"<<endl;

        
    }

    test_var = 1;
}

//Default Constructor
SwitchHandler::SwitchHandler(){

    //start with stop mode 1

    //(un)provided settings

    //
}

// Constructor with int  current_mode passed
SwitchHandler::SwitchHandler( int number_of_switches){
    this->init(number_of_switches);

}

//Copy Constructor
SwitchHandler::SwitchHandler(const SwitchHandler  &cp)
    : test_var(1), switches(cp.switches), lights(cp.lights)
{}

//Copy Constructor Assignment
SwitchHandler& SwitchHandler::operator=(const SwitchHandler& cp){
    if(this != &cp){
        auto iter = switches.begin();
        for ( ; iter !=  switches.end(); iter++)
        {
            delete (*iter);
        }
        switches.clear();

        auto iter2 = lights.begin();
        for ( ; iter2 !=  lights.end(); iter2++)
        {
            delete (*iter2);
        }
        lights.clear();

        init(1);
    }
    return *this;
}

SwitchHandler::~SwitchHandler(){
    //~SwitchHandler gets called every loop,which causes our vectors to be deleted during runtime
    //Delete all switches
    // auto iter = switches.begin();
    // for ( ; iter !=  switches.end(); iter++)
    // {
    //     delete (*iter);
    // }
    // switches.clear();
    // auto iter2 = lights.begin();
    // for ( ; iter2 !=  lights.end(); iter2++)
    // {
    //     delete (*iter2);
    // }
    // lights.clear();
    
    // for_each( switches.begin(),switches.end(),
    //       DeleteVector<Switch*>());
}




void SwitchHandler::updateSwitches(vector<short> switch_values){
    //Update members using params
    // auto iter = switches.begin();
    // for ( ; iter !=  switches.end(); iter++)
    // {
    //     int array_index = (*iter)->getSwitchArrayIndex();
    //     (*iter)->updateSwitch(array_index);
    // }

    for(int i=0; i<    9   ; i++){
        int array_index = switches[i]->getSwitchArrayIndex();
        switches[i]->updateSwitch(switch_values[array_index]);

    }


    
    //Switch Logic
    
    
}

void SwitchHandler::updateTimers(float seconds_passed){
    cout<<"Timer Updated SH: "<<seconds_passed<<endl;
    auto iter = switches.begin();
    for ( ; iter !=  switches.end(); iter++)
    {
        (*iter)->updateTimer(float(seconds_passed));
    }
} 




//getters
vector<short> SwitchHandler::getLightValues(){
    vector<short> return_vector;
    
    auto iter = lights.begin();
    for ( ; iter !=  lights.end(); iter++){   
        short tmp = (*iter)->getLightValue();
        return_vector.push_back( tmp ); 
    }

    return return_vector;
} 

//setters

