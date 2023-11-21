#include "SerialPort.h"

SerialPort::SerialPort(char* port) : _hSerial(INVALID_HANDLE_VALUE), portName(port) {
    if (!openSerialPort())
    {
        std::cout << "Failed to open serial port.\n";
        exit(0);
    }
}

SerialPort::~SerialPort() {
    if (_hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(_hSerial);
    }
}

bool SerialPort::openSerialPort() {
    _hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (_hSerial == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND) {
            std::cout << "Serial port not found.\n";
        }
        else {
            std::cout << "Serial port not opened. Error no " << err << "\n";
        }
        return false;
    }

    PurgeComm(_hSerial, PURGE_TXCLEAR | PURGE_RXCLEAR);

    DCB _serialParams = { 0 };
    _serialParams.DCBlength = sizeof(_serialParams);

    GetCommState(_hSerial, &_serialParams);
    _serialParams.BaudRate = CBR_115200;
    _serialParams.ByteSize = 8;
    _serialParams.StopBits = ONESTOPBIT;
    _serialParams.Parity = NOPARITY;
    if (!SetCommState(_hSerial, &_serialParams)) {
        std::cout << "Could not set serial port.\n";
        return false;
    }

    COMMTIMEOUTS _timeout = { 0 };
    _timeout.ReadIntervalTimeout = 50;
    _timeout.ReadTotalTimeoutConstant = 250;
    _timeout.ReadTotalTimeoutMultiplier = 10;
    _timeout.WriteTotalTimeoutConstant = 50;
    _timeout.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(_hSerial, &_timeout);

    std::cout << "Serial port opened!\n";
    return true;
}

void SerialPort::writeOrder(const char* order) {
    DWORD dummyArg;
    WriteFile(_hSerial, order, strlen(order), &dummyArg, NULL);
    PurgeComm(_hSerial, PURGE_RXCLEAR);
}

double SerialPort::getMeasValue(int n_max, double t_delay, const char* order, double prec) {
    
    int	   _n = 0;
    double _meas = 0;
    double _pMeas = 0;

    while (_n < n_max)
    {
        Sleep((DWORD)(1000 * t_delay));


        writeOrder(order);
        _pMeas = _meas;

        DWORD dummyArg;
        char  inputBuf[50];
        if (!ReadFile(_hSerial, inputBuf, sizeof(inputBuf) - 1, &dummyArg, NULL))
        {
            std::cout << "  --read error\n";
        }
        sscanf_s(inputBuf, "%lf", &_meas);

        std::cout << "    --- n = " << _n << " meas = " << _meas << "\n";

        //if( np.abs( _pMeas - _meas ) < np.abs( _meas ) * prec ):
        if (abs(_pMeas - _meas) < prec)
        {
            std::cout << "  --prec achieved\n";
            break;
        }
        _n = _n + 1;
    }
    if (_n == n_max)
    {
        std::cout << "  --timeout achieved\n";
    }
    return _meas;
}

double SerialPort::getVolt()
{
    return this->getMeasValue(40, 0.1, "MEAS:VOLT? (@3)\n", 0.0001);
}

double SerialPort::getCurr1()
{
    return this->getMeasValue(40, 0.1, "MEAS:CURR? (@1)\n", 0.0001);
}

double SerialPort::getCurr2()
{
    return this->getMeasValue(40, 0.1, "MEAS:CURR? (@2)\n", 0.0001);
}

void SerialPort::setVolt1(const double value)
{
    char  order[30];
    sprintf_s(order, "VOLT1 %f\n", value);
    this->writeOrder(order);
}

void SerialPort::setVolt2(const double value)
{
    char  order[30];
    sprintf_s(order, "VOLT2 %f\n", value);
    this->writeOrder(order);
}

void SerialPort::setCurr(const double value)
{
    char  order[30];
    sprintf_s(order, "CURR %f\n", value);
    this->writeOrder(order);
}

