#include <iostream>
#include <vector>
using std::vector;

const int dim1 = 3;
const int dim2 = 3;

int field[dim1][dim2] =
        {
                {0, 0, 0},
                {0, 32767, 0},
                {0, 0, 0}
        };

void PrintField(){
    for (auto & i : field) {
        for (int j: i) {
            std::cout << j << " ";
        }
        std::cout << "\n";
    }
}

void fall(vector <std::pair<int, int>> &unstable_list){
    for (auto i : unstable_list) {
        int x = i.first;
        int y = i.second;
        field[x][y] -= 4;
        if (x == 0) {
            if (y == 0) {
                field[x + 1][y]++;
                field[x][y + 1]++;
            } else if (y == dim2 - 1) {
                field[x + 1][y]++;
                field[x][y - 1]++;
            } else {
                field[x + 1][y]++;
                field[x][y - 1]++;
                field[x][y + 1]++;
            }
        } else if (x == dim1 - 1) {
            if (y == 0) {
                field[x - 1][y]++;
                field[x][y + 1]++;
            } else if (y == dim2 - 1) {
                field[x - 1][y]++;
                field[x][y - 1]++;
            } else {
                field[x - 1][y]++;
                field[x][y - 1]++;
                field[x][y + 1]++;
            }
        } else {
            if (y == 0) {
                field[x - 1][y]++;
                field[x + 1][y]++;
                field[x][y + 1]++;
            } else if (y == dim2 - 1) {
                field[x - 1][y]++;
                field[x + 1][y]++;
                field[x][y - 1]++;
            } else {
                field[x - 1][y]++;
                field[x + 1][y]++;
                field[x][y - 1]++;
                field[x][y + 1]++;
            }
        }
    }
}

vector <std::pair<int, int>> find_unstable() {
    vector <std::pair<int, int>> vect;
    for (int i = 0; i < dim1; i++) {
        for (int j = 0; j < dim2; j++) {
            if (field[i][j] >= 4){
                vect.emplace_back(i, j);
            }
        }
    }
    return vect;
}

int main() {
    vector <std::pair<int, int>> unstable_list = find_unstable();
    while (!unstable_list.empty()) {
        fall(unstable_list);
        unstable_list = find_unstable();
    }
    PrintField();
    /*
    for (auto i : unstable_list){
        std::cout << "(" << i.first << "," << i.second << ") ";
    }
    std::cout << "\n\n";
    */
}