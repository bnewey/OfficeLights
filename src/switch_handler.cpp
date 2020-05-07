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
    if(!(switch_config.size() >= 0 && light_config.size() >= 0)){
        return;
    }

    //Create SWITCHES using config
    int sw_con_size = switch_config.size();
    for(int i=0; i<sw_con_size; i++){
        //Check mode: 0==single 1==double switch
        if(stoi(switch_config[i][2]) == 0){
            switches.push_back( new SingleSwitch(stoi(switch_config[i][0]), stoi(switch_config[i][1]), 0, 0, stoi(switch_config[i][2]), string(switch_config[i][3]), string(switch_config[i][4])) );    
        }
        else if(stoi(switch_config[i][2]) == 1){
            switches.push_back( new DoubleSwitch(stoi(switch_config[i][0]), stoi(switch_config[i][1]), 0, 0, stoi(switch_config[i][2]), string(switch_config[i][3]), string(switch_config[i][4])) );  
        }
    }
    
    //Create LIGHTS using config
    int light_con_size = light_config.size();
    for(int i=0; i<light_con_size; i++){
        lights.push_back( new Light(stoi(light_config[i][0]), stoi(light_config[i][1]), stoi(light_config[i][2]),  0, string(light_config[i][4]), string(light_config[i][5])) );
        
        int light_switch_id = stoi(light_config[i][2]);

        //Add light to its configured switch with switch_id
        int sw_size = switches.size();
        for(int i=0; i<sw_size; i++){
            if(light_switch_id == switches[i]->getSwitchId()){
                switches[i]->addLightToSwitch(lights[i]);
            }
        }

    }


    cout<<"Switch Handler done"<<endl;

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

        //init(8); //idk how to do this 
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

    int sw_size = switches.size();
    for(int i=0; i<    sw_size   ; i++){
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

bool SwitchHandler::setSwitchToggle(int idToToggle){
    //Find switch with id
    auto iter = switches.begin();
    for ( ; iter !=  switches.end(); iter++ )
    {   
        if((*iter)->getSwitchId() == idToToggle){
            (*iter)->toggleLight();
            //Set Mode to 0 because this is coming from UI and shouldnt 
            (*iter)->setModeValue(0);
            return true;
        } 
    }
    return false;
}


//getters
//Light Getters
vector<short> SwitchHandler::getLightValues(){
    vector<short> return_vector(150,0);
    
    int l_size = lights.size();
    for(int i=0; i<(l_size);i++){
        int tmp = (lights[i]->getLightArrayIndex()-151);
        return_vector[tmp] = lights[i]->getLightValue();
    }
    // auto iter = lights.begin();
    // for ( ; iter !=  lights.end(); iter++){   
    //     short tmp = (*iter)->getLightValue();
    //     return_vector.push_back( tmp ); 
    // }
    return return_vector;
} 

vector<short> SwitchHandler::getLightSwitchIds(){
    vector<short> return_vector(150,0);
    
    int l_size = lights.size();
    for(int i=0; i<(l_size);i++){
        int tmp = (lights[i]->getLightArrayIndex()-151);
        return_vector[tmp] = lights[i]->getSwitchId();
    }

    return return_vector;
}

//Switch Getters
vector<short> SwitchHandler::getSwitchValues(){
    vector<short> return_vector(150,0);
    
    int l_size = switches.size();
    for(int i=0; i<(l_size);i++){
        int tmp = (switches[i]->getSwitchArrayIndex());
        return_vector[tmp] = switches[i]->getSwitchValue();
    }
    return return_vector;
}

vector<short> SwitchHandler::getModeValues(){
    vector<short> return_vector(150,0);
    
    int l_size = switches.size();
    for(int i=0; i<(l_size);i++){
        int tmp = (switches[i]->getSwitchArrayIndex());
        return_vector[tmp] = switches[i]->getModeValue();
    }
    return return_vector;
}

vector<vector<float>> SwitchHandler::getTimerValues(){
    vector<vector<float>> return_vector(150,vector<float>(3));
    
    int l_size = switches.size();
    for(int i=0; i<(l_size);i++){
        int tmp = (switches[i]->getSwitchArrayIndex());
        return_vector[tmp] = switches[i]->getTimerValues();
    }
    return return_vector;
} 


//setters

