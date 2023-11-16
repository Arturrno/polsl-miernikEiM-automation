#include "FileInitializer.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <locale>

FileInitializer::FileInitializer() {
    initializeFile();
}

FileInitializer::~FileInitializer() {
    closeFile();
}

void FileInitializer::initializeFile() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm timeInfo;
    std::string timeString(100, 0);

    if (localtime_s(&timeInfo, &currentTime) == 0) {
        timeString.resize(std::strftime(&timeString[0], timeString.size(), "%F_%H-%M-%S", &timeInfo));
        std::cout << "Time of measurement: " << timeString << std::endl;

        std::string tempFileName = "Measurements_" + timeString + ".csv";
        outFile.open(tempFileName);
        outFile.imbue(std::locale("fr_FR.utf8"));

        if (outFile.is_open()) {
            std::cout << "File opened" << std::endl;
        }
        else {
            std::cout << "Error opening file" << std::endl;
        }
    }
    else {
        std::cout << "Error in localtime_s" << std::endl;
    }
}

void FileInitializer::closeFile() {
    if (outFile.is_open()) {
        outFile.close();
        std::cout << "File closed" << std::endl;
    }
}

