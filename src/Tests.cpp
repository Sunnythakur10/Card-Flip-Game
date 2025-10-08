// Simple runtime tests for deck generation invariants.
#include "Deck.h"
#include "GameManager.h"
#include <iostream>
#include <set>
#include <map>

static bool testDeckFrequencies(int pairs) {
    Deck d; d.createPairs(pairs);
    std::map<std::pair<int,int>, int> freq;
    for (auto &cPtr : d.getCards()) {
        freq[{cPtr->getRank(), cPtr->getSuit()}]++;
    }
    if (freq.size() != (size_t)pairs) {
        std::cerr << "FAIL: expected unique patterns=" << pairs << " got=" << freq.size() << "\n"; return false; }
    for (auto &kv : freq) {
        if (kv.second != 2) { std::cerr << "FAIL pattern freq !=2\n"; return false; }
    }
    return true;
}

int runAllTests() {
    int failures = 0;
    if (!testDeckFrequencies(8)) { std::cerr << "testDeckFrequencies(8) failed\n"; failures++; }
    if (!testDeckFrequencies(4)) { std::cerr << "testDeckFrequencies(4) failed\n"; failures++; }
    if (failures==0) std::cout << "All deck tests passed.\n";
    return failures;
}

#ifdef RUN_TESTS_MAIN
int main(){ return runAllTests(); }
#endif