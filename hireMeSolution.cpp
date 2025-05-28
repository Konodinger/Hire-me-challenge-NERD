#include "hireMeUtility.cpp"
#include "hireMeStatistics.cpp"

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


bool BackwardSecondStep(word c, word d, reverseIndexList& reverseConf1, int currentIteration, int currentIndex) {
    if (currentIndex == 0) {
        mulDiffuse(c, d);
        for (int i = 0; i < 32; ++i) {
            if (reverseConf1[d[i]].size() == 0) {
                return false;
            }
        }
    }
    
    if (currentIndex == 32) {
        if (currentIteration == 255) {
            return true;
        }
        word cCopy, dCopy;
        copyWord(c, cCopy);
        copyWord(d, dCopy);
        if (BackwardSecondStep(cCopy, dCopy, reverseConf1, currentIteration + 1, 0)) {
            copyWord(cCopy, c);
            copyWord(dCopy, d);
            return true;
        } else {
            return false;
        }
    }
    
    for (auto value : reverseConf1[d[currentIndex]]) {
        c[currentIndex] = value;
        if (BackwardSecondStep(c, d, reverseConf1, currentIteration, currentIndex + 1)) {
            return true;
        }
    }
    return false;
    
}

bool BackwardFirstStep(word c, word d, reverseIndexList& reverseConf1, reverseIndexList& reverseConf2, int currentIndex) {
    if (currentIndex == 16) {
        word cCopy, dCopy;
        copyWord(c, cCopy);
        copyWord(d, dCopy);
        if (BackwardSecondStep(cCopy, dCopy, reverseConf1, 0, 0)) {
            copyWord(cCopy, c);
            copyWord(dCopy, d);
            return true;
        } else {
            return false;
        }
    }

    bool found = false;
    for (u32 j1 = 0; j1 < 256; ++j1) {
        u8 value = confusion[j1] ^ d[currentIndex];
        for (u8 j2: reverseConf2[value]) {
            c[currentIndex*2] = j1;
            c[currentIndex*2+1] = j2;
            if (BackwardFirstStep(c, d, reverseConf1, reverseConf2, currentIndex + 1)) {
                return true;
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

int main(int argc, char* argv[])
{
    reverseIndexList reverseConf1, reverseConf2;
    preTreatConfusion(reverseConf1, reverseConf2);

    BackwardAnalysis(reverseConf1, reverseConf2, 300);

    
    word outputRef = "Hire me!!!!!!!!";
    word outputRefCopy, inputDecrypted, output;
    copyWord(outputRef, outputRefCopy);
    initWord(inputDecrypted);
    initWord(output);
    

    // auto refClock = clock();
    // if (!Backward(inputDecrypted, outputRefCopy, reverseConf1, reverseConf2)) {
    //     cout << "The backward pass failed..." << endl;
    // }
    // cout << "Time: " << (float)(clock() - refClock) / CLOCKS_PER_SEC << endl;
    
    // crypt(inputDecrypted, output, true);
    // cout << (compareHalf(outputRef, output) ? "Success!" : "Failure") << endl;
    // printHalf("Reference:", outputRef);
    // printWord("Solution:", inputDecrypted);
    // printHalf("Result:   ", output);
    
    // return memcmp(outputRef,output,16);
}
