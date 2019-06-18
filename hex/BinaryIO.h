#ifndef _BINARY_IO_H
#define _BINARY_IO_H

#include <iostream>
#include <fstream>

enum BinaryIOMode {
    None = 0,
    Read = 1,
    Write = 2
};

class BinaryIO {
    public:
    BinaryIO();
    ~BinaryIO();
    
    bool Open(std::string fileName, BinaryIOMode mode);
    void Close();
    
    template<typename T>
        void Write(T &value);
    void WriteString(std::string str);
    template<typename T>
        T Read();
    template<typename T>
        void Read(T &value);
    std::string ReadString();
    void ReadString(std::string &result);
    
    private:
    
    std::ofstream writer;
    std::ifstream reader;
    std::string filePath;
    BinaryIOMode currentMode;
    
    const std::string savePath = "../save/";
    
    bool checkReadabilityStatus();
    bool checkWritabilityStatus();
};

#endif