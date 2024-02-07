// @Author  : enesergul
// @Time    : 13/12/2023
// @OS      : ubuntu 18.04
// @Software: VSCode

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <cstring>

#define ENABLE_DEBUG_MODE 1
#define PLOT_DATA 1

uint16_t calculateCrc(const std::vector<unsigned char> &data, size_t length)
{
    uint16_t crc = 0x0000;
    uint16_t polynomial = 0x1021;

    for (size_t i = 0; i < length; ++i)
    {
        crc ^= (data[i] << 8);
        for (int j = 0; j < 8; ++j)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ polynomial;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void printHex(const std::vector<unsigned char> &data)
{
    for (const auto &byte : data)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << "\n"
              << std::endl;
}

float bytesToFloat(const uint8_t *byteArray)
{
    float result;
    std::memcpy(&result, byteArray, sizeof(float));
    return result;
}

void writeToFile(const std::string &fileName, const std::vector<unsigned char> &data)
{
    std::ofstream file(fileName, std::ios::out | std::ios::app | std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Couldn't open file: " << fileName << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char *>(&data[0]), data.size());

    file.close();
}

void writeFloatToFile(const std::string &fileName, float value) {
    std::ofstream file(fileName, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Couldn't open file: " << fileName << std::endl;
        return;
    }

    file << value << std::endl;
    file.close();
}

struct Packet
{
    unsigned char header;
    unsigned char length;
    unsigned char command;
    std::vector<unsigned char> payload;
    unsigned short crc;
};

int main(int argc, char *argv[])
{
    //* The main function operates using the arguments taken from the command line.
    //* Get fname from command line (cmd line input required)
    const std::string fileName = argv[1];
    std::ifstream file(fileName, std::ios::binary);
    std::string command12Data = "command12.bin";
    std::string command16Data = "command16.bin";
    std::string command12Float = "convertedFloat12.txt";
    std::string command16Float = "convertedFloat16.txt";
    float sizeOfData = 0.0;
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <fileName>" << std::endl;
        return 1;
    }


    if (!file.is_open())
    {
        std::cerr << "Couldn't open file: " << fileName << std::endl;
        return 1;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
    file.close();
    std::vector<Packet> packets;
    int errorCounter = 0;
    size_t i = 0;

    while (i < buffer.size())
    {
        if (buffer[i] != 0xE3)
        {
            ++i;
            continue;
        }
        size_t beginFlag = i;

        Packet packet;
        packet.header = buffer[i++];
        packet.length = buffer[i++];
        if (buffer[i] == 0x12 || buffer[i] == 0x16)
        {
            packet.command = buffer[i++];
        }
        else
        {
            ++errorCounter;
            continue;
        }
        size_t j = 0;
        //* The reason for subtracting 5 from the packet length is that
        //* 'header,' 'length,' 'command,' and 'crc' together occupy 5 bytes.
        while (j < (packet.length - 5))
        {
            packet.payload.push_back(buffer[i++]);
            ++j;
        }
        size_t endFlag = i;
        packet.crc = (buffer[endFlag] << 8) | buffer[endFlag + 1];
        i += 2;


        std::vector<unsigned char> data(buffer.begin() + beginFlag, buffer.begin() + endFlag + 2);
        std::vector<unsigned char> dataForCrc(buffer.begin() + beginFlag, buffer.begin() + endFlag);

        unsigned short calculatedCrc = calculateCrc(dataForCrc, dataForCrc.size());
        if (calculatedCrc != packet.crc)
        {
            ++errorCounter;
        }
        else if (packet.command == 0x12 || packet.command == 0x16)
        {
            sizeOfData += data.size();
        }

#if PLOT_DATA
        if (packet.command == 0x12 && calculatedCrc == packet.crc)
        {
            uint8_t byteArray[] = {packet.payload[5], packet.payload[4], packet.payload[3], packet.payload[2]};
     
                float convertedFloat = bytesToFloat(byteArray);
                writeFloatToFile(command12Float, convertedFloat);   
                writeToFile(command12Data, data);
        }

        if (packet.command == 0x16 && calculatedCrc == packet.crc)
        {        
            uint8_t byteArray[] = {packet.payload[67], packet.payload[66], packet.payload[65], packet.payload[64]};  

                float convertedFloat = bytesToFloat(byteArray);
                writeFloatToFile(command16Float, convertedFloat);
                writeToFile(command16Data, data);
        }
#endif
#if ENABLE_DEBUG_MODE
        std::cout << "--------------------------------------------------------------- \nDATA: " << std::endl;
        printHex(data);
        std::cout << "dataForCrc: " << std::endl;
        printHex(dataForCrc);
        std::cout << "Packets:" << std::endl;
        std::cout << "Header: 0x" << std::hex << static_cast<int>(packet.header) << std::endl;
        std::cout << "Length: " << static_cast<int>(packet.length) << std::endl;
        std::cout << "Command: 0x" << std::hex << static_cast<int>(packet.command) << std::endl;
        std::cout << "Payload: ";
        for (const auto &byte : packet.payload)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }

        std::cout << std::endl;
        std::cout << "CRC: 0x" << std::hex << packet.crc << std::endl;
        std::cout << "calculatedCRC: 0x" << calculatedCrc << std::endl;
#endif

        packets.push_back(packet);
    }

    int type1Count = 0, type2Count = 0;
    for (const auto &packet : packets)
    {
        if (packet.command == 0x12)
        {
            ++type1Count;
        }
        else if (packet.command == 0x16)
        {
            ++type2Count;
        }
    }
    float errorRatio = (static_cast<float>(errorCounter) / packets.size()) * 100;
    float errorByteRatio = (1 - sizeOfData / buffer.size()) * 100;

#if ENABLE_DEBUG_MODE
    std::cout << "\nsizeOfData: " << sizeOfData << std::endl;
    std::cout << "buffer: " << buffer.size() << std::endl;
    std::cout << "\nError packet ratio: %" << errorRatio << std::endl;
#endif
    std::cout << "\nType-1(0x12 Command): " << std::dec << type1Count << " Packets" << std::endl;
    std::cout << "Type-2(0x16 Command): " << std::dec << type2Count << " Packets" << std::endl;
    std::cout << "Error ratio: %" << errorByteRatio << std::endl;
    return 0;
}
