#ifndef _BINARY_IO_H
#define _BINARY_IO_H

#include <iostream>
#include <fstream>
#include <string>

enum BinaryIOMode {
    None = 0,
    Read = 1,
    Write = 2
};

class BinaryIO {
    public:
    BinaryIO() {
        currentMode = BinaryIOMode::None;
    }
    
    ~BinaryIO() {
        if (writer.is_open()) writer.close();
        if (reader.is_open()) reader.close();
    }
    
    bool Open(std::string fileName, BinaryIOMode mode) {
        filePath = savePath + fileName;
        
        if (mode == BinaryIOMode::Write) {
            currentMode = mode;
            
            if (writer.is_open()) writer.close();
            
            writer.open(filePath, std::ios::binary);
            
            if (!writer.is_open()) {
                std::cout << "Could not open file for writing at: " << filePath << std::endl;
                currentMode = BinaryIOMode::None;
            }
        }
        else if (mode == BinaryIOMode::Read) {
            currentMode = mode;
            
            if (reader.is_open()) reader.close();
            
            reader.open(filePath, std::ios::binary);
            if (!reader.is_open()) {
                std::cout << "Could not open file for reading at: " << filePath << std::endl;
                currentMode = BinaryIOMode::None;
            }
        }
        else {
            std::cout << "Error: wrong BinaryIO file mode!" << std::endl;
        }
        
        return currentMode == BinaryIOMode::None ? false : true;
    }
    
    void Close() {
        if (currentMode == BinaryIOMode::Write) writer.close();
        else if (currentMode == BinaryIOMode::Read) reader.close();
        currentMode = BinaryIOMode::None;
    }
    
    template<typename T> void Write(T &value) {
        if (!checkWritabilityStatus()) return;
        
        writer.write((const char *)&value, sizeof(value));
    }
    
    void WriteString(std::string str) {
        if (!checkWritabilityStatus()) return;
        
        str += '\0';
        
        char *text = (char*)(str.c_str());
        size_t size = str.size();
        
        writer.write((const char *)text, size);
    }
    
    template<typename T> T Read() {
        if (!checkReadabilityStatus()) return NULL;
        
        T value;
        reader.read((char*)&value, sizeof(value));
        return value;
    }
    
    template<typename T> void Read(T &value) {
        if (!checkReadabilityStatus()) return;
        
        reader.read((char*)&value, sizeof(value));
    }
    
    std::string ReadString() {
        if (!checkReadabilityStatus()) return NULL;
        
        char c;
        std::string result = "";
        while ((c = Read<char>()) != '\0') {
            result += c;
        }
        return result;
    }
    
    void ReadString(std::string &result) {
        if (!checkReadabilityStatus()) return;
        
        char c;
        result = "";
        while ((c = Read<char>()) != '\0') {
            result += c;
        }
    }
    
    private:
    
    std::ofstream writer;
    std::ifstream reader;
    std::string filePath;
    BinaryIOMode currentMode;
    
    const std::string savePath = "../save/";
    
    bool checkReadabilityStatus() {
        if (currentMode != BinaryIOMode::Read) {
            std::cerr << "Trying to read with a non-Readable mode!" << std::endl;
            return false;
        }
        return true;
    }
    
    bool checkWritabilityStatus() {
        if (currentMode != BinaryIOMode::Write) {
            std::cerr << "Trying to write with a non-Writable mode!" << std::endl;
            return false;
        }
        return true;
    }
};

#endif