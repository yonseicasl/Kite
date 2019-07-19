#include "pipe_reg.h"

pipe_reg_t::pipe_reg_t() :
    inst(0) {
}

pipe_reg_t::~pipe_reg_t() {
}

// Read instruction from pipeline register
inst_t* pipe_reg_t::read() const {
    return inst;
}

// Write instruction in pipeline register.
void pipe_reg_t::write(inst_t *m_inst) {
    inst = m_inst;
}

// Remove instruction from pipeline register.
void pipe_reg_t::clear() {
    inst = 0;
}

// Is pipeline register free?
bool pipe_reg_t::is_free() {
    return !inst;
}

