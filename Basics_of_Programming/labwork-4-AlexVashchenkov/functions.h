#include <iostream>
#include <cstdlib>
#include <ctime>

unsigned char encode_8_4(unsigned char halfByte);

unsigned char decode_8_4(unsigned char fullByte);

void readFileDataAndSaveHamming(std::ifstream &inp, std::ofstream &outp, int64_t nBytes);

void copyData(std::ifstream &inp, std::ofstream &outp, int64_t nBytes);

void createArchive(const char* archiveName, const char* fileName);

void addToArchive(const char* archiveName, const char* fileName);

void deleteFile(const char* archiveName, const char * fileName);

void extractFile(const char * archiveName, const char * fileName);

void mergeTwoArchives(const char* archiveA, const char* archiveB, const char* archiveC);

std::vector<std::string> getFileNames(const char* archiveName);

void variousFunctions(int argc, char* argv[], void (*func)(const char*, const char*));