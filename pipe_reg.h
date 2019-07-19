#ifndef __KITE_PIPE_REG_H__
#define __KITE_PIPE_REG_H__

#include "inst.h"

// Pipeline register
class pipe_reg_t {
public:
    pipe_reg_t();
    ~pipe_reg_t();

    inst_t* read() const;           // Read instruction from pipeline register.
    void write(inst_t *m_inst);     // Write instruction in pipeline register.
    void clear();                   // Remove instruction from pipeline register.
    bool is_free();                 // Is pipeline register free?

private:
    inst_t *inst;                   // Instruction currently in pipeline register
};

#endif

