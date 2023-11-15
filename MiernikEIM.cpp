#include "pch.h"

#include <iostream>
#include <fstream>
#include <windows.h>
#include <filesystem>
#include <chrono>
#include <iomanip>

using namespace std;

//"VOLT1 3.141\n"       - ustawienie napiêcia SEM1 w[ V ]
//"MEAS:CURR? (@1)\n"   - pomiar pr¹du w kan. 1, czyli SEM1

//"VOLT2 6.284\n"       - ustawienie napiêcia SEM2 w[ V ]
//"MEAS:CURR? (@2)\n"   - pomiar pr¹du w kan. 2, czyli SEM2

//"CURR 2.721\n"        - ustawienie pr¹du SPM w[ mA ]
//"MEAS:VOLT? (@3)\n"   - pomiar napiêcia w kan. 3, czyli na SPM

//"MEAS:VOLT? (@4)\n"   - pomiar napiêcia w kan. 4, czyli wej AUX

HANDLE openSerialPort(char* portName)
{
	HANDLE _hSerial = CreateFileA("COM2", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (_hSerial == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		if (err == ERROR_FILE_NOT_FOUND)
		{
			std::cout << "Serial port not found.\n";
		}
		else
		{
			std::cout << "Serial port not opened. Error no " << err << "\n";
		}
		exit(1);
	}
	PurgeComm(_hSerial, PURGE_TXCLEAR | PURGE_RXCLEAR);

	// Do some basic settings
	DCB _serialParams = { 0 };
	_serialParams.DCBlength = sizeof(_serialParams);

	GetCommState(_hSerial, &_serialParams);
	_serialParams.BaudRate = CBR_115200; // 115200;
	_serialParams.ByteSize = 8;
	_serialParams.StopBits = ONESTOPBIT; //  1;
	_serialParams.Parity = NOPARITY;  // 0;
	if (!SetCommState(_hSerial, &_serialParams))
	{
		std::cout << "Could not set serial port.\n";
		exit(2);
	}

	// Set timeouts
	COMMTIMEOUTS _timeout = { 0 };
	_timeout.ReadIntervalTimeout = 50;
	_timeout.ReadTotalTimeoutConstant = 250;
	_timeout.ReadTotalTimeoutMultiplier = 10;
	_timeout.WriteTotalTimeoutConstant = 50;
	_timeout.WriteTotalTimeoutMultiplier = 10;

	SetCommTimeouts(_hSerial, &_timeout);

	std::cout << "Serial port opened!\n";
	return _hSerial;
}

void writeOrder(HANDLE hPort, const char* order)
{
	DWORD dummyArg;
	char  dummyBuf[50];
	WriteFile(hPort, order, strlen(order), &dummyArg, NULL);
	//ReadFile( hPort, dummyBuf, sizeof( dummyBuf )-1, &dummyArg, NULL );
	PurgeComm(hPort, PURGE_RXCLEAR);
}

double getMeasValue(HANDLE hPort, int n_max, double t_delay, const char* order, double prec)
{
	
//	_startTime = time.time()
	int	   _n = 0;
	double _meas = 0;
	double _pMeas = 0;

	while (_n < n_max)
	{
		//time.sleep( t_delay )
		Sleep((DWORD)(1000 * t_delay));

		writeOrder(hPort, order);
		_pMeas = _meas;

		DWORD dummyArg;
		char  inputBuf[50];
		if (!ReadFile(hPort, inputBuf, sizeof(inputBuf) - 1, &dummyArg, NULL))
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

int main()
{
	HANDLE hSerial = openSerialPort((char*)"COM2");

	double     parameterValues[] = { -0.1, -0.04, -0.02, -0.01,  0.005, 0.01, 0.02, 0.04, 0.1, 0.2 };
	double     tempParameterValues[] = { -0.1, -0.08, -0.06,-0.05, -0.04,-0.03, -0.02, -0.01,  0.005, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.2 };
	const int  noOfCurves = sizeof(parameterValues) / sizeof(double);
	const int  tempNoOfCurves = sizeof(tempParameterValues) / sizeof(double);
	double     setValues[] = { 0.03, 0.08, 0.15, 0.3, 0.4, 0.5, 0.6, 0.75, 1, 2, 3, 5, 7, 9, 11 };
	const int  noOfPoints = sizeof(setValues) / sizeof(double);
	
	double     tempMeasValues[tempNoOfCurves][noOfPoints];
	double     measValues_2[noOfCurves][noOfPoints];

	//Needed variables for table 1
	double currentValuesTab1[] = { 0.015, 0.030, 0.060 };
	double voltsValuesTab1[] = { 0.00, 0.05, 0.1, 0.15, 0.2, 0.3, 0.4, 0.5, 0.6,0.9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	//double	voltsValuesTab1[] = { 0.00, 0.05, 0.1 };
	double	measValues[sizeof(voltsValuesTab1) / sizeof(double)][sizeof(currentValuesTab1) / sizeof(double)];
	double	measValuesVolts[sizeof(voltsValuesTab1) / sizeof(double)][sizeof(currentValuesTab1) / sizeof(double)];

	std::cout << "Starting measurements" << std::endl;

	char  order[30];

	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	std::tm timeInfo;
	std::string timeString(100, 0);

	if (localtime_s(&timeInfo, &currentTime) == 0) 
	{	
		timeString.resize(std::strftime(&timeString[0], timeString.size(), "%H-%M-%S %F", &timeInfo));
		//std::cout << "Time of measurement: " << std::put_time(&timeInfo, "%T %D") << std::endl;
		std::cout << "Time of measurement: " << timeString << std::endl;
	}
	else 
	{
		std::cout << "Error in localtime_s" << std::endl;
	}

	std::string tempFileName =  string("Measurement ") + timeString + ".csv";
	std::ofstream outFile(tempFileName);
	outFile.imbue(std::locale("fr_FR.utf8"));

	std::cout << "file opened" << std::endl;

	for (int j = 0; j < sizeof(voltsValuesTab1) / sizeof(double); j++)
	{
		sprintf_s(order, "VOLT2 %f\n", voltsValuesTab1[j]);
		writeOrder(hSerial, order);

		Sleep(2500); // [ms]

		for (int i = 0; i < sizeof(currentValuesTab1) / sizeof(double); i++)
		{
			sprintf_s(order, "CURR %f\n", currentValuesTab1[i]);
			writeOrder(hSerial, order);

			Sleep(1000);

			double meas = getMeasValue(hSerial, 40, 0.1, "MEAS:CURR? (@2)\n", 0.0001);
			measValues[j][i] = -meas;

			double measVolts = getMeasValue(hSerial, 40, 0.1, "MEAS:VOLT? (@3)\n", 0.0001);
			measValuesVolts[j][i] = measVolts;
		}
	}

	outFile << "Uce [V] ; Ube ; Ic ; Ube ; Ic ; Ube ; Ic " << std::endl;
	for (size_t j = 0; j < sizeof(voltsValuesTab1) / sizeof(double); j++)
	{
		outFile << voltsValuesTab1[j];

		for (size_t i = 0; i < sizeof(currentValuesTab1) / sizeof(double); i++)
		{
			outFile << ';' << measValuesVolts[j][i];
		}
		outFile << endl;
	}

	outFile << endl << endl << endl;
	
	for (size_t j = 0; j < sizeof(voltsValuesTab1) / sizeof(double); j++)
	{
		outFile << voltsValuesTab1[j];

		for (size_t i = 0; i < sizeof(currentValuesTab1) / sizeof(double); i++)
		{
			outFile << ';' << measValues[j][i];
		}
		outFile << endl;
	}

	outFile.close();
	std::cout << "file closed" << std::endl;

	//CloseHandle(hSerial);

	return 0;
}