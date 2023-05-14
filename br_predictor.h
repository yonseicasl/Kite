#ifndef __BR_PRED_H__
#define __BR_PRED_H__

#include <cstdint>
#include "inst.h"

// Branch predictor
class br_predictor_t {
public:
    br_predictor_t(unsigned m_bht_bits, unsigned m_pht_bits, unsigned m_hist_len);
    ~br_predictor_t();

    bool is_taken(inst_t *m_inst);                      // Is a branch predicted to be taken?
    void update(inst_t *m_inst);                        // Update a prediction counter.

private:
    unsigned *bht;                                      // Branch history table (BHT)
    uint8_t  *pht;                                      // Pattern history table (PHT)
    unsigned  b;                                        // b bits for BHT indexing
    unsigned  p;                                        // p bits for PHT indexing
    unsigned  h;                                        // h-bit branch history per BHT entry
};

// Branch target buffer
class br_target_buffer_t {
public:
    br_target_buffer_t(uint64_t m_size);
    ~br_target_buffer_t();

    uint64_t get_target(uint64_t m_pc);                 // Get a branch target address.
    void update(uint64_t m_pc, uint64_t m_target_addr); // Update the BTB.

private:
    uint64_t num_entries;                               // Number of BTB entries
    uint64_t *buffer;                                   // Target address array
};

#endif

