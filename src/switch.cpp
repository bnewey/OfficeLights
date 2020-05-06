//============================================================================
// Name        : switch.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : Handles light switch related functions
//============================================================================


// C library headers


#include "./switch.hpp"



void Switch::init(int id, int array_i, short value, short mode, short type, string name, string desc){
    this->id = id; 
    this->array_index = array_i; 
    this->value = value; 
    this->mode = mode;
    this->type = type;
    this->name = name;
    this->description = desc;

    //this->lights = lights;
    this->move_timers.push_back( new Timer(1, 1, 0) );
    this->toggle_timers.push_back( new Timer(2, 1, 0));
    this->delay_timers.push_back(new Timer(3,1,0));
}

//Default Constructor
Switch::Switch(){

    //start with stop mode 1

    //(un)provided settings
    //init(1,251,0,0, "defaultLight", "Default constructor was used", );

    //
}

// Constructor with int  current_mode passed
Switch::Switch( int id, int array_i, short value, short mode, short type, string name, string desc) {
    init(id, array_i, value, mode,type, name, desc);  
}

//Copy constructor
Switch::Switch( const Switch &cp)
    : id(cp.id), array_index(cp.array_index), value(cp.value), mode(cp.mode), type(cp.type), name(cp.name), 
        description(cp.description), lights(cp.lights), move_timers(cp.move_timers), toggle_timers(cp.toggle_timers), delay_timers(cp.delay_timers)
{}

//Copy Constructor Assignment
Switch& Switch::operator=(const Switch& cp){
    if(this != &cp){    

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

        auto iter4 = delay_timers.begin();
        for ( ; iter4 !=  delay_timers.end(); iter4++)
        {
            delete (*iter4);
        }
        delay_timers.clear();

        init(cp.id, cp.array_index, cp.value,cp.mode,cp.type, cp.name, cp.description);
    }
    return *this;
}

//Deconstructor
Switch::~Switch(){
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



void Switch::updateTimer(float seconds_passed){

    //Delay Timer update for toggle mode
    bool isDelayUp = delay_timers[0]->updateTimer(seconds_passed);

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

bool Switch::checkLightsInit(){
    auto iter1 = lights.begin();
    for ( ; iter1 !=  lights.end(); iter1++){
        if(!((*iter1)->isInitialized())){
            cout<<"Bad Initialization"<<endl;
            return false;
        }
    }
    return true;
}

//Timers
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
    auto iter1 = delay_timers.begin();
    for ( ; iter1 !=  delay_timers.end(); iter1++){
        (*iter1)->setTimerValue(float(time_to_set));
    }
}

vector<float> Switch::getTimerValues(){
    vector<float> return_vector(3,0);
    float tmp = move_timers[0]->getTimerValue();
    if(tmp < 0){
        tmp =0;
    }
    return_vector[0] = float(tmp);

    tmp = toggle_timers[0]->getTimerValue();
    if(tmp < 0){
        tmp =0;
    }
    return_vector[1] = float(tmp);

    tmp = delay_timers[0]->getTimerValue();
    if(tmp < 0){
        tmp =0;
    }
    return_vector[2] = float(tmp);


    return return_vector;
}

//Lights
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

void Switch::addLightToSwitch(Light * lightToAdd){
    this->lights.push_back(lightToAdd);
}


//getters

int Switch::getSwitchId(){
    return this->id;
}
int Switch::getSwitchArrayIndex(){
    return this->array_index;
}
short Switch::getSwitchValue(){
    return this->value;
}
short Switch::getSwitchType(){
    return this->type;
}
short Switch::getModeValue(){
    return this->mode;
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
void Switch::setModeValue(short value){
    if(value != 0 || value != 1){
        return;
    }
    this->setModeValue(value);
}
void Switch::setSwitchName(string name){
    this->name = name;
}
void Switch::setSwitchDescription(string desc){
    this->description = desc;
}

