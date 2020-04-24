#ifndef SWITCHHANDLER_HPP
#define SWITCHHANDLER_HPP

#include <vector>

#include "./switch.cpp"
//#include "./light.cpp"


// Fraction class
class SwitchHandler {

  // private: no access from outside
  private:
    //Timers will be added/removed multiple times
    int test_var;
    vector<Switch *> switches; 

    void init( int );

  public:
    // default constructor
    SwitchHandler();

    // constructor from int (denominator)
    SwitchHandler( int );

    //Copy Constructor
    SwitchHandler(const SwitchHandler  &cp);

    //Copy Constructor Assignment
    SwitchHandler& operator=(const SwitchHandler& cp);

    //deconstructor
    ~SwitchHandler();

    // update
    void updateSwitches(short, short, short);

    //Timer 
    void updateTimers(float);
    
    

    //getters
    vector<short> getLightValues();

    //setters  

};

#endif    /* SWITCHHANDLER_HPP */