#include <cstdlib>
#include <cstring>
#include <iostream>
#include "data_cache.h"

using namespace std;

data_cache_t::data_cache_t(uint64_t *m_ticks, uint64_t m_cache_size,
                           uint64_t m_block_size, uint64_t m_ways) :
    memory(0),
    ticks(m_ticks),
    blocks(0),
    cache_size(m_cache_size),
    block_size(m_block_size),
    num_sets(0),
    num_ways(m_ways),
    block_offset(0),
    set_offset(0),
    block_mask(0),
    set_mask(0),
    num_accesses(0),
    num_misses(0),
    num_loads(0),
    num_stores(0),
    num_writebacks(0),
    missed_inst(0) {
    // Calculate the block offset.
    uint64_t val = block_size;
    while(!(val & 0b1)) {
        val = val >> 1; block_offset++;
        block_mask = (block_mask << 1) | 0b1;
    }
    // Check if the block size is a multiple of doubleword.
    if((block_size & 0b111) || (val != 1)) {
        cerr << "Error: cache block size must be a multiple of doubleword" << endl;
        exit(1);
    }

    // Check if the number of ways is a power of two.
    val = num_ways;
    while(!(val & 0b1)) { val = val >> 1; }
    if(val != 1) {
        cerr << "Error: number of ways must be a power of two" << endl;
        exit(1);
    }

    // Calculate the number of sets.
    num_sets = cache_size / block_size / num_ways;
    // Calculate the set offset and mask.
    val = num_sets;
    while(!(val & 0b1)) {
        val = val >> 1; set_offset++;
        set_mask = (set_mask << 1) | 0b1;
    }
    set_offset += block_offset;
    set_mask = set_mask << block_offset;
    // Check if the number of sets is a power of two.
    if(val != 1) {
        cerr << "Error: number of sets must be a power of two" << endl;
        exit(1);
    }
    
    // Allocate cache blocks.
    blocks = new block_t*[num_sets]();
    for(uint64_t i = 0; i < num_sets; i++) { blocks[i] = new block_t[num_ways](); }
}

data_cache_t::~data_cache_t() {
    // Deallocate the cache blocks.
    for(uint64_t i = 0; i < num_sets; i++) { delete [] blocks[i]; }
    delete [] blocks;
}

// Connect to the lower-level memory.
void data_cache_t::connect(data_memory_t *m_memory) { memory = m_memory; }

// Is cache free?
bool data_cache_t::is_free() const { return !missed_inst; }

// Read data from cache.
void data_cache_t::read(inst_t *m_inst) {
    // Check the memory address alignment.
    uint64_t addr = m_inst->memory_addr;
    uint64_t data_size = 0, data_mask = -1, addr_mask = 0;
    bool sign_ext = 1;
    switch(m_inst->op) {
        case op_fld: { sign_ext = 0; }
        case op_ld:  { data_size = 8; data_mask = -1;         addr_mask = 0b111; break; }
        case op_lwu: { sign_ext = 0; }
        case op_lw:  { data_size = 4; data_mask = 0xffffffff; addr_mask = 0b11;  break; }
        case op_lhu: { sign_ext = 0; }
        case op_lh:  { data_size = 2; data_mask = 0xffff;     addr_mask = 0b1;   break; } 
        case op_lbu: { sign_ext = 0; }
        case op_lb:  { data_size = 1; data_mask = 0xff;       addr_mask = 0;     break; }
        default:     { break; }
    }
    // Check if the address is aligned with the data size.
    // Misaligned accesses are prohibited.
    if(addr & addr_mask) {
        cerr << "Error: invalid alignment of memory address " << addr << endl;
        exit(1);
    }

    // Calculate the set index and tag.
    uint64_t set_index = (addr & set_mask) >> block_offset;
    uint64_t tag = addr >> set_offset;

    // Check direct-mapped cache entry.
    block_t *block = &blocks[set_index][0];
    if(!block->valid || (block->tag != tag)) { block = 0; }
    
    if(block) { // Cache hit
        // Update the last access time.
        block->last_access = *ticks;
        // Load data as int or fp.
        int64_t *data = &m_inst->rd_val;
        // Read the right-sized data out of the cache block.
        *data = 0;
        memcpy(data, ((uint8_t*)block->data) + (addr & block_mask), data_size);
        // Do sign extension if necessary.
        *data |= (sign_ext && (*data >> ((data_size << 3) -1))) ? ~data_mask : 0;
#ifdef DATA_FWD
        m_inst->rd_ready = true;
#endif
        num_accesses++;
        num_loads++;
    }
    else { // Cache miss
        missed_inst = m_inst;
        memory->load_block(addr & ~block_mask, block_size);
        num_misses++;
#ifdef DEBUG
        cout << *ticks << " : cache miss : addr = " << addr
             << " (tag = " << tag << ", set = " << set_index << ")" << endl;
#endif
    }
}

