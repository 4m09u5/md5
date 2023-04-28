#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

uint32_t shift(uint32_t num, int k) {
    return num << k | num >> (32 - k);
}

uint8_t* md5(uint8_t* data, size_t size) {
    uint32_t* hash = (uint32_t*)malloc(sizeof(uint32_t) * 4);

    hash[0] = 0x67452301;
    hash[1] = 0xefcdab89;
    hash[2] = 0x98badcfe;
    hash[3] = 0x10325476;

    uint32_t shifts[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

    uint32_t K[64];

    for (int i = 0; i < 64; i++) {
        K[i] = ((1ull << 32) * fabs(sin(i + 1)));
    }

    size_t addition = ((((size + 8) / 64) + 1) * 64) - 8;
    uint8_t* prepared = (uint8_t*)calloc(addition + 64, 1);
    for (int i = 0; i < size; i++) {
        prepared[i] = data[i];
    }
    prepared[size] = 1 << 7;
    *(uint32_t*)(prepared + addition) = size * 8;
    *(uint32_t*)(prepared + addition - 4) = (size * 8) >> 32;


    for (size_t i = 0; i < addition; i += 64) {
        uint32_t* M = (uint32_t*)(prepared + i);
        uint32_t A = hash[0];
        uint32_t B = hash[1];
        uint32_t C = hash[2];
        uint32_t D = hash[3];

        for (int j = 0; j < 64; j++) {
            uint32_t F, g;

            if (j < 16) {
                F = (B & C) | ((~B) & D);
                g = j;
            }
            else if (j < 32) {
                F = (D & B) | ((~D) & C);
                g = (5 * j + 1) % 16;
            }
            else if (j < 48) {
                F = B ^ C ^ D;
                g = (3 * j + 5) % 16;
            }
            else if (j < 64) {
                F = C ^ (B | (~D));
                g = (7 * j) % 16;
            }

            F += A + K[j] + M[g];
            A = D;
            D = C;
            C = B;
            B += shift(F, shifts[j]);
        }

        hash[0] += A;
        hash[1] += B;
        hash[2] += C;
        hash[3] += D;
    }

    free(prepared);

    return (uint8_t*)hash;
}

void print_hash(uint8_t* hash) {
    for (int i = 0; i < 16; i++) {
        printf("%02x", hash[i]);
    }
}

int main(int argc, char** argv) {
    if (argc == 2) {
        if (strcmp("-h", argv[1]) == 0) {
            printf("md5 [-h help | -f file | -m message] [arg]\n\n-h : print this help message.\n-f : returns checksum of file; path should be passed as an argument.\n-m :  returns checksum of message; string should be passed as an argument.");
        }
        else {
            printf("md5 [-h help | -f file | -m message] [arg]\n\nTry 'md5 -h' for more information.");
        }
    }
    else if (argc == 3) {

        if(strcmp("-m", argv[1]) == 0) {

            uint8_t* hash = md5(argv[2], strlen(argv[2]));

            print_hash(hash);
            free(hash);
        }

        if (strcmp("-f", argv[1]) == 0) {
            FILE* file = fopen(argv[2], "rb");

            if (file == 0) {
                printf("Couldn't open file %s", argv[2]);
                return 0xeba7e1;
            }

            fseek(file, 0, SEEK_END);
            size_t size = ftell(file);
            fseek(file, 0, SEEK_SET);

            uint8_t* inp = (uint8_t*)malloc(sizeof(uint8_t) * size);

            fread(inp, sizeof(uint8_t), size, file);

            fclose(file);

            uint8_t* hash = md5(inp, size);

            print_hash(hash);
            free(hash);
        }
    }
    else {
        printf("md5 [-h help | -f file | -m message] [arg]\nTry 'md5 -h' for more information.");
    }
}
