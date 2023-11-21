#include "SerialPort.h"
#include "FileInitializer.h"
#include "Variables.h"

#define DEBUG_MODE 1
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))


int main()
{
	char portName[] = "COM2";
	SerialPort tester(portName);
	FileInitializer outFile;

	double measValues_1[ARRAY_SIZE(volt1)][ARRAY_SIZE(volt1)]; //Measurements for the first table
	double UGSoff = 0; 

#if DEBUG_MODE == 1
	
	for (size_t i = 0; i < ARRAY_SIZE(volt1); i++)
	{
		tester.setVolt1(volt1[i]);

		for (size_t j = 0; j < ARRAY_SIZE(volt2); j++)
		{
			tester.setVolt2(volt2[j]);
			measValues_1[i][j] = tester.getCurr1();
		}
	}

	//Writing the results to an external file
	outFile << "Id;Id;Id" << std::endl;
	for (size_t i = 0; i < ARRAY_SIZE(volt1); i++)
	{
		outFile << volt1[i] << ";";
		for (size_t j = 0; j < ARRAY_SIZE(volt2); j++)
		{
			outFile << measValues_1[i][j] << ";";
		}

		outFile << std::endl;
	}
	
#else

	if (serial.openSerialPort()) 
	{
		const char* order = "your_order";

		double measurement = serial.getMeasValue(10, 1.0, order, 0.001); // Example of a function call

		

		std::cout << "Starting measurements..." << std::endl;


		// Do something with the measurement
		
	} 
	else 
	{
		std::cout << "Failed to open serial port.\n";
	}


	//char  order[30];

	//for (int j = 0; j < sizeof(voltsValuesTab1) / sizeof(double); j++)
	//{
	//	sprintf_s(order, "VOLT2 %f\n", voltsValuesTab1[j]);
	//	serial.writeOrder(order);

	//	Sleep(2500); // [ms]

	//	for (int i = 0; i < sizeof(currentValuesTab1) / sizeof(double); i++)
	//	{
	//		sprintf_s(order, "CURR %f\n", currentValuesTab1[i]);
	//		serial.writeOrder(order);

	//		Sleep(1000);

	//		double meas = serial.getMeasValue(40, 0.1, "MEAS:CURR? (@2)\n", 0.0001);
	//		measValues[j][i] = -meas;

	//		double measVolts = serial.getMeasValue(40, 0.1, "MEAS:VOLT? (@3)\n", 0.0001);
	//		measValuesVolts[j][i] = measVolts;
	//	}
	//}

	//outFile << "Uce [V] ; Ube ; Ic ; Ube ; Ic ; Ube ; Ic \n";
	//

	//for (size_t j = 0; j < sizeof(voltsValuesTab1) / sizeof(double); j++)
	//{
	//	outFile << voltsValuesTab1[j];

	//	for (size_t i = 0; i < sizeof(currentValuesTab1) / sizeof(double); i++)
	//	{
	//		outFile << ';' << measValuesVolts[j][i];
	//	}
	//	outFile << "\n";
	//}

	//outFile << "\n\n\n";
	//
	//for (size_t j = 0; j < sizeof(voltsValuesTab1) / sizeof(double); j++)
	//{
	//	outFile << voltsValuesTab1[j];

	//	for (size_t i = 0; i < sizeof(currentValuesTab1) / sizeof(double); i++)
	//	{
	//		outFile << ';' << measValues[j][i];
	//	}
	//	outFile << "\n";
	//}
#endif

	return 0;
}