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

    template<typename T>
    FileInitializer& operator<<(const T& data) {
        if (outFile.is_open()) {
            outFile << data;
        }
        else {
            std::cout << "Unable to write to file" << std::endl;
        }
        return *this;
    }

    // Obs³uga specjalna dla endl
    FileInitializer& operator<<(std::ostream& (*manip)(std::ostream&)) {
        if (outFile.is_open()) {
            manip(outFile);
        }
        else {
            std::cout << "Unable to write to file!" << std::endl;
        }
        return *this;
    }
};

