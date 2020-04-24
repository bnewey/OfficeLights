//============================================================================
// Name        : switch.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : Handles light switch related functions
//============================================================================


// C library headers


#include "./switch.hpp"


using namespace std;

void Switch::init(int id, int array_i, short value, short mode, string name, string desc){
    this->id = id; 
    this->array_index = array_i; 
    this->value = value; 
    this->mode = mode;
    this->name = name;
    this->description = desc;

    this->lights.push_back( new Light(1, 251, 1,  0, "light1", "desc1") );
    this->move_timers.push_back( new Timer(1, 1, 0) );
    this->toggle_timers.push_back( new Timer(2, 1, 0));
    this->delay_timer = new Timer(3,1,0);
}

//Default Constructor
Switch::Switch(){

    //start with stop mode 1

    //(un)provided settings
    init(1,251,0,0, "defaultLight", "Default constructor was used");

    //
}

// Constructor with int  current_mode passed
Switch::Switch( int id, int array_i, short value, short mode, string name, string desc) {
    init(id, array_i, value, mode, name, desc);  
}

//Copy constructor
Switch::Switch( const Switch &cp)
    : id(cp.id), array_index(cp.array_index), value(cp.value), mode(cp.mode), name(cp.name), 
        description(cp.description), lights(cp.lights), move_timers(cp.move_timers), toggle_timers(cp.toggle_timers), delay_timer(cp.delay_timer)
{}

//Copy Constructor Assignment
Switch& Switch::operator=(const Switch& cp){
    if(this != &cp){
        auto iter = lights.begin();
        for ( ; iter !=  lights.end(); iter++)
        {
            delete (*iter);
        }
        lights.clear();

        auto iter2 = move_timers.begin();
        for ( ; iter2 !=  move_timers.end(); iter2++)
        {
            delete (*iter2);
        }
        move_timers.clear();

        auto iter3 = toggle_timers.begin();
        for ( ; iter3 !=  toggle_timers.end(); iter3++)
        {
            delete (*iter3);
        }
        toggle_timers.clear();

        delete delay_timer;

        init(cp.id, cp.array_index, cp.value,cp.mode, cp.name, cp.description);
    }
    return *this;
}

//Deconstructor
Switch::~Switch(){
    cout<<"Destructing"<<endl;
    //Delete all lights
    auto iter = lights.begin();
    for ( ; iter !=  lights.end(); iter++){
        delete (*iter);
    }
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

    delete delay_timer;
}

void Switch::updateSwitch(short value){
    this->value = value;
    //this->value = value;
    cout<<"SWITCH ID:  "<<id<<endl;
    if(value == 0){
        cout<<"value is 0"<<endl;
    }

    if(value == 1 && mode ==0){
        this->setMoveTimer(float(360.00));
        this->setLight(short(1));
        cout<<"Updating Switch 1 Lights to 1"<<endl;
    }
    if(mode == 1 && value == 1){
        //Reset Toggle Timer if movement
        this->setToggleTimer(float(360.00));
    }
    if(value == 2 && this->delay_timer->getIsTimeUp()){
        this->mode = 1; //toggle mode
        //Clear timer and toggle
        this->setMoveTimer(0.00);
        this->setToggleTimer(float(15.00));
        this->toggleLight();
        this->delay_timer->setTimerValue(float(1.00));
        cout<<"Updating Switch 1 Lights to TOGGLE"<<endl;
    }

    
}


void Switch::updateTimer(float seconds_passed){
    cout<<"Timer Updated in Switch: "<<seconds_passed<<endl;

    //Delay Timer update for toggle mode
    bool isDelayUp = delay_timer->updateTimer(seconds_passed);

    // TOGGLE MODE
    if(this->mode == 1){ 
        auto iter2 = toggle_timers.begin();
        for ( ; iter2 !=  toggle_timers.end(); iter2++){
            bool isTimeUp = (*iter2)->updateTimer(seconds_passed);
            if(isTimeUp && this->mode == 1){ //1 = toggle mode
                this->mode = 0;
                this->setLight(short(0));
                cout<<endl<<endl<<"TOGGLE TIME IS UP !!!!!!!!!!!!!!!!!!!!!!!!"<<endl<<endl;
            }
        }
    }

    // MOVEMENT / UI MODE
    if(this->mode == 0){ 
        auto iter1 = move_timers.begin();
        for ( ; iter1 !=  move_timers.end(); iter1++){
            bool isTimeUp = (*iter1)->updateTimer(seconds_passed);
            if(isTimeUp && this->mode == 0){
                this->value = 0;
                this->setLight(short(0));
                cout<<endl<<endl<<"TIME IS UP !!!!!!!!!!!!!!!!!!!!!!!!"<<endl<<endl;
            }
        }
    }   


} 

void Switch::setMoveTimer(float time_to_set){
    auto iter1 = move_timers.begin();
    for ( ; iter1 !=  move_timers.end(); iter1++){
        (*iter1)->setTimerValue(float(time_to_set));
    }
}

void Switch::setToggleTimer(float time_to_set){
    auto iter1 = toggle_timers.begin();
    for ( ; iter1 !=  toggle_timers.end(); iter1++){
        (*iter1)->setTimerValue(float(time_to_set));
    }
}

void Switch::setDelayTimer(float time_to_set){
    this->delay_timer->setTimerValue(float(time_to_set));
}

void Switch::setLight(short new_light_value){
    auto iter1 = lights.begin();
    for ( ; iter1 !=  lights.end(); iter1++){
        (*iter1)->setLightValue(short(new_light_value));
    }
}

void Switch::toggleLight(){
    auto iter1 = lights.begin();
    for ( ; iter1 !=  lights.end(); iter1++){
        (*iter1)->setLightValue(!(*iter1)->getLightValue());
    }
}




//getters
vector<short> Switch::getLightValuesFromSwitch(){
    vector<short> return_vector;
    
    //Iterate switches
    auto iter = lights.begin();
    for ( ; iter !=  lights.end(); iter++){   
        short tmp = (*iter)->getLightValue();
        return_vector.push_back( tmp ); 
    }
    
    return(return_vector);
}

int Switch::getSwitchId(){
    return this->id;
}

int Switch::getSwitchArrayIndex(){
    return this->array_index;
}
short Switch::getSwitchValue(){
    return this->value;
}
string Switch::getSwitchName(){
    return this->name;
}
string Switch::getSwitchDesc(){
    return this->description;
}

//setters
void Switch::setSwitchId(int id){
    if(id < 0){
        return;
    }
    this->id = id;
}
void Switch::setSwitchArrayIndex(int array_index){
    if(array_index < 0){
        return;
    }
    this->array_index = array_index;
}
void Switch::setSwitchValue(short value){
    if(value > 2 || value < 0){
        return;
    }
    this->value = value;
}
void Switch::setSwitchName(string name){
    this->name = name;
}
void Switch::setSwitchDescription(string desc){
    this->description = desc;
}
