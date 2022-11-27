#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "functions.h"

unsigned char encode_8_4(unsigned char halfByte){
    int a[4] = {0};
    a[3] = halfByte%2;
    a[2] = (halfByte>>1)%2;
    a[1] = (halfByte>>2)%2;
    a[0] = (halfByte>>3);

    int G[4][7] = {
            {1, 0, 0, 0, 1, 0, 1},
            {0, 1, 0, 0, 1, 1, 1},
            {0, 0, 1, 0, 1, 1, 0},
            {0, 0, 0, 1, 0, 1, 1}
    };

    int X[8] = {0};
    int nOne = 0;
    for (int column = 0; column < 7; column++){
        int s = 0;
        for (int row = 0; row < 4; row++){
            s += G[row][column] * a[row];
        }
        X[column] = s % 2;
        if (X[column] == 1){
            nOne++;
        }
    }
    X[7] = nOne % 2;

    unsigned char pwr2 = 1;
    unsigned char result = 0;

    for (int k = 0; k < 8; k++){
        if (X[7-k] == 1){
            result += pwr2;
        }
        pwr2 *= 2;
    }

    return result;
}

unsigned char decode_8_4(unsigned char fullByte){
    int H[7][3] = {
            {1, 0, 1},
            {1, 1, 1},
            {1, 1, 0},
            {0, 1, 1},
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}
    };

    int X[8] = {0};
    unsigned char tmp = fullByte;
    int nOne = 0;

    for (int i = 0; i < 8; i++){
        X[7 - i] = tmp % 2;
        tmp /= 2;
        if (X[7 - i] == 1){
            nOne++;
        }
    }
    //std::cout << "Noisy X: ";
    //for (int i = 0; i < 8; i++){
    //    std::cout << X[i] << " ";
    //}
    //std::cout << std::endl;


    int s[3] = {0};
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 7; j++){
            s[i] += H[j][i] * X[j];
        }
        s[i] %= 2;
    }

    //for (int i = 0; i < 3; i++){
    //    std::cout << s[i] << " ";
    //}

    int s_code = 0;
    s_code += s[2];
    s_code += 2*s[1];
    s_code += 4*s[0];

    int s_to_err_bit_pos[8] = {-1, 6, 5, 3, 4, 0, 2, 1};

    if (nOne % 2 == 0){
        if (s_code == 0){
            //std::cout << "No errors" << std::endl;
        } else {
            //std::cout << "One or two errors" << std::endl;
            //successFlag = false;
            return 255;
        }
    } else {
        if (s_code == 0){
            //std::cout << "Parity bit error" << std::endl;
            return 255;
        } else {
            int ebp = s_to_err_bit_pos[s_code];
            //std::cout << "Fixing error in bit #" << ebp << std::endl;
            X[ebp] = 1 - X[ebp];
        }
    }
    unsigned char result = 0;
    int k = 3;
    int pwr = 1;
    while (k >= 0){
        if (X[k] == 1){
            result += pwr;
        }
        pwr *= 2;
        k--;
    }

    return result;
}

void readFileDataAndSaveHamming(std::ifstream &inp, std::ofstream &outp, int64_t nBytes){
    unsigned char twoBytes[2];
    for (int64_t k = 0; k < nBytes; k++){
        unsigned char byte = 0;
        inp.read((char *) &byte, 1);
        twoBytes[0] = encode_8_4(byte);
        twoBytes[1] = encode_8_4(byte>>4);
        outp.write((char *) twoBytes, 2);
    }
}

void readHammingAndSaveFileData(std::ifstream &inp, std::ofstream &outp, int64_t nBytes){
    for (int i = 0; i < nBytes; i++){
        unsigned char data1;
        inp.read((char*) &data1, 1);
        unsigned char data2;
        inp.read((char*) &data2, 1);

        unsigned char decoded_data = (decode_8_4(data2)<<4)+(decode_8_4(data1));
        outp.write((const char *) &decoded_data, sizeof(unsigned char));
    }
    inp.close();
    outp.close();
}

void copyData(std::ifstream &inp, std::ofstream &outp, int64_t nBytes){
    for (uint64_t i = 0; i < nBytes; i++){
        char data;
        inp.read(&data, 1);
        outp.write(&data, 1);
    }
}

void createArchive(const char* archiveName, const char* fileName){
    std::ifstream inp(fileName, std::ios::binary | std::ios::in);
    if (!inp.good()){
        std::cout << "File not found: " << fileName << std::endl;
        return ;
    }

    std::ofstream pf(archiveName, std::ios::binary | std::ios::out);

    int slen = strlen(fileName);
    unsigned short headerSize = 1 + slen + 8;

    unsigned char ulen = slen;
    pf.write((const char *) &headerSize, 2);
    pf.write((const char *) &ulen, 1);
    pf.write(fileName, slen);

    inp.seekg(0, std::ifstream::end);
    int64_t nBytes = inp.tellg();
    inp.seekg(0, std::ifstream::beg);

    pf.write((const char *) &nBytes, 8);

    readFileDataAndSaveHamming(inp, pf, nBytes);
    inp.close();
    pf.close();
    std::remove(fileName);
}

