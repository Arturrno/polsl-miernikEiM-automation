#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

class FileInitializer {
private:
    std::ofstream outFile;

public:
    FileInitializer();
    ~FileInitializer();

    void initializeFile();
    void closeFile();

    template <typename T>
    FileInitializer& operator<<(const T& data);

};

template <typename T>
FileInitializer& FileInitializer::operator<<(const T& data) {
    if (outFile.is_open()) {
        outFile << data;
    }
    else {
        std::cout << "File is not open!" << std::endl;
    }
    return *this;
}

