#include <iostream>
#include <string>
#include <cmath>

#include "cache_simulator.h"

void displayStatistics(Cache &cache)
{
    std::cout << "Total loads: " << cache.loadCount << std::endl;
    std::cout << "Total stores: " << cache.storeCount << std::endl;
    std::cout << "Load hits: " << cache.loadHits << std::endl;
    std::cout << "Load misses: " << cache.loadMisses << std::endl;
    std::cout << "Store hits: " << cache.storeHits << std::endl;
    std::cout << "Store misses: " << cache.storeMisses << std::endl;
    std::cout << "Total cycles: " << cache.totalCycles << std::endl;
}

bool checkPowTwo(int x)
{
    // false: must be positive
    if (x < 1)
    {
        return false;
    }
    if (((x & (x - 1)) == 0))
    // sets lowest bit to 0: powers of two should only have one set bit, equals zero
    {
        return true;
    }
    // false: not a power of 2
    return false;
}

// return 0 for success, 1 for bad input
int validateArguments(int numSets, int numBlocks, int blockSize, std::string handleMiss, std::string handleWrite, std::string handleEviction)
{
    // number of sets is a positive power of 2
    if (numSets < 1 || !checkPowTwo(numSets))
    {
        std::cerr << "Invalid number of sets. Exiting.\n";
        return 1;
    }

    // number of blocks is a positive power of 2
    if (numBlocks < 1 || !checkPowTwo(numBlocks))
    {
        std::cerr << "Invalid block size. Exiting.\n";
        return 1;
    }

    // byte size is a positive power of 2 and at least 4
    if (blockSize < 4 || !checkPowTwo(blockSize))
    {
        std::cerr << "Invalid number of bytes. Exiting.\n";
        return 1;
    }

    if (handleMiss != "write-allocate" && handleMiss != "no-write-allocate")
    {
        std::cerr << "Invalid input, not write-allocate or no-write-allocate. Exiting.\n";
        return 1;
    }

    if (handleWrite == "write-through")
    {
        // no action needed shouldn't do anything
    }
    else if (handleWrite == "write-back")
    {
        if (handleMiss == "no-write-allocate")
        {
            // cannot have both write-back and no-write-allocate
            std::cerr << "Invalid input, not write-through or write-back or conflict with no-write-allocate. Exiting.\n";
            return 1;
        }
    }
    else
    {
        std::cerr << "Invalid input. Exiting.\n";
        return 1;
    }

    if (handleEviction != "lru" && handleEviction != "fifo")
    {
        std::cerr << "Invalid input, not lru or fifo. Exiting.\n";
        return 1;
    }

    return 0;
}

int calculateIndex(int address, Cache &cache)
{
    int offset = log2(cache.numBytes);
    int index = log2(cache.numSets);
    int shifted = address >> offset;
    int mask = (1 << index) - 1;

    return (shifted & mask);
}

int calculateTag(int address, Cache &cache)
{
    int offset = log2(cache.numBytes);
    int index = log2(cache.numSets);
    return (address >> (offset + index));
}

void cacheSetUp(Cache &cache, int numSets, int numBlocks, int numBytes, std::string handleMiss, std::string handleWrite, std::string handleEviction)
{
    // initialize blocks with default values
    for (int i = 0; i < numSets; i++)
    {
        Set set;
        for (int j = 0; j < numBlocks; j++)
        {
            Slot slot;
            slot.tag = 0;
            slot.valid = false;
            slot.dirty = false;
            slot.load_ts = 0;
            slot.access_ts = 0;
            set.slots.push_back(slot);
        }
        cache.sets.push_back(set);
    }

    // set up cache size
    cache.numSets = numSets;
    cache.numBlocks = numBlocks;
    cache.numBytes = numBytes;

    // set up cache policies
    cache.handleMiss = handleMiss;
    cache.handleWrite = handleWrite;
    cache.handleEviction = handleEviction;
}

