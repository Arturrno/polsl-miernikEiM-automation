#pragma once
#include <iostream>
#include <windows.h>

//"VOLT1 3.141\n"       - ustawienie napiêcia SEM1 w[ V ]
//"MEAS:CURR? (@1)\n"   - pomiar pr¹du w kan. 1, czyli SEM1

//"VOLT2 6.284\n"       - ustawienie napiêcia SEM2 w[ V ]
//"MEAS:CURR? (@2)\n"   - pomiar pr¹du w kan. 2, czyli SEM2

//"CURR 2.721\n"        - ustawienie pr¹du SPM w[ mA ]
//"MEAS:VOLT? (@3)\n"   - pomiar napiêcia w kan. 3, czyli na SPM

//"MEAS:VOLT? (@4)\n"   - pomiar napiêcia w kan. 4, czyli wej AUX

class SerialPort {
private:
    HANDLE _hSerial;
    char* portName;
    void writeOrder(const char* order);
    double getMeasValue(int n_max, double t_delay, const char* order, double prec);
    bool openSerialPort();

public:
    SerialPort(char* port);
    ~SerialPort();

    //Get values 
    double getCurr1();
    double getVolt();
    double getCurr2();

    //Set values
    void setVolt1(const double value);
    void setCurr(const double value);
    void setVolt2(const double value);
};