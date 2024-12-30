# Cache Simulator

This cache simulator cna be used to study and compare the effectiveness of various cache configurations by reading memory access traces from standard input to simulate how a cache would respond to certain access parameters.

## Makefile:

Generate an executable `cism` using command `make` or `make csim`.
Command `make clean` removes all object files and excutables.

## Trace Data

### Formatting:

- Field 1: `l` or `s` representing a load or store request, respectively
- Field 2: 32-bit address written in hexadecimal
- Field 3: length (unused in the cache simulation)

### Trace Data Example:

`s 0x1fffff50 1`

`l 0x1fffff58 1`

`l 0x1fffff88 6`

`l 0x1fffff90 2`

`l 0x1fffff98 2`

`l 0x200000e0 2`

`l 0x1fffff58 4`

## Simulator Usage:

`./cism <number of sets> <set size (in blocks)> <block size (in bytes)> <miss policy> <write policy> <eviction policy> < <trace file>`

- Number of sets: must be a power of two Set size: must be a power of two
- Block size: must be a power of two and greater than four
- Miss policy: `write-allocate` (misses update the cache) or `no-write-allocate` (do not update the cache on miss)
- Write policy: `write-through` (write to the cache and memory) or `write-back` (write only to the cache, updates dirty block on evictions) -> Note: `no-write-allocate` cannot be used with `write-back`
- Eviction policy: `lru` (evict least recently used) `fifo` (first in, first out)

Example execution command: `./csim 256 4 16 write-allocate write-back lru < tracefile`

This would simulate a 4-way set associative cache with 256 sets, with each block containing 16 bytes of memory; the cache performs write-allocate and write-back, and will evict the least-recently-used block.

## Results

Results are formatted:

`Total loads: <count>`

`Total stores: <count>`

`Load hits: <count>`

`Load misses: <count>`

`Store hits: <count>`

`Store misses: <count>`

`Total cycles: <count>`
