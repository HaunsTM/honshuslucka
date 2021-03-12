#ifndef ACTUATOR_H
#define ACTUATOR_H

class Actuator {

  private:
    int _pinRelay1;
     
    int _pinRelay2;

    bool _initialized;

  public:
    Actuator(int pinRelay1, int pinRelay2);
    
    void initialize();
    
    void push();
        
    void pull();
    
    void turnOFF();
};

#endif