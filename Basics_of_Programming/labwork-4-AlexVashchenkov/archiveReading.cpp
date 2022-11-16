#include <iostream>
#include <fstream>
#include <vector>

std::vector<unsigned char> readHeaderSize(std::ifstream input){
    std::vector<unsigned char> data;
    while(input){
        unsigned char buff;
        input >> buff;
        data.push_back(buff);
    }

    for (unsigned char i : data){
        std::cout << (int)i << " ";
    }
}

int main(){
    std::vector<unsigned char> data;
    char* path = "D:\\ITMO\\Basics_of_Programming\\labwork-4-AlexVashchenkov\\archive1.haf)";
    std::ifstream input(path, std::ios::binary);

    while(input){
        unsigned char buff;
        input >> buff;
        data.push_back(buff);
    }

    for (unsigned char i : data){
        std::cout << (int)i << " ";
    }
}