#include <fstream>
#include <vector>


void write_64(uint64_t number, std::ofstream &output){
    char array[8];
    for (int i = 0; i < 8; i++) {
        array[i] = (char)((number >> 8 * (7-i)) & (255));
    }
    output.write(array, 8);
};

void write_byte(char *byte, std::ofstream &output){
    output.write(byte, 8);
}

int main() {
    std::string path = "D:\\ITMO\\Basics_of_Programming\\labwork-4-AlexVashchenkov\\archive1.haf";
    std::ofstream output(path, std::ios::binary);
    auto number = (uint64_t) 18446744073709551612;
    auto byte = (char) 254;
    //const char headerSize[8] = {0, 2, 17, 0, 38, 0, 0, (char)255};
    write_byte(&byte, output);

    return 0;
}