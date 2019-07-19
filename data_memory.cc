#include <fstream>
#include <iostream>
#include "defs.h"
#include "data_cache.h"
#include "data_memory.h"

using namespace std;

data_memory_t::data_memory_t(uint64_t *m_ticks, uint64_t m_memory_size, uint64_t m_latency) :
    cache(0),
    ticks(m_ticks),
    memory(0),
    accessed(0),
    memory_size(m_memory_size),
    num_dwords(m_memory_size>>3),
    latency(m_latency),
    resp_ticks(0),
    req_block(0) {
    // Check if memory size is a multiple of doubleword.
    if(memory_size & 0b111) {
        cerr << "Error: memory size must be a multiple of doubleword" << endl;
        exit(1);
    }

    // Allocate memory space in unit of doublewords.
    memory = new int64_t[num_dwords]();
    accessed = new bool[num_dwords]();

    // Load initial memory state.
    load_memory_state();
}

data_memory_t::~data_memory_t() {
    // Deallocate memory space.
    delete [] memory;
    delete [] accessed;
}

// Connect to upper-level cache.
void data_memory_t::connect(data_cache_t *m_cache) { cache = m_cache; }

// Run data memory.
void data_memory_t::run() {
    if(req_block && (*ticks >= resp_ticks)) {
        // Invoke upper-level cache for response.
        cache->handle_response(req_block);
        // Clear requested block.
        req_block = 0;
    }
}

// Load memory block.
void data_memory_t::load_block(uint64_t m_addr, uint64_t m_block_size) {
    // Check doubleword alignment of memory address.
    if(m_addr & 0b111) {
        cerr << "Error: invalid alignment of memory address " << m_addr << endl;
        exit(1);
    }
    // Check if requested block size is within memory space.
    if((m_addr+m_block_size) > memory_size) {
        cerr << "Error: memory address " << m_addr << " is out of bounds" << endl;
        exit(1);
    }

    // Mark all doublewords in the requested block are accessed.
    for(uint64_t i = 0; i < m_block_size>>3; i++) { accessed[(m_addr>>3)+i] = true; }
    // Set pointer to requested block.
    req_block = &memory[m_addr>>3];
    // Set time ticks to respond to cache later.
    resp_ticks = *ticks + latency;
}

// Load initial memory state.
void data_memory_t::load_memory_state() {
    // Open memory state file.
    fstream file_stream;
    file_stream.open("memory_state", fstream::in);
    if(!file_stream.is_open()) {
        cerr << "Error: failed to open memory_state" << endl;
        exit(1);
    }

    string line;
    size_t line_num = 0;
    while(getline(file_stream, line)) {
        line_num++;
        // Crop everything after the comment symbol.
        if(line.find_first_of("#") != string::npos) { line.erase(line.find_first_of("#")); }
        // Erase all spaces.
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        // Skip blank lines.
        if(!line.size()) { continue; }
        // Store memory state.
        size_t l = line.find_first_of("=");
        string addr_str = line.substr(0, l);
        // Trim the line.
        line.erase(0, l+1);
        string data_str = line;

        // Check if memory address is valid.
        if(!is_num_str(addr_str) || !is_num_str(data_str)) {
            cerr << "Error: invalid memory address and/or data " << addr_str
                 << " = " << data_str << " at line #" << line_num
                 << " of memory_state" << endl;
            cout << (((addr_str[0] == '-') ? (addr_str.find_first_not_of("0123456789", 1) == string::npos) : (addr_str.find_first_not_of("0123456789") == string::npos))) << endl;
            cout << is_num_str(addr_str) << endl;
            exit(1);
        }
        // Convert memory address and data string to numbers. 
        uint64_t memory_addr = get_imm(addr_str); 
        int64_t memory_data = get_imm(data_str);
        // Check the alignment of memory address.
        if(memory_addr & 0b111) {
            cerr << "Error: invalid alignment of memory address " << memory_addr
                 << " at line #" << line_num << " of memory_state" << endl;
            exit(1);
        }
        // Memory address cannot go out of bounds.
        if((memory_addr+8) > memory_size) {
            cerr << "Error: memory address " << memory_addr << " is out of bounds"
                 << " at line#" << line_num << " of memory_state" << endl;
            exit(1);
        }
        // Check if multiple memory addresses are defined and conflicting.
        int64_t &dword = memory[memory_addr>>3];
        if(dword && (dword != memory_data)) {
            cerr << "Error: memory address conflicts with " << memory_addr
                 << " at line #" << line_num << " of memory_state" << endl;
            exit(1);
        }
        // Store memory data.
        dword = memory_data;
    }

    // Close memory state file.
    file_stream.close();
}

// Print memory state.
void data_memory_t::print_state() const {
    cout << endl << "Memory state (all accessed addresses):" << endl;
    for(uint64_t i = 0; i < num_dwords; i++) {
        if(accessed[i]) { cout << "(" << (i<<3) << ") = " << memory[i] << endl; }
    }
}

