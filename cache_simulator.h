#ifndef CACHESIMULATOR_H
#define CACHESIMULATOR_H

#include <cstdint>
#include <string>
#include <vector>

// STRUCTS TO REPRESENT THE CACHE

/**
 * Struct representing a cache slot.
 * Contains information about a single block in the cache.
 */
struct Slot
{
    int tag;            // The tag of the block in memory
    bool valid;         // Indicates if the block is valid
    bool dirty;         // Indicates if the block has been modified
    uint32_t load_ts;   // Timestamp when the block was loaded (used for FIFO)
    uint32_t access_ts; // Timestamp of the last access (used for LRU)
};

/**
 * Struct representing a set of slots in the cache.
 * A cache set contains multiple slots for storing blocks.
 */
struct Set
{
    std::vector<Slot> slots; // A vector of slots in the set
};

/**
 * Struct representing the cache itself.
 * The cache consists of multiple sets.
 * The original number of hits and misses and cycles is zero.
 */
struct Cache
{
    // CACHE INFORMATION (SIZE AND POLICY)
    int numSets;
    int numBlocks;
    int numBytes; // block size
    std::string handleMiss;
    std::string handleWrite;
    std::string handleEviction;
    std::vector<Set> sets; // the different blocks in the cache

    // CACHE STATISTICS
    int loadCount = 0;
    int storeCount = 0;
    int loadHits = 0;
    int loadMisses = 0;
    int storeHits = 0;
    int storeMisses = 0;
    int totalCycles = 0;
};

/**
 * Displays the cache simulation statistics.
 *
 * @param cache Reference to the Cache object that contains the simulation statistics.
 */
void displayStatistics(Cache &cache);

/**
 * Check if a number is a power of two (used in argument validation).
 * @param x user input number to be checked
 * @return true if x is a positive power of 2, false otherwise
 */
bool checkPowTwo(int x);

/**
 * This function validates the provided arguments for cache simulator configuration.
 *
 * @param numSets The number of cache sets (must be greater than 0 and a power of 2).
 * @param numBlocks The number of blocks in each cahce set (must be greater than 0 and a power of 2).
 * @param numBytes The number of bytes in each block cache (must be greater than 4 and a power of 2).
 * @param handleMiss The string describing how to handle cache misses ("write-allocate", "no-write-allocate").
 * @param handleWrite The string describing how to handle cache writes ("write-through", "write-back").
 * @param handleEviction The string describing the eviction policy ("lru", "fifo").
 *
 * @return int 0 for success, 1 for invalid input.
 */
int validateArguments(int numSets, int numBlocks, int numBytes, std::string handleMiss, std::string handleWrite, std::string handleEviction);

/**
 * Computes the cache index for a given memory address.
 *
 * @param address The memory address for which the cache index is being computed.
 * @param cache Reference to the Cache object that contains cache parameters.
 *
 * @return int The index of the cache set where the memory block might be located.
 */
int calculateIndex(int address, Cache &cache);

/**
 * Computes the cache tag for a given memory address.
 *
 * @param address The memory address for which the cache tag is being computed.
 * @param cache Reference to the Cache object that contains cache parameters.
 *
 * @return int The tag for the specified memory address.
 */
int calculateTag(int address, Cache &cache);

/**
 * Sets up the cache parameters and initializes the cache structure.
 *
 * @param cache Reference to the Cache structure to be set up.
 * @param numSets The number of sets in the cache.
 * @param blockSize The size of each block in bytes.
 * @param numBytes The total number of bytes for the cache.
 * @param handleMiss A string indicating the cache miss handling strategy
 * @param handleWrite A string indicating the write handling strategy
 * @param handleEviction A string indicating the eviction policy
 */
void cacheSetUp(Cache &cache, int numSets, int blockSize, int numBytes, std::string handleMiss, std::string handleWrite, std::string handleEviction);

/**
 * Main function for simulating cache operations.
 *
 * @param cache Reference to the Cache structure being simulated.
 * @param loadstore A character indicating the operation to perform: 'l' for load, 's' for store.
 * @param address The memory address involved in the load/store operation.
 */
void cacheSimulator(Cache &cache, char loadStore, int address);

/**
 * Find a block in the cache.
 * @param tag The tag of the block to find
 * @param index The index of the set to search
 * @param cache The cache to search in
 * @return A block if found, or null if not found.
 */
Slot *findBlock(int tag, int index, Cache &cache);

/**
 * Find a block in the cache.
 * @param tag The tag of the block to find
 * @param index The index of the set to search
 * @param cache The cache to search in
 * @return A block if found, or false if not found.
 */
Slot *getBlock(int tag, int index, Cache &cache);

/**
 * Find a block in the cache.
 * @param index The index of the set to search
 * @param cache The cache to search in
 * @return replacement block being used.
 */

Slot *findReplacementBlock(int index, Cache &cache);

/**
 * Handles a load.
 *
 * @param tag The tag of the address that caused the miss.
 * @param index The index of the cache set being accessed.
 * @param cache Reference to the Cache structure containing the sets and slots.
 * @param loadstore A character indicating the operation to perform: 'l' for load, 's' for store.
 * @param block Reference to the relevant Slot structure.
 */
void handleLoad(Cache &cache, char loadStore, int index, int tag, Slot *block);

/**
 * Handles a store.
 *
 * @param tag The tag of the address that caused the miss.
 * @param index The index of the cache set being accessed.
 * @param cache Reference to the Cache structure containing the sets and slots.
 * @param loadstore A character indicating the operation to perform: 'l' for load, 's' for store.
 * @param block Reference to the relevant Slot structure.
 */
void handleStore(Cache &cache, int index, int tag, Slot *block);

/**
 * Handles a miss by locating a suitable slot in the cache.
 *
 * @param tag The tag of the address that caused the miss.
 * @param index The index of the cache set being accessed.
 * @param cache Reference to the Cache structure containing the sets and slots.
 * @param block Reference to the Slot structure to fill in with the new data.
 * @param loadstore A character indicating the operation to perform: 'l' for load, 's' for store.
 */
void handleMiss(int tag, int index, Cache &cache, char loadStore);

/**
 * Finds the Least Recently Used (LRU) block in a given set.
 *
 * @param set Reference to the Set containing the slots to be searched.
 * @param cache Reference to the Cache structure containing the sets and slots.
 * @return Pointer to the Slot that is the least recently used.
 */
Slot *findLRUBlock(Set &set, Cache &cache);

/**
 * Finds the first loaded block in a given set. 
 * 
 * @param set Reference to the Set containing the slots to be searched.
 * @param cache Reference to the Cache structure containing the sets and slots.
 * @return Pointer to the Slot that was loaded first in the set. 
 */
Slot *findFIFOBlock(Set &set, Cache &cache);

#endif // CACHESIMULATOR_H