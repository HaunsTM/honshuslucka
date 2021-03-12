#ifndef SWITCH_H
#define SWITCH_H

class Switch {

private:
    int _pinSwitch;
    bool _normallyOpen;

    bool _initialized;

public:
    Switch(int _pinSwitch, bool normallyOpen);

    void initialize();

    bool isPressed();
};

#endif