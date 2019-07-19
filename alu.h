#ifndef __KITE_ALU_H__
#define __KITE_ALU_H__

#include <stdint.h>
#include "inst.h"

// Arithmetic-logical unit (ALU)
class alu_t {
public:
    alu_t(uint64_t *m_ticks);
    ~alu_t();

    inst_t* get_output();       // Get an instruction leaving the ALU.
    bool is_free();             // Is ALU free?
    void run(inst_t *m_inst);   // Execute the instruction.
    inst_t* flush();            // Remove instruction from the ALU.

private:
    uint64_t *ticks;            // Pointer to processor clock ticks
    uint64_t exit_ticks;        // Exit ticks for run_inst to leave the ALU
    inst_t *run_inst;           // Instruction currently in the ALU
};

#endif