void addToArchive(const char* archiveName, const char* fileName){
    std::ifstream inp(fileName, std::ios::binary | std::ios::in);

    if (!inp.good()){
        std::cout << "File not found: " << fileName << std::endl;
        return ;
    }
    std::ifstream pf(archiveName, std::ios::binary | std::ios::in);
    if (!pf.good()){
        inp.close();
        createArchive(archiveName, fileName);
        return;
    }

    std::ofstream newArchive("tmp.haf", std::ios::binary | std::ios::out);

    unsigned short currHeaderSize;
    pf.read((char *) &currHeaderSize, 2);

    char* headerBuff = new char[currHeaderSize];
    pf.read((char *) headerBuff, currHeaderSize);

    int slen = strlen(fileName);
    unsigned short newHeaderSize = currHeaderSize + 9 + slen;

    newArchive.write((const char *) &newHeaderSize, 2);
    newArchive.write(headerBuff, currHeaderSize);
    newArchive.write((const char *) &slen, 1);
    newArchive.write(fileName, slen);

    inp.seekg(0, std::ifstream::end);
    int64_t nBytes = inp.tellg();
    inp.seekg(0, std::ifstream::beg);
    newArchive.write((const char *) &nBytes, 8);

    char * headerPos = headerBuff;

    while(headerPos - headerBuff < currHeaderSize){
        headerPos += *((unsigned char *) headerPos) + 1;
        int64_t currFileLen = *((int64_t*) headerPos);
        headerPos += 8;
        for (int i = 0; i < 2 * currFileLen; i++){
            unsigned char data = 0;
            pf.read((char*) &data, sizeof(char));
            newArchive.write((const char *) &data, sizeof(char));
        }
    }

    delete[] headerBuff;

    readFileDataAndSaveHamming(inp, newArchive, nBytes);
    newArchive.close();
    pf.close();
    inp.close();
    std::remove(fileName);
    std::remove(archiveName);
    std::rename("tmp.haf", archiveName);
}

void deleteFile(const char* archiveName, const char * fileName){
    std::ifstream inp(archiveName, std::ios::binary | std::ios::in);

    if (!inp.good()){
        return ;
    }

    unsigned short headerSize;
    inp.read((char *) &headerSize, 2);

    char* header = new char[headerSize];
    inp.read((char *) header, headerSize);

    for (unsigned short i = 0; i < headerSize; i++){
        std::cout << header[i] << ",";
    }

    bool found = false;
    unsigned short headerOffset = 0;
    int searchLen = strlen(fileName);

    while (headerOffset < headerSize){
        std::cout.flush();
        int nextNameLen = *((unsigned char *)(header + headerOffset));
        if (nextNameLen != searchLen or (strncmp(fileName, header + headerOffset + 1, searchLen) != 0)){
            headerOffset += 1 + nextNameLen + 8;
        } else {
            found = true;
            *((int64_t*) (header + headerOffset + 1 + searchLen)) *= -1;
            break;
        }
    }

    unsigned short newHeaderSize = headerSize;
    if (found){
        newHeaderSize -= 9 + searchLen;
    } else {
        delete[] header;
        return ;
    }

    std::ofstream out("tmp.haf", std::ios::binary | std::ios::out);

    out.write((const char *) &newHeaderSize, 2);

    headerOffset = 0;
    while (headerOffset < headerSize){
        int64_t nBytes = *((int64_t*) (header + headerOffset + 1 + searchLen));
        int nextNameLen = *(unsigned char *)(header + headerOffset);
        if (nBytes < 0){

        } else {
            out.write((const char *) (header + headerOffset), 1 + nextNameLen + 8);
        }
        headerOffset += 1 + nextNameLen + 8;
    }

    headerOffset = 0;
    while (headerOffset < headerSize){
        int64_t nBytes = *((int64_t*) (header + headerOffset + 1 + searchLen));
        int nextNameLen = *(unsigned char *)(header + headerOffset);
        if (nBytes < 0){
            inp.seekg(-2 * nBytes, std::ifstream::cur);
        } else {
            copyData(inp, out, 2 * nBytes);
        }
        headerOffset += 1 + nextNameLen + 8;
    }

    inp.close();
    out.close();
    delete[] header;
    std::remove(archiveName);
    std::rename("tmp.haf", archiveName);
}

