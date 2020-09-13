#ifndef CUBECONTROL_H
#define CUBECONTROL_H
#include <Arduino.h>

class cubeControl
{
private:
    unsigned int impulseTime = 0;
    unsigned int distance_sm = 0;
    unsigned int lastTick = 0;

public:
    //constructor
    cubeControl(/* args */);

    //destructor
    ~cubeControl();
    /*
        initialize LED matrix functional,
        and pinmode for pins connected to motors
    */
    void init();

    /*
        Set true state to led on matrix, with coordinates x and y,
        and write information

    */
    void setPixel(int x, int y);

    /*
        Set fasle state to all led's on matrix
    */
    void clearPixel();

    /*
        Set false state to led on matrix, with coordinates x and y,
        and write information
    */
    void clearOnePixel(int x, int y);

    /*
        Set signals on motor pins, to moving foward
    */
    void moveFoward();

    /*
        Set signals on motor pins, to moving backward
    */
    void moveBackward();

    /*
        Set signals on motor pins, to moving left
    */
    void moveLeft();

    /*
        Set signals on motor pins, to moving right
    */
    void moveRight();

    /*
        Set LOW signals on all motor pins
    */
    void stop();

    /*
        initializate sensors 
    */
    void sensorsInit();

    /*
        hc_sr04 ultrasonic sensor
    */
   unsigned int hc_sr04();
};

#endif