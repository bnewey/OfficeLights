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

using namespace std;


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
    if(number_of_switches > 0 && number_of_switches < 251){
        switches.push_back( new Switch(1, 1, 0, 0, "switch1", "Switch for office next to Ben's") );
        switches.push_back( new Switch(2, 2, 0, 0, "switch2", "Switch for office next to Ben's") );
        switches.push_back( new Switch(3, 3, 0, 0, "switch3", "Switch for office next to Ben's") );
        switches.push_back( new Switch(4, 4, 0, 0, "switch4", "Switch for office next to Ben's") );
        switches.push_back( new Switch(5, 5, 0, 0, "switch5", "Switch for office next to Ben's") );
        switches.push_back( new Switch(6, 6, 0, 0, "switch6", "Switch for office next to Ben's") );
        switches.push_back( new Switch(7, 7, 0, 0, "switch7", "Switch for office next to Ben's") );
        switches.push_back( new Switch(8, 8, 0, 0, "switch8", "Switch for office next to Ben's") );
        switches.push_back( new Switch(9, 9, 0, 0, "switch9", "Switch for office next to Ben's") );
        cout<<"Switch1 made"<<endl;
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
    : test_var(1), switches(cp.switches)
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

        init(1);
    }
    return *this;
}

SwitchHandler::~SwitchHandler(){
    //Delete all switches
    // auto iter = switches.begin();
    // for ( ; iter !=  switches.end(); iter++)
    // {
    //     delete (*iter);
    // }
    // switches.clear();
    // for_each( switches.begin(),switches.end(),
    //       DeleteVector<Switch*>());
}




void SwitchHandler::updateSwitches(short s1v, short s2v, short s3v){
    //Update members using params
    // auto iter = switches.begin();
    // for ( ; iter !=  switches.end(); iter++)
    // {
    //     (*iter)->updateSwitch(s1v);
    // }

    switches[0]->updateSwitch(s1v);
    switches[1]->updateSwitch(s3v);
    switches[2]->updateSwitch(s2v);
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
    
    //Iterate switches
    auto iter = switches.begin();
    for ( ; iter !=  switches.end(); iter++) {   
        vector<short> tmp_vec = (*iter)->getLightValuesFromSwitch();
        return_vector.insert(return_vector.end(), tmp_vec.begin(), tmp_vec.end() );
    }
    return return_vector;
} 

//setters

