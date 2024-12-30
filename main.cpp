#include <iostream>
#include <string>
#include <cmath>

#include "cache_simulator.h"

int main(int argc, char *argv[])
{
    // PARAMETER HANDLING

    // check that all inputs were included
    if (argc != 7)
    {
        std::cerr << "Invalid input. Exiting.\n";
        return 1;
    }

    int numSets = std::atoi(argv[1]);
    int numBlocks = std::atoi(argv[2]);
    int numBytes = std::atoi(argv[3]); // block size
    std::string handleMiss = argv[4];
    std::string handleWrite = argv[5];
    std::string handleEviction = argv[6];

    if (validateArguments(numSets, numBlocks, numBytes, handleMiss, handleWrite, handleEviction) == 1)
    {
        return 1;
    }

    // SET UP CACHE
    Cache cache;
    cacheSetUp(cache, numSets, numBlocks, numBytes, handleMiss, handleWrite, handleEviction);

    // RUN SIMULATOR
    // Note: assumes all input data from file is valid

    char loadStore;   // 1
    uint32_t address; // 2
    std::string addressString;
    std::string thirdField; // 3 (placeholder)

    // get info from trace file
    while (std::cin >> loadStore >> addressString >> thirdField)
    {
        address = std::stoul(addressString, nullptr, 16);
        cacheSimulator(cache, loadStore, address);
    }

    displayStatistics(cache); // prints final caching statistics
    return 0;
}