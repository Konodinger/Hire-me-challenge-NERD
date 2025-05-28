#ifndef __NERDSTATISTICS__
#define __NERDSTATISTICS__

#include "hireMeUtility.cpp"

struct SecondStepMeasure {
    long part2Calls = 0;
    long part3Calls = 0;
};

bool BackwardSecondStepVar(word c, word d, reverseIndexList& reverseConf1, int currentIteration, int currentIndex, SecondStepMeasure& measures) {
    if (currentIndex == 0) { // This is part 2
        measures.part2Calls++;
        mulDiffuse(c, d);
        for (int i = 0; i < 32; ++i) {
            if (reverseConf1[d[i]].size() == 0) {
                return false;
            }
        }
        measures.part3Calls++;
    }
    
    if (currentIndex == 32) {
        if (currentIteration == 255) {
            return true;
        }
        word cCopy, dCopy;
        copyWord(c, cCopy);
        copyWord(d, dCopy);
        if (BackwardSecondStepVar(cCopy, dCopy, reverseConf1, currentIteration + 1, 0, measures)) {
            copyWord(cCopy, c);
            copyWord(dCopy, d);
            return true;
        } else {
            return false;
        }
    }
    
    // This is part 3 (which is also recursive for convenience, but we consider
    // those calls to be the same "part 3 call" for our measurement, since branching is
    // already measured with part2)
    for (auto value : reverseConf1[d[currentIndex]]) {
        c[currentIndex] = value;
        if (BackwardSecondStepVar(c, d, reverseConf1, currentIteration, currentIndex + 1, measures)) {
            return true;
        }
    }
    return false;
    
}

struct FirstStepMeasure {
    long nbSecondStepCalls = 0;
    long cumulatedPart2Calls = 0;
    long cumulatedPart3Calls = 0;
    long part2CallsForSolution = 0;
    long part3CallsForSolution = 0;
    float timeForSolution = 0.f;
};

bool BackwardFirstStepVar(word c, word d, reverseIndexList& reverseConf1, reverseIndexList& reverseConf2, int currentIndex, FirstStepMeasure& measure) {
    if (currentIndex == 16) {
        measure.nbSecondStepCalls++;
        SecondStepMeasure ssmeasure {};
        word cCopy, dCopy;
        copyWord(c, cCopy);
        copyWord(d, dCopy);
        auto refClock = clock();
        bool answer = BackwardSecondStepVar(cCopy, dCopy, reverseConf1, 0, 0, ssmeasure);
        //cout << "Cumulated length: " << cumulatedLength << endl;
        measure.cumulatedPart2Calls += ssmeasure.part2Calls;
        measure.cumulatedPart3Calls += ssmeasure.part3Calls;

        if (answer) {
            copyWord(cCopy, c);
            copyWord(dCopy, d);
            measure.part2CallsForSolution = ssmeasure.part2Calls;
            measure.part3CallsForSolution = ssmeasure.part3Calls;
            measure.timeForSolution = (float)(clock() - refClock) / CLOCKS_PER_SEC;
        }
        return answer;
    }

    bool found = false;
    for (u32 j1 = 0; j1 < 256; ++j1) {
        u8 value = confusion[j1] ^ d[currentIndex];
        for (u8 j2: reverseConf2[value]) {
            c[currentIndex*2] = j1;
            c[currentIndex*2+1] = j2;
            if (BackwardFirstStepVar(c, d, reverseConf1, reverseConf2, currentIndex + 1, measure)) {
                return true;
            }
        }
    }
    if (currentIndex == 0) {
        cout << "Error: could not find a solution...\n";
    }
    return false;
}

void BackwardAnalysis(reverseIndexList& reverseConf1, reverseIndexList& reverseConf2, int nbTests) {
    // Variant computing some stats about the algorithm length.
    srand (time(NULL)); 
    long cumulatedSecondStepCalls = 0;
    double cumulatedPart2Calls = 0;
    double cumulatedPart3Calls = 0;
    double cumulatedPart2CallsForSolution = 0;
    double cumulatedPart3CallsForSolution = 0;
    double cumulatedTime = 0;
    double cumulatedTimeForSolution = 0;
    for (int test = 0; test < nbTests; ++test) {
        word c, d;
        initWord(c);
        initWord(d);
        for(int i = 0; i < 16; ++i) {
            d[i] = rand();
        }

        FirstStepMeasure fsmeasure;
        
        auto refClock = clock();
        bool result = BackwardFirstStepVar(c, d, reverseConf1, reverseConf2, 0, fsmeasure);
        float time = (float)(clock() - refClock) / CLOCKS_PER_SEC;
        double meanPart2 = ((double) fsmeasure.cumulatedPart2Calls) / fsmeasure.nbSecondStepCalls;
        double meanPart3 = ((double) fsmeasure.cumulatedPart3Calls) / fsmeasure.nbSecondStepCalls;
        cout << "------------------Test number " << test + 1 << "------------------" << endl
        << setfill(' ')
        << std::setw(41) << "Time for the whole algorithm: " << time << "s" << endl
        << std::setw(41) << "Time for the correct second step: " << fsmeasure.timeForSolution << "s" << endl
        << std::setw(41) << "Second step calls: " << fsmeasure.nbSecondStepCalls << endl
        << std::setw(41) << "Total part2 calls: " << fsmeasure.cumulatedPart2Calls << endl
        << std::setw(41) << "Mean part2 calls: " << meanPart2 << endl
        << std::setw(41) << "Part2 calls for the correct second step: " << fsmeasure.part2CallsForSolution << endl
        << std::setw(41) << "Total part3 calls: " << fsmeasure.cumulatedPart3Calls << endl
        << std::setw(41) << "Mean part3 calls: " << meanPart3 << endl
        << std::setw(41) << "Part3 calls for the correct second step: " << fsmeasure.part3CallsForSolution << endl
        << endl;
        cumulatedSecondStepCalls += fsmeasure.nbSecondStepCalls;
        cumulatedPart2Calls += meanPart2;
        cumulatedPart3Calls += meanPart3;
        cumulatedPart2CallsForSolution += fsmeasure.part2CallsForSolution;
        cumulatedPart3CallsForSolution += fsmeasure.part3CallsForSolution;
        cumulatedTime += time;
        cumulatedTimeForSolution += fsmeasure.timeForSolution;
    }

    cout << "-----------------------------------------------------------------" << endl
    << "-----------------------------Results-----------------------------" << endl 
    << "-----------------------------------------------------------------" << endl
    << endl << setfill(' ')
    << "Tests performed: " << nbTests << endl
    << "On average," << endl
    << std::setw(55) << "Time to find the solution: " << cumulatedTime / nbTests << "s" << endl
    << std::setw(55) << "Time to find the solution for the correct second step: " << cumulatedTimeForSolution / nbTests << "s" << endl
    << std::setw(55) << "Second step calls: " << ((double) cumulatedSecondStepCalls) / nbTests << endl
    << std::setw(55) << "Part2 calls during the second step: " << cumulatedPart2Calls / nbTests << endl
    << std::setw(55) << "Part2 calls for the correct second step: " << cumulatedPart2CallsForSolution / nbTests << endl
    << std::setw(55) << "Part3 calls during the second step: " << cumulatedPart3Calls / nbTests << endl
    << std::setw(55) << "Part3 calls for the correct second step: " << cumulatedPart3CallsForSolution / nbTests << endl;
       
}

#endif