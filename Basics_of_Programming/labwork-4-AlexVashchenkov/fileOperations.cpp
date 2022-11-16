#include <iostream>
#include <fstream>
#include <vector>


void createArchive(char archiveHeaderSize){
    FILE *f;
    f = fopen("archive1.haf", "wb");
    fwrite(&archiveHeaderSize, 64, 1, f);
}

int main(){
    FILE *f = fopen("archive1.haf", "wb");
    fwrite((char) 125, 64, 1, f);

    std::string path = "D:\\ITMO\\Basics_of_Programming\\labwork-4-AlexVashchenkov";
    char archiveHeaderSize = (unsigned char)127;

    createArchive(archiveHeaderSize);
}
