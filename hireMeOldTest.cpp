#ifndef __NERDOLDTEST__
#define __NERDOLDTEST__

#include "hireMeUtility.cpp"


void printExemples1() {
    word input;
    halfWord output;
    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            input[j] = (j == i);
        }
        cryptThenPrint(input, output, true, std::string("Test with only a 1 at the ") + std::to_string(i) + std::string("th position"));
    }
}

void printExemples2(int refIndex, int nb) {
    word input;
    halfWord output;

    for (int i = 0; i < 32; ++i) {
            input[i] = 0;
        }
    for (int i = 0; i < nb; ++i) {
        input[refIndex] = i;
        cryptThenPrint(input, output, true);
    }
}

void printExemplesOnSpecificIndex(int refIndex) {
    word input;
    halfWord output;
    for (int i = 0; i < 32; ++i) {
            input[i] = 0;
        }
    for (int i = 0; i < 256; ++i) {
        input[refIndex] = i;
        crypt(input, output);
        cout << HEX(input[refIndex]) << " || " << HEX(output[refIndex / 2]) << endl;
    }
}


bool printExemplesOnSpecificIndex2(int refIndex) {
    bool everythingIsConstant = true;
    word input;
    halfWord output;
    for (int i = 0; i < 32; ++i) {
            input[i] = 0;
        }
    for (int i = 0; i < 256; ++i) {
        input[refIndex] = i;
        crypt(input, output);
        cout << HEX(input[refIndex]) << " || " << HEX(output[refIndex / 2]) << endl;
        bool isConstant = true;
        for (int j = 0; j < 16; ++j) {
            if((j != refIndex / 2) && output[j] != (u8) 95) {
                cout << "Inconsistency at num " << j << " (iteration " << i << ")\n";
                isConstant = false;
            }
        }
        if (isConstant) {
            cout << "The rest is constant\n";
        } else {
            everythingIsConstant = false;
        }
    }
    return everythingIsConstant;
}



void analyzeBasicExemples(int refIndex) {
    word input;
    halfWord output;
    int outputIndex[256];
    for (int i = 0; i < 32; ++i) {
        input[i] = 0;
    }
    for (int i = 0; i < 256; ++i) {
        outputIndex[i] = 0;
    }
    for (int i = 0; i < 256; ++i) {
        input[refIndex] = i;
        crypt(input, output);
        outputIndex[output[0]]++;
    }
    int equal0 = 0;
    int equal1 = 0;
    int equal2 = 0;
    int equal3 = 0;
    for (int i = 0; i < 256; ++i) { 
        if (outputIndex[i] == 0) {
            equal0++;
        }
        if (outputIndex[i] == 1) {
            equal1++;
        }
        if (outputIndex[i] == 2) {
            equal2++;
        }
        if (outputIndex[i] == 3) {
            equal3++;
        }
        cout << i << " || " << outputIndex[i] << endl;
    }
    cout << "Number of index with 0 link: " << equal0 << endl;
    cout << "Number of index with 1 link: " << equal1 << endl;
    cout << "Number of index with 2 links: " << equal2 << endl;
    cout << "Number of index with 3 links: " << equal3 << endl;
}

void analyzeBasicExemples(int refIndex) {
    word input;
    halfWord output;
    int outputIndex[256];
    for (int i = 0; i < 32; ++i) {
        input[i] = 0;
    }
    for (int i = 0; i < 256; ++i) {
        outputIndex[i] = 0;
    }
    for (int i = 0; i < 256; ++i) {
        input[refIndex * 2] = i;
        input[refIndex * 2 + 1] = 0;
        crypt(input, output);
        outputIndex[output[refIndex]]++;

        input[refIndex * 2] = 0;
        input[refIndex * 2 + 1] = i;
        crypt(input, output);
        outputIndex[output[refIndex]]++;
    }
    int equal[5] = {0, 0, 0, 0, 0};
    for (int i = 0; i < 256; ++i) { 
        if (outputIndex[i] == 0) {
            cout << i << "! ";
        }
        if (outputIndex[i] == 4) {
            cout << i << "? ";
        }
        if (outputIndex[i] >= 5) {
            cout << "WARNING" << endl;
        } else {
            equal[outputIndex[i]]++;
        }
    }
    cout << endl;
    cout << "Occurencies:" << equal[0] << " " << equal[1] << " " << equal[2] << " " << equal[3] << " " << equal[4] << " " << endl;
}

// Deprecated
// bool TryBackward(word c, word d, u8 s[512], u32 p[32], reverseIndexList& reverseConf1, reverseIndexList& reverseConf2)
// {
//     word dInitial, dCopy, cCopy, dFinal;
//     copyWord(d, dInitial);
//     copyWord(d, dCopy);
//     initWord(cCopy);
//     initWord(dFinal);

//     printWord("Initial word to decrypt (d): ", dInitial, false);

//     //DECRYPT

//     for(u32 i = 0; i < 16; ++i) {
//         bool found = false;
//         for (u32 j1 = 0; j1 < 256; ++j1) {
//             u8 value = s[j1] ^ dCopy[i];
//             if (reverseConf2[value].size() != 0) {
//                 cCopy[i*2] = j1;
//                 cCopy[i*2+1] = reverseConf2[value][0]; // We can create various solution by using other value when possible.
//                 found = true;
//                 break;
//             }
//         }
//         if (!found) {
//             cout << "Error: could not find a matching index for index " << i << " during first step." << endl;
//             return false;
//         }
//     }

//     printWord("Decrypted word (c) after first step: ", cCopy);
//     if (!BackwardSecondStep(cCopy, dCopy, reverseConf1, 0, 0)) {
//         cout << "ERROR: the backward process failed" << endl;
//     }
    
//     printWord("Decrypted word (c): ", cCopy);
    
//     //CRYPT
    
//     for(u32 i=0;i<256;i++) {
//         for(u8 j=0;j<32;j++) {
//             dCopy[j]=s[cCopy[j]];
//             cCopy[j] = 0;
//         }

//         for(u8 j=0;j<32;j++) {
//             for(u8 k=0;k<32;k++)
//                 cCopy[j]^=dCopy[k]*((p[j]>>k)&1);
//         }
//     }

//     printWord("Transformed uncrypted word (c): ", cCopy);

//     for(u8 i=0;i<16;i++)
//         dFinal[i]=s[cCopy[i*2]]^s[cCopy[i*2+1]+256];
    
//     printWord("dInitial: ", dInitial);
//     printWord("dFinal: ", dFinal);
//     return compareHalf(dInitial,dFinal);

// }

void printDiffusionTest() {

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 8; ++j) {
            halfWord X;
            initHalfWord(X);
            X[i] = (1 << j);

            printHWMatrix(X);
            cout << endl;
            word Y;
            initWord(Y);
            
            for(u8 k=0;k<32;k++)
                for(u8 l=0;l<16;l++)
                Y[k]^=X[l]*((diffusion[k]>>l)&1);
            // The ith row of the diffusion matrix is on the jth row of Y.
            printWMatrix(Y);
            cout << endl;
        }
    }
    printMatrix(diffusion);
}

void testInverseMatrix() {
    word input, code, decode;
    initWord(code);
    initWord(decode);

    for(u8 i=0;i<32;i++) {
        for(u8 j=0;j<32;j++) {

            code[i]^=input[j]*((diffusion[i]>>j)&1);
        }
    }
    for(u8 i=0;i<32;i++) {
        for(u8 j=0;j<32;j++) {
            decode[i]^=code[j]*((diffusion[i]>>j)&1);
        }
    }

    printWord("Input: ", input);
    printWord("Decode: ", decode);
}

#endif