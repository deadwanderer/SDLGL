#include "BinaryIO.h"

BinaryIO::BinaryIO() {
    currentMode = BinaryIOMode::None;
}

BinaryIO::~BinaryIO() {
    if (writer.is_open()) writer.close();
    if (reader.is_open()) reader.close();
}

bool BinaryIO::Open(std::string fileName, BinaryIOMode mode) {
    filePath = savePath + fileName;
    
    if (mode == BinaryIOMode::Write) {
        currentMode = mode;
        
        if (writer.is_open()) writer.close();
        
        writer.open(filePath, std::ios::binary);
        
        if (!writer.is_open()) {
            std::cerr << "Could not open file for writing at: " << filePath << std::endl;
            currentMode = BinaryIOMode::None;
        }
    }
    else if (mode == BinaryIOMode::Read) {
        currentMode = mode;
        
        if (reader.is_open()) reader.close();
        
        reader.open(filePath, std::ios::binary);
        if (!reader.is_open()) {
            std::cerr << "Could not open file for reading at: " << filePath << std::endl;
            currentMode = BinaryIOMode::None;
        }
    }
    else {
        std::cout << "Error: wrong BinaryIO file mode!" << std::endl;
    }
    
    return currentMode == BinaryIOMode::None ? false : true;
}

void BinaryIO::Close() {
    if (currentMode == BinaryIOMode::Write) writer.close();
    else if (currentMode == BinaryIOMode::Read) reader.close();
    currentMode = BinaryIOMode::None;
}

template<typename T>
void BinaryIO::Write(T &value) {
    if (!checkWritabilityStatus()) return;
    
    writer.write((const char *)&value, sizeof(value));
}

void BinaryIO::WriteString(std::string str) {
    if (!checkWritabilityStatus()) return;
    
    str += '\0';
    
    char *text = (char*)(str.c_str());
    unsigned long size = str.size();
    
    writer.write((const char *)text, size);
}

template<typename T>
T BinaryIO::Read() {
    if (!checkReadabilityStatus()) return;
    
    T value;
    reader.read((char*)&value, sizeof(value));
    return value;
}

template<typename T>
void BinaryIO::Read(T &value) {
    if (!checkReadabilityStatus()) return;
    
    reader.read((char*)&value, sizeof(value));
}

std::string BinaryIO::ReadString() {
    if (!checkReadabilityStatus()) return;
    
    char c;
    string result = "";
    while ((c = read<char>()) != '\0') {
        result += c;
    }
    return result;
}

void BinaryIO::ReadString(std::string &result) {
    if (!checkReadabilityStatus()) return;
    
    char c;
    result = "";
    while ((c = read<char>()) != '\0') {
        result += c;
    }
}

bool BinaryIO::checkReadabilityStatus() {
    if (currentMode != BinaryIOMode::Read) {
        std::cerr << "Trying to read with a non-Readable mode!" << std::endl;
        return false;
    }
    return true;
}

bool BinaryIO::checkWritabilityStatus() {
    if (currentMode != BinaryIOMode::Write) {
        std::cerr << "Trying to write with a non-Writable mode!" << std::endl;
        return false;
    }
    return true;
}