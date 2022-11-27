#include <iostream>
#include <vector>
#include <cstring>
#include "functions.h"

int main(int argc, char* argv[]) {
    const char* shortNames[] = {"-c", "-l", "-x", "-a", "-d", "-A"};
    const char* longNames[] = {"--create", "--list", "--extract", "--append", "--delete", "--concatenate"};

    int cmdOffset = -1;
    for (int i = 0; i < sizeof(shortNames) / sizeof(shortNames[0]); i++){
        if (strcmp(argv[1], shortNames[i]) == 0){
            cmdOffset = i;
            break;
        }
        if (strcmp(argv[1], longNames[i]) == 0){
            cmdOffset = i;
            break;
        }
    }
    if (cmdOffset == -1){
        std::cout << "Bad input (no operation is requested)";
        return 0;
    }

    if (cmdOffset == 0){
        int fOffset = -1;
        for (int j = 2; j < argc; j++){
            if (strcmp(argv[j], "-f") == 0){
                if ((j == argc - 1) || (argc < 5)){
                    std::cout << "Bad input (too few arguments)";
                    return 0;
                }
                bool created = false;

                for (int k = 2; k < argc; k++){
                    if ((k == j) || (k == j + 1)){
                        continue;
                    } else {
                        if (!created){
                            createArchive(argv[j+1], argv[k]);
                            created = true;
                        } else {
                            addToArchive(argv[j+1], argv[k]);
                        }
                    }
                }
                fOffset = j;
            } else if (strncmp(argv[j], "--file=", 7) == 0){
                if (argc < 4){
                    std::cout << "Bad input (too few arguments)";
                    return 0;
                }
                if (strlen(argv[j]) < 8){
                    std::cout << "Bad input (invalid name for an archive)";
                    return 0;
                } else {
                    bool created = false;

                    for (int k = 2; k < argc; k++){
                        if (k == j){
                            continue;
                        } else {
                            if (!created){
                                createArchive(argv[j]+7, argv[k]);
                                created = true;
                            } else {
                                addToArchive(argv[j]+7, argv[k]);
                            }
                        }
                    }
                }
                fOffset = j;
            }
        }
        if (fOffset == -1){
            std::cout << "Bad input (archive wasn't created due to absence of the files)";
            return 0;
        }
    } else if (cmdOffset == 1){
        if (argc == 3){
            if (strncmp(argv[2], "--file=", 7) != 0){
                std::cout << "Bad input (operation typed wrong)";
                return 0;
            } else {
                auto fNames = getFileNames(argv[2]+7);
                for (auto & fName : fNames){
                    std::cout << fName << "\n";
                }
            }
        } else if (argc == 4){
            if (strncmp(argv[2], "-f", 2) != 0){
                std::cout << "Bad input (operation typed wrong)";
                return 0;
            } else {
                auto fNames = getFileNames(argv[3]);
                for (auto & fName : fNames){
                    std::cout << fName << "\n";
                }
            }
        } else {
            std::cout << "Bad input (given not exactly 4 arguments)";
            return 0;
        }
    } else if (cmdOffset == 2){
        variousFunctions(argc, argv, extractFile);
    } else if (cmdOffset == 3){
        variousFunctions(argc, argv, addToArchive);
    } else if (cmdOffset == 4){
        variousFunctions(argc, argv, deleteFile);
    } else if (cmdOffset == 5){
        for (int j = 2; j < argc; j++){
            if (strcmp(argv[j], "-f") == 0){
                if ((j == argc - 1) || (argc != 6)){
                    std::cout << "Bad input (given not exactly 6 arguments)";
                    return 0;
                }
                std::vector<int> archiveToMerge = {2, 3, 4, 5};
                archiveToMerge.erase(archiveToMerge.begin()+j-2, archiveToMerge.begin()+j);
                mergeTwoArchives(argv[archiveToMerge[0]], argv[archiveToMerge[1]], argv[j+1]);
            } else if (strncmp(argv[j], "--file=", 7) == 0){
                if (argc != 5){
                    std::cout << "Bad input (given not exactly 6 arguments)";
                    return 0;
                }
                std::vector<int> archiveToMerge = {2, 3, 4};
                archiveToMerge.erase(archiveToMerge.begin()+j-2, archiveToMerge.begin()+j-1);
                mergeTwoArchives(argv[archiveToMerge[0]], argv[archiveToMerge[1]], argv[j]+7);
            }
        }
    }
    return 0;
}