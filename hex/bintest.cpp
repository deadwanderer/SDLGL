#include "BinaryIO.h"
#include <string>

int main() {
    std::string name = "test.bin";
    
    BinaryIO writer;
    
    writer.Open(name, BinaryIOMode::Write);
    
    unsigned int testUInt = 31;
    int testInt = -3;
    float testFloat = 4.5f;
    bool testBool = false;
    std::string testString = "Testing, testing...";
    writer.Write(testUInt);
    writer.Write(testInt);
    writer.Write(testFloat);
    writer.Write(testBool);
    writer.WriteString(testString);
    
    writer.Close();
    
    BinaryIO reader;
    
    reader.Open(name, BinaryIOMode::Read);
    
    unsigned int resultUInt;
    reader.Read(resultUInt);
    int resultInt = reader.Read<int>();
    float resultFloat;
    reader.Read(resultFloat);
    bool resultBool = reader.Read<bool>();
    std::string resultString = reader.ReadString();
    
    std::cout << resultString << ", " << resultBool << ", " << resultFloat << ", " << resultInt << ", " << resultUInt << std::endl;
    
    reader.Close();
    
    return 0;
}