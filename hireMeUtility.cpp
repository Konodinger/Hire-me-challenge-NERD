#ifndef __NERDUTLITY__
#define __NERDUTLITY__

#define PRINT0X
//#define PRINTHEXASINT

#ifdef PRINTHEXASINT
#define HEX( x ) (int) x
#else
#ifdef PRINTOX
#define HEX( x ) "0x" << setw(2) << setfill('0') << hex << (int) x
#else
#define HEX( x ) setw(2) << setfill('0') << hex << (int) x
#endif
#endif

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

#include "hireMeRef.cpp"

typedef u8 word[32];
typedef u8 halfWord[16];

using namespace std;

typedef vector<u8> reverseIndexList[256];

void initWord(word w) {
    for (int i = 0; i < 32; ++i) {
        w[i] = 0;
    }
}

void initHalfWord(halfWord h) {
    for (int i = 0; i < 16; ++i) {
        h[i] = 0;
    }
}


void copyWord(word reference, word dest) {
    memcpy(dest, reference, 32);
}

void copyHalf(halfWord reference, halfWord dest) {
    memcpy(dest, reference, 16);
}

bool compareWord (word a, word b) {
    for (int i = 0; i < 32; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

bool compareHalf (halfWord a, halfWord b) {
    for (int i = 0; i < 16; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void printWord(string preText, word charList, bool asHex = true) {
    cout << preText;
    if (asHex) {
        for (u8 i = 0; i < 32; ++i) {
            if (i % 8 == 0) {
                cout << endl;
            }
            cout << HEX(charList[i]) << " ";
        }
    } else {
        cout << charList;
    }
    cout << endl;
}

void printHalf(string preText, halfWord charList, bool asHex = false) {
    cout << preText;
    if (asHex) {
        for (u8 i = 0; i < 16; ++i) {
            if (i % 8 == 0) {
                cout << endl;
            }
            cout << HEX(charList[i]) << " ";
        }
    } else {
        cout << charList;
    }
    cout << endl;
}

void printWMatrix(word matrix) {
    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 8; ++j) {
            cout << ((matrix[i] >> j) & 1) << " ";
        }
        cout << endl;
    }
}

void printHWMatrix(halfWord matrix) {
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 8; ++j) {
            cout << ((matrix[i] >> j) & 1) << " ";
        }
        cout << endl;
    }
}

void printMatrix(u32 matrix[32]) {
    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            cout << ((matrix[i] >> j) & 1) << " ";
        }
        cout << endl;
    }
}

void printMatrix(u32 matrix[32], int row1, int col1, int nbR, int nbC) {
    for (int i = 0; i < nbR; ++i) {
        for (int j = 0; j < nbC; ++j) {
            cout << ((matrix[i + row1] >> (j + col1)) & 1) << " ";
        }
        cout << endl;
    }
}

void crypt(word input, halfWord output, bool fullResult = false) {
    word inputCopy;
    copyWord(input, inputCopy);
    word outputComplete;
    Forward(inputCopy, outputComplete, confusion, diffusion);

    if (fullResult) {
        memcpy(output, outputComplete, 32);
    } else {
        memcpy(output, outputComplete, 16);
    }
}

void cryptThenPrint(word input, halfWord output, bool asHex = true, string preText = "") {
    cout << preText;
    crypt(input, output);
    if (asHex) {
        for (u8 line = 0; line < 4; ++line) {
            cout << endl;
            for (u8 i = 0; i < 8; ++i) {
                cout << HEX(input[i + line * 8]) << " ";
            }
            cout << "   ||    ";
            if (line < 2) {
                for (u8 i = 0; i < 8; ++i) {
                    cout << HEX(output[line * 8 + i]) << " ";
                }
            }
        }
        cout << endl;
    } else {
        cout << input << endl;
        cout << output << endl;
    }
}

void mulDiffuse(word input, word output) {
    for(u8 j=0;j<32;j++) {
        output[j] = 0;
        for(u8 k=0;k<32;k++)
            output[j]^=input[k]*((diffusion[j]>>k)&1);
    }
}

void ForwardFirstPart(word c, word d, u8 s[512], u32 p[32])
{
    for(u32 i=0;i<256;i++)
    {
        for(u8 j=0;j<32;j++)
        {
            d[j]=s[c[j]];
            c[j]=0;
        }
        mulDiffuse(d, c);
    }
}

void cryptFirstPart(word input, halfWord output) {
    word inputCopy;
    copyWord(input, inputCopy);
    word outputComplete;
    ForwardFirstPart(inputCopy, outputComplete, confusion, diffusion);
    memcpy(output, outputComplete, 32);}
#endif