void handleLoad(Cache &cache, int index, int tag, Slot *hit)
{
    cache.loadCount++;
    // load hit
    if (hit)
    {
        cache.loadHits++;
        cache.totalCycles++;
        if (cache.handleEviction == "lru")
        {
            hit->access_ts = cache.totalCycles; // uses cycles as a timestamp
        }
    }
    // load miss
    else
    {
        cache.loadMisses++;
        cache.totalCycles += 100 * cache.numBytes / 4; // pretend we access from memory here
        // if it's a miss, bring the info from the memory into the cache
        Slot *temp = findReplacementBlock(index, cache);
        cache.totalCycles++; // time for updating cache
        temp->valid = true;
        temp->tag = tag;
        if (cache.handleEviction == "lru")
        {
            temp->access_ts = cache.totalCycles;
        } 
        else //fifo 
        {
            temp->load_ts = cache.totalCycles;
        }
    }
}

void handleStore(Cache &cache, int index, int tag, Slot *hit)
{
    cache.storeCount++;
    if (hit)
    {
        cache.storeHits++;
        hit->access_ts = cache.totalCycles;
        if (cache.handleWrite == "write-back")
        {
            hit->dirty = true;
            cache.totalCycles++;
        }
        else // write-through policy
        {
            cache.totalCycles += 100; // simulate cost of writing to memory and to cache
        }
    }
    // store miss
    else
    {
        cache.storeMisses++;
        if (cache.handleMiss == "no-write-allocate")
        {
            cache.totalCycles += 100; // writes directly to memory
            return;
        }
        // write-allocate policy
        else
        {
            cache.totalCycles += 100 * (cache.numBytes / 4); // getting the block from memory
            Slot *temp = findReplacementBlock(index, cache);
            cache.totalCycles++; // time for updating cache
            temp->valid = true;
            temp->tag = tag;
            if (cache.handleEviction == "lru")
            {
                temp->access_ts = cache.totalCycles;
            }
            else //fifo
            {
                temp->load_ts = cache.totalCycles;
            }
            if (cache.handleWrite == "write-back")
            {
                temp->dirty = true;
                // not stored in memory yet
            }
        }
    }
}

void cacheSimulator(Cache &cache, char loadStore, int address)
{
    // get block information
    int index = calculateIndex(address, cache);
    int tag = calculateTag(address, cache);

    // find block with the corresponding address
    Slot *hit = findBlock(tag, index, cache);

    if (loadStore == 'l') // load/read
    {
        handleLoad(cache, index, tag, hit);
    }
    else // store/write
    {
        handleStore(cache, index, tag, hit);
    }
}

Slot *findBlock(int tag, int index, Cache &cache)
{
    // check index line for tag
    Set &set = cache.sets[index];
    for (Slot &slot : set.slots)
    {
        // check if the slot contains a valid block and if the tag matches
        if (slot.valid && (slot.tag == tag))
        {
            // return block if found (hit)
            return &slot;
        }
    }
    return nullptr;
}

Slot *findReplacementBlock(int index, Cache &cache)
{
    Slot *temp = nullptr;
    // can fill in invalid slot
    Set &set = cache.sets[index];
    for (Slot &slot : set.slots)
    {
        if (slot.valid == false)
        {
            temp = &slot;
            return temp;
        }
    }
    // no invalids, needs to evict
    if (temp == nullptr)
    {
        // get replacement blocks
        if (cache.handleEviction == "lru")
        {
            temp = findLRUBlock(cache.sets[index], cache);
        }
        else //fifo
        {
            temp = findFIFOBlock(cache.sets[index], cache);
        }
    }
    return temp;
}

Slot *findLRUBlock(Set &set, Cache &cache)
{
    Slot *temp = &(set.slots[0]);
    for (Slot &slot : set.slots)
    {
        // find min
        if (slot.access_ts < temp->access_ts)
        {
            temp = &slot;
        }
    }

    if (temp->dirty && cache.handleWrite == "write-back")
    {
        temp->valid = false;
        temp->dirty = false;
        cache.totalCycles += 100 * cache.numBytes / 4; // store to memory
    }

    return temp;
}

Slot *findFIFOBlock(Set &set, Cache &cache) {
    Slot *temp = &(set.slots[0]);
    for (Slot &slot : set.slots)
    {
        // find min
        if (slot.load_ts < temp->load_ts)
        {
            temp = &slot;
        }
    }

    if (temp->dirty && cache.handleWrite == "write-back")
    {
        temp->valid = false;
        temp->dirty = false;
        cache.totalCycles += 100 * cache.numBytes / 4; // store to memory
    }

    return temp;
}