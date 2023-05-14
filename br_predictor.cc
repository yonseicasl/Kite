#include <cstring>
#include "br_predictor.h"

// Branch predictor
br_predictor_t::br_predictor_t(unsigned m_bht_bits, unsigned m_pht_bits, unsigned m_hist_len) :
    bht(0),
    pht(0),
    b(m_bht_bits),
    p(m_pht_bits),
    h(m_hist_len) {
    // Create branch history table (BHT) and pattern history table (PHT).
    bht = new unsigned[1 << b];
    pht = new uint8_t[(1 << p) * (1 << h)];
}

br_predictor_t::~br_predictor_t() {
    // Deallocate the BHT and PHT.
    delete [] bht;
    delete [] pht;
}

// Is a branch predicted to be taken?
bool br_predictor_t::is_taken(inst_t *m_inst) {
    // Predict always not taken.
    return false;
}

// Update a prediction counter.
void br_predictor_t::update(inst_t *m_inst) {
}



// Branch target buffer
br_target_buffer_t::br_target_buffer_t(uint64_t m_size) :
    num_entries(m_size),
    buffer(0) {
    // Create a direct-mapped branch target buffer (BTB).
    buffer = new uint64_t[num_entries];
}

br_target_buffer_t::~br_target_buffer_t() {
    // Deallocate the target address array.
    delete [] buffer;
}

// Get a branch target address.
uint64_t br_target_buffer_t::get_target(uint64_t m_pc) {
    // Always return PC = 0.
    return 0;
}

// Update the branch target buffer.
void br_target_buffer_t::update(uint64_t m_pc, uint64_t m_target_addr) {
}

