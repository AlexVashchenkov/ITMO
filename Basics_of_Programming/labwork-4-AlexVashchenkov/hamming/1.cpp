#include <iostream>
#include <cstdlib>
#include <ctime>

unsigned char encode_8_4(unsigned char halfByte){
    int a[4] = {0};
    a[3] = halfByte%2;
    a[2] = (halfByte>>1)%2;
    a[1] = (halfByte>>2)%2;
    a[0] = (halfByte>>3);

    for (int i = 0; i < 4; i++){
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;

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
    std::cout << "X: ";
    for (int i = 0; i < 8; i++){
        std::cout << X[i] << " ";
    }
    std::cout << std::endl;
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
    std::cout << "Noisy X: ";
    for (int i = 0; i < 8; i++){
        std::cout << X[i] << " ";
    }
    std::cout << std::endl;


    int s[3] = {0};
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 7; j++){
            s[i] += H[j][i] * X[j];
        }
        s[i] %= 2;
    }

    for (int i = 0; i < 3; i++){
        std::cout << s[i] << " ";
    }

    int s_code = 0;
    s_code += s[2];
    s_code += 2*s[1];
    s_code += 4*s[0];

    int s_to_err_bit_pos[8] = {-1, 6, 5, 3, 4, 0, 2, 1};

    if (nOne % 2 == 0){
        if (s_code == 0){
            std::cout << "No errors" << std::endl;
        } else {
            std::cout << "One or two errors" << std::endl;
            return 255;
        }
    } else {
        if (s_code == 0){
            std::cout << "Parity bit error" << std::endl;
            return 255;
        } else {
            int ebp = s_to_err_bit_pos[s_code];
            std::cout << "Fixing error in bit #" << ebp << std::endl;
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

unsigned char randomError(unsigned char msg){
    unsigned char err_bit = 1;
    int delta = rand()%8;
    std::cout << "Delta: " << delta << std::endl;

    if ((msg >> delta) % 2 == 1) {
        msg &= ~(err_bit << delta);
    } else {
        msg |= (err_bit << delta);
    }

    return msg;
}

int main(){
    srand(time(0));
    //std::cout << encode_8_4(11);
    auto emsg = encode_8_4(13);
    std::cout << "Encoded: " << (int)emsg << std::endl;
    auto x = randomError(emsg);
    std::cout << "After noisy channel: " << (int)x << std::endl;
    auto dmsg = decode_8_4(x);
    std::cout << "Decoded: " << (int)dmsg << std::endl;

    return 0;
}