void extractFile(const char * archiveName, const char * fileName){
    std::ifstream inp(archiveName, std::ios::binary | std::ios::in);

    if (!inp.good()){
        return ;
    }
    unsigned short headerSize;
    inp.read((char *) &headerSize, 2);
    uint64_t index = 0;
    uint64_t DataPosIndex = headerSize + 2;

    std::ofstream out(fileName, std::ios::binary | std::ios::out);

    while (index < headerSize){
        unsigned char fileNameSize;
        inp.read((char *) &fileNameSize, 1);
        std::string fileNameBuff;
        for (int j = 0; j < fileNameSize; j++){
            char chr;
            inp.read(&chr, 1);
            fileNameBuff += chr;
        }
        uint64_t fileSize;
        inp.read((char *) &fileSize, 8);

        if (fileNameBuff == fileName){
            inp.seekg(DataPosIndex);
            readHammingAndSaveFileData(inp, out, fileSize);
            return ;

        } else {
            DataPosIndex += 2*fileSize;
        }
        index += 1 + fileNameSize + 8;
    }
    inp.close();
    out.close();
}

void mergeTwoArchives(const char* archiveA, const char* archiveB, const char* archiveC){
    std::ifstream inpA(archiveA, std::ios::binary | std::ios::in);

    if (!inpA.good()){
        //std::cout << "Archive not found: " << archiveA << std::endl;
        return ;
    }

    std::ifstream inpB(archiveB, std::ios::binary | std::ios::in);

    if (!inpB.good()){
        //std::cout << "Archive not found: " << archiveB << std::endl;
        return ;
    }

    std::ofstream out(archiveC, std::ios::binary | std::ios::out);

    uint16_t headerSizeA;
    inpA.read((char *) &headerSizeA, 2);

    char* headerA = new char[headerSizeA];
    inpA.read(headerA, headerSizeA);

    inpA.seekg(0, std::ifstream::end);
    uint64_t archiveASize = inpA.tellg();

    uint16_t headerSizeB;
    inpB.read((char *) &headerSizeB, 2);

    char* headerB = new char[headerSizeB];
    inpB.read(headerB, headerSizeB);
    //char * headerA = getHeader(inpA);


    inpB.seekg(0, std::ifstream::end);
    uint64_t archiveBSize = inpB.tellg();

    uint16_t newHeaderSize = headerSizeA + headerSizeB;

    out.write((char *) &newHeaderSize, 2);
    out.write(headerA, headerSizeA);
    out.write(headerB, headerSizeB);

    inpA.seekg(headerSizeA+2, std::ifstream::beg);
    uint64_t index = headerSizeA+2;
    while (index < archiveASize){
        char data;
        inpA.read((char *) &data, 1);
        out.write((char *) &data, 1);
        index++;
    }

    inpB.seekg(headerSizeB+2, std::ifstream::beg);
    index = headerSizeB+2;
    while (index < archiveBSize){
        char data;
        inpB.read((char *) &data, 1);
        out.write((char *) &data, 1);
        index++;
    }

    inpA.close();
    inpB.close();
    out.close();
}

std::vector<std::string> getFileNames(const char* archiveName){
    std::vector<std::string> fileNames;

    std::ifstream inp(archiveName, std::ios::binary | std::ios::in);

    if (!inp.good()){
        std::cout << "Archive not found" << std::endl;
        return {};
    }

    uint16_t headerSize;
    inp.read((char *) &headerSize, 2);

    char* header = new char[headerSize];
    inp.read((char *) header, headerSize);

    int i = 0;
    while (i < headerSize){
        uint8_t fileNameSize = header[i];
        std::string fileName;
        for (int j = 0; j < fileNameSize; j++){
            fileName += header[i+j+1];
        }
        fileNames.push_back(fileName);
        i += 1 + fileNameSize + 8;
    }
    return fileNames;
}

void variousFunctions(int argc, char* argv[], void (*func)(const char*, const char*)){
    int fOffset = -1;
    for (int j = 2; j < argc; j++){
        if (strcmp(argv[j], "-f") == 0){
            if ((j == argc - 1) || (argc < 5)){
                std::cout << "Bad input";
                return ;
            }
            for (int k = 2; k < argc; k++){
                if ((k == j) || (k == j + 1)){
                    continue;
                } else {
                    func(argv[j+1], argv[k]);
                }
            }
            fOffset = j;
        } else if (strncmp(argv[j], "--file=", 7) == 0){
            if (argc < 4){
                std::cout << "Bad input";
                return ;
            }
            if (strlen(argv[j]) < 8){
                std::cout << "Bad input";
                return ;
            } else {

                for (int k = 2; k < argc; k++){
                    if (k == j){
                        continue;
                    } else {
                        func(argv[j]+7, argv[k]);
                    }
                }
            }
            fOffset = j;
        }
    }
    if (fOffset == -1){
        std::cout << "Bad input";
        return ;
    }
}
