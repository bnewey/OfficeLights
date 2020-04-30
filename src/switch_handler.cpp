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


void SwitchHandler::init(int number_of_switches){
    if(number_of_switches > 0 && number_of_switches < 151){
        //Lights
        vector<Light *> tmpLightVec(2);
        tmpLightVec.clear();

        lights.push_back( new Light(1, 151, 1,  0, "Bens", "desc1") );
        tmpLightVec.push_back( lights[0] );
        switches.push_back( new SingleSwitch(1, 0, 0, 0,0, "Bens", "SingleSwitch for Bens office", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(2, 152, 2,  0, "Mikes", "desc2") );
        tmpLightVec.push_back( lights[1] );
        switches.push_back( new SingleSwitch(2, 2, 0, 0, 0, "Mikes", "Mikes office", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(3, 153, 3,  0, "Frames", "desc3") );
        tmpLightVec.push_back( lights[2] );
        switches.push_back( new SingleSwitch(3, 4, 0, 0, 0, "Frames", "Frames office", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(4, 154, 4,  0, "Conference", "desc4") );
        tmpLightVec.push_back( lights[3] );
        switches.push_back( new SingleSwitch(4, 5, 0, 0, 0,"Conference Room", "Conference Room", tmpLightVec) );
        tmpLightVec.clear();
        
        lights.push_back( new Light(5, 155, 5,  0, "Holding", "desc5") );
        tmpLightVec.push_back( lights[4] );
        switches.push_back( new SingleSwitch(5, 6, 0, 0,0, "Holding", "Holding", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(6, 156, 6,  0, "Jeremy", "desc6") );
        tmpLightVec.push_back( lights[5] );
        switches.push_back( new SingleSwitch(6, 7, 0, 0,0, "Jeremy Office", "Jeremys office", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(7, 157, 7,  0, "Weston", "desc7") );
        tmpLightVec.push_back( lights[6] );
        switches.push_back( new SingleSwitch(7, 8, 0, 0,0, "Weston", "Westons office", tmpLightVec) );
        tmpLightVec.clear();

        lights.push_back( new Light(8, 158, 8,  0, "Joe", "desc8") );
        tmpLightVec.push_back( lights[7] );
        switches.push_back( new SingleSwitch(8, 8, 0, 0,0, "Joe", "Joes office", tmpLightVec) );
        tmpLightVec.clear();

        //Double Switch
        lights.push_back( new Light(9, 159, 9,  0, "light2", "double switch light9") );
        tmpLightVec.push_back( lights[8] );
        switches.push_back( new DoubleSwitch(9, 1, 0, 0,1, "Front/Back Hall", "Front/Back Hall Dbl Switch", tmpLightVec) );
        tmpLightVec.clear();
        //

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

        init(8);
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
    
    for_each( switches.begin(),switches.end(),
          DeleteVector<Switch*>());

    for_each( lights.begin(),lights.end(),
          DeleteVector<Light*>());

}




void SwitchHandler::updateSwitches(vector<short> switch_values){
    //Update members using params
    // auto iter = switches.begin();
    // for ( ; iter !=  switches.end(); iter++)
    // {
    //     int array_index = (*iter)->getSwitchArrayIndex();
    //     (*iter)->updateSwitch(array_index);
    // }

    for(int i=0; i<    8   ; i++){
        int switch_type = switches[i]->getSwitchType();
        if(switch_type == 0){
            int array_index = switches[i]->getSwitchArrayIndex();
            switches[i]->updateSwitch(switch_values[array_index]);
        }
        //Hacky but works for now
        //works only if double switches are back to back in array_index until i add array_index2 into double_switch
        if(switch_type == 1){
            int array_index = switches[i]->getSwitchArrayIndex();
            switches[i]->updateSwitch(switch_values[array_index], switch_values[array_index + 1]);
        }        

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