// Write data in memory.
void data_cache_t::write(inst_t *m_inst) {
    // Check the memory address alignment.
    uint64_t addr = m_inst->memory_addr;
    uint64_t data_size = 0, addr_mask = 0;
    switch(m_inst->op) {
        case op_fsd:
        case op_sd:  { data_size = 8; addr_mask = 0b111; break; }
        case op_sw:  { data_size = 4; addr_mask = 0b11;  break; }
        case op_sh:  { data_size = 2; addr_mask = 0b1;   break; } 
        case op_sb:  { data_size = 1; addr_mask = 0;     break; }
        default:     { break; }
    }
    // Check if the address is aligned with the data size.
    // Misaligned accesses are prohibited.
    if(addr & addr_mask) {
        cerr << "Error: invalid alignment of memory address " << addr << endl;
        exit(1);
    }

    // Calculate the set index and tag.
    uint64_t set_index = (addr & set_mask) >> block_offset;
    uint64_t tag = addr >> set_offset;

    // Check the direct-mapped cache entry.
    block_t *block = &blocks[set_index][0];
    if(!block->valid || (block->tag != tag)) { block = 0; }

    if(block) { // Cache hit
        // Update the last access time and dirty flag.
        block->last_access = *ticks;
        block->dirty = true;
        // Write the right-sized data in the cache block.
        memcpy(((uint8_t*)block->data) + (addr & block_mask), &m_inst->rs2_val, data_size);
        num_accesses++;
        num_stores++;
    }
    else { // Cache miss
        missed_inst = m_inst;
        memory->load_block(addr & ~block_mask, block_size);
        num_misses++;
#ifdef DEBUG
        cout << *ticks << " : cache miss : addr = " << addr
             << " (tag = " << tag << ", set = " << set_index << ")" << endl;
#endif
    }
}

// Handle a memory response.
void data_cache_t::handle_response(int64_t *m_data) {
    // Calculate the set index and tag.
    uint64_t addr = missed_inst->memory_addr;
    uint64_t set_index = (addr & set_mask) >> block_offset;
    uint64_t tag = addr >> set_offset;

    // Block replacement
    block_t *allocator = &blocks[set_index][0];
    if(allocator->dirty) { num_writebacks++; }
#ifdef DEBUG
    if(allocator->valid) {
        cout << *ticks << " : cache block eviction : addr = " << addr
             << " (tag = " << tag << ", set = " << set_index << ")" << endl;
    }
#endif
    // Place the missed block.
    *allocator = block_t(tag, m_data, /* valid */ true);

    // Replay the cache access.
    if(is_op_load(missed_inst->op)) { read(missed_inst); }
    else { write(missed_inst); }
    // Clear the missed instruction so that the cache becomes free.
    missed_inst = 0;
}

// Run data cache.
bool data_cache_t::run() {
    memory->run();          // Run the data memory.
    return missed_inst;     // Return true if the cache is busy.
}

// Print cache stats.
void data_cache_t::print_stats() {
    cout << endl << "Data cache stats:" << endl;
    cout.precision(3);
    cout << "    Number of loads = " << num_loads << endl;
    cout << "    Number of stores = " << num_stores << endl;
    cout << "    Number of writebacks = " << num_writebacks << endl;
    cout << "    Miss rate = " << fixed
         << (num_accesses ? double(num_misses) / double(num_accesses) : 0)
         << " (" << num_misses << "/" << num_accesses << ")" << endl;
    cout.precision(-1);
}

