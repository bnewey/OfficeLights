#ifndef SWITCHHANDLER_HPP
#define SWITCHHANDLER_HPP

#include <vector>
#include <jsoncpp/json/json.h>
#include <memory>
// class SingleSwitch;
// class DoubleSwitch;
// #include "./switch.hpp"
// #include "./light.hpp"

#include "./constants.hpp"
#include "./light.hpp"
#include "./switch.hpp"
#include "./single_switch.hpp"
#include "./double_switch.hpp"

// class Switch;
// #include "./single_switch.hpp"
// #include "./double_switch.hpp"
// #include "./light.hpp"

using namespace std;

// Fraction class
class SwitchHandler {

  // private: no access from outside
  private:
    //Timers will be added/removed multiple times
    bool initialized;
    vector< shared_ptr<Switch> > switches; 
    vector< shared_ptr<Light> > lights;

    void init( vector<vector<string>>, vector<vector<string>> );

  public:
    // default constructor
    SwitchHandler();

    // constructor from int (denominator)
    SwitchHandler( vector<vector<string>>, vector<vector<string>> );

    //Copy Constructor
    SwitchHandler(const SwitchHandler  &cp);

    //Copy Constructor Assignment
    SwitchHandler& operator=(const SwitchHandler& cp);

    //deconstructor
    ~SwitchHandler();

    // update
    void updateSwitches(vector<short>);

    //Timer 
    void updateTimers(float);
    
    bool setSwitchToggle(int);

    //Create Json for Ui Send
    string createJsonDataString(long numJsonSends);

    //getters
    //Light getters
    vector<short> getLightValues();
    vector<short> getLightSwitchIds();
    //Switch getters
    vector<short> getSwitchValues();
    vector<short> getModeValues();
    vector<vector<float>> getTimerValues();


    //setters  

};

#endif    /* SWITCHHANDLER_HPP */