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


void SwitchHandler::init( vector<vector<string>> switch_config, vector<vector<string>> light_config){
    if(switch_config.size() > -1 && light_config.size() > -1){


        int sw_con_size = switch_config.size();
        for(int i=0; i<sw_con_size; i++){
            switches.push_back( new SingleSwitch(switch_config[i][0], switch_config[i][1], 0, 0,0, switch_config[i][3], switch_config[i][4]) );
        }
        
        int light_con_size = light_config.size();
        for(int i=0; i<light_con_size; i++){
            lights.push_back( new Light(light_config[i][0], light_config[i][1], light_config[i][2],  0, light_config[i][4], light_config[i][5]) );
        }

        

        //Lights
        // vector<Light *> tmpLightVec(2);
        // tmpLightVec.clear();

        // lights.push_back( new Light(1, 151, 1,  0, "light1", "desc1") );
        // tmpLightVec.push_back( lights[0] );
        // switches.push_back( new SingleSwitch(1, 0, 0, 0,0, "switch1", "SingleSwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();

        // //Double Switch
        // lights.push_back( new Light(1, 152, 1,  0, "light2", "double switch light") );
        // tmpLightVec.push_back( lights[1] );
        // switches.push_back( new DoubleSwitch(2, 1, 0, 0,1, "switch2", "DOUBLESwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();
        // //

        // lights.push_back( new Light(1, 153, 1,  0, "light4", "desc4") );
        // tmpLightVec.push_back( lights[2] );
        // switches.push_back( new SingleSwitch(3, 3, 0, 0, 0, "switch4", "SingleSwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();

        // lights.push_back( new Light(1, 154, 1,  0, "light5", "desc5") );
        // tmpLightVec.push_back( lights[3] );
        // switches.push_back( new SingleSwitch(4, 4, 0, 0, 0, "switch5", "SingleSwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();

        // lights.push_back( new Light(1, 155, 1,  0, "light6", "desc6") );
        // tmpLightVec.push_back( lights[4] );
        // switches.push_back( new SingleSwitch(5, 5, 0, 0, 0,"switch6", "SingleSwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();
        
        // lights.push_back( new Light(1, 156, 1,  0, "light7", "desc7") );
        // tmpLightVec.push_back( lights[5] );
        // switches.push_back( new SingleSwitch(6, 6, 0, 0,0, "switch7", "SingleSwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();

        // lights.push_back( new Light(1, 157, 1,  0, "light8", "desc8") );
        // tmpLightVec.push_back( lights[6] );
        // switches.push_back( new SingleSwitch(7, 7, 0, 0,0, "switch8", "SingleSwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();

        // lights.push_back( new Light(1, 158, 1,  0, "light9", "desc9") );
        // tmpLightVec.push_back( lights[7] );
        // switches.push_back( new SingleSwitch(8, 8, 0, 0,0, "switch9", "SingleSwitch for office next to Ben's", tmpLightVec) );
        // tmpLightVec.clear();

        //cout<<"SingleSwitch1 made"<<endl;

        
    }

    this->initialized = true;
}

//Default Constructor
SwitchHandler::SwitchHandler(){

    //start with stop mode 1

    //(un)provided settings

    //
}

// Constructor with int  current_mode passed
SwitchHandler::SwitchHandler( vector<vector<string>> switch_config, vector<vector<string>> light_config){
    this->initialized = false;
    this->init(switch_config, light_config);

}

//Copy Constructor
SwitchHandler::SwitchHandler(const SwitchHandler  &cp)
    : initialized(cp.initialized), switches(cp.switches), lights(cp.lights)
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
    if(!(this->initialized == true)){
        cout<<"Not yet initialized"<<endl;
        return;
    }


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
    vector<short> return_vector(150,0);
    
    int l_size = lights.size();
    for(int i=0; i<(l_size);i++){
        int tmp = lights[i]->getLightArrayIndex();
        return_vector[tmp] = lights[i]->getLightValue();
    }
    // auto iter = lights.begin();
    // for ( ; iter !=  lights.end(); iter++){   
    //     short tmp = (*iter)->getLightValue();
    //     return_vector.push_back( tmp ); 
    // }

    return return_vector;
} 

//setters

