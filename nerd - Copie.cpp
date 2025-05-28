#include "nerdUtility.cpp"
/*
The solutions to this challenge belong to different levels :

Level 1 : an iterative algorithm which typically takes more than a second to
find a solution (for any given output). 

Most people stop here, which is fine, but if you want to go further, there is :

Level 2 : a non-iterative algorithm which typically takes less than a
millisecond to find a solution (for any given output).

Few people have reached this level. But if you want to beat it completely,
there's yet another castle...

Level 3 : an algorithm which can provide any of the 2^128 solutions (for any
given output).

Even fewer people have reached this final level. Congratulations to them!

*/
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

bool BackwardSecondStep(word c, word d, reverseIndexList& reverseConf1, int currentIteration, int currentIndex) {
    if (currentIndex == 0) {
        mulDiffuse(c, d);
        for (int i = 0; i < 32; ++i) {
            if (reverseConf1[d[i]].size() == 0) {
                //cout << "Dead end at iteration: " << currentIteration << ", index " << i << ", value " << (int) d[i] << endl;
                return false;
            }
        }
    }
    
    if (currentIndex == 32) {
        if (currentIteration == 255) {
            return true;
        }
        return BackwardSecondStep(c, d, reverseConf1, currentIteration + 1, 0);
    }
    
    for (auto value : reverseConf1[d[currentIndex]]) {
        if ((((value >> 1) & 1) != ((value >> 3) & 1))) {

            c[currentIndex] = value;
            word newC, newD;
            copyWord(c, newC);
            copyWord(d, newD);
            if (BackwardSecondStep(newC, newD, reverseConf1, currentIteration, currentIndex + 1)) {
                copyWord(newC, c);
                copyWord(newD, d);
                return true;
            }
        }
    }
    return false;
    
}

bool BackwardFirstStep(word c, word d, reverseIndexList& reverseConf1, reverseIndexList& reverseConf2, int currentIndex) {
    if (currentIndex == 16) {
        return BackwardSecondStep(c, d, reverseConf1, 0, 0);
    }
    u8 safe256AComplete[] = {
        0x07, 0x36, 0x52, 0x63, 0x96, 0xa7, 0xc3, 0xf2,
        0x19, 0x28, 0x4c, 0x7d, 0x88, 0xb9, 0xdd, 0xec
    };

    bool found = false;
    cout << "Value " << (int) d[currentIndex] << endl;
    for (u32 i = 0; i < 16; ++i) {
        // if (((j1 >> 1) & 1) != ((j1 >> 3) & 1)) {
        //     continue;
        // }
        u32 j1 = safe256AComplete[i];
        u8 value = confusion[j1] ^ d[currentIndex];
        for (u32 j = 0; j < 16; ++j) {
            u32 j2 = safe256AComplete[j];
            if ((confusion[j1] ^ confusion[j2 + 256]) == d[currentIndex]) {
                std::cout << (int) confusion[j1] << " / " << (int)confusion[j2] << endl;
                // if (((j2 >> 1) & 1) == ((j2 >> 3) & 1)) {
                    //     continue;
                    // }
                    word cCopy, dCopy;
                    copyWord(c, cCopy);
                    cCopy[currentIndex*2] = j1;
                    cCopy[currentIndex*2+1] = j2;
                    copyWord(d, dCopy);
                    if (BackwardFirstStep(cCopy, dCopy, reverseConf1, reverseConf2, currentIndex + 1)) {
                    copyWord(cCopy, c);
                    copyWord(dCopy, d);
                    return true;
                }
            }
        }
    }
    if (currentIndex == 0) {
        cout << "Error: could not find a solution...\n";
    }
    return false;
}

bool Backward(word c, word d, reverseIndexList& reverseConf1, reverseIndexList& reverseConf2) {
    return BackwardFirstStep(c, d, reverseConf1, reverseConf2, 0);
}

void preTreatConfusion(reverseIndexList& firstHalf, reverseIndexList& secondHalf) {
    for (u32 i = 0; i < 256; ++i) {
        firstHalf[i].clear();
        secondHalf[i].clear();
        for (u32 j = 0; j < 256; ++j) {
            if (confusion[j] == i) {
                firstHalf[i].push_back(j);
            }
            if (confusion[j + 256] == i) {
                secondHalf[i].push_back(j);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    word outputRef = "Hello there!";
    word outputRefCopy, inputDecrypted, output;
    copyWord(outputRef, outputRefCopy);
    initWord(inputDecrypted);
    initWord(output);
    
    reverseIndexList reverseConf1, reverseConf2;
    preTreatConfusion(reverseConf1, reverseConf2);

    // for (int i = 0; i < 256; ++i) {
    //     cout << "Index " << i;
    //     for (auto a : reverseConf2[i]) {
    //         cout << " " << (int) a;
    //     }
    //     cout << endl;
    // }
    auto refClock = clock();
    if (!Backward(inputDecrypted, outputRefCopy, reverseConf1, reverseConf2)) {
        cout << "The backward pass failed..." << endl;
    }
    cout << "Time: " << (float)(clock() - refClock) / CLOCKS_PER_SEC << endl;
    
    crypt(inputDecrypted, output, true);
    cout << (compareHalf(outputRef, output) ? "Success!" : "Failure") << endl;
    printHalf("Reference:", outputRef);
    printWord("Solution:", inputDecrypted);
    printHalf("Result:   ", output);
    
    return memcmp(outputRef,output,16);
}
        