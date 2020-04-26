#include <iomanip>
#include <iostream>
#include "proc.h"

using namespace std;

proc_t::proc_t() :
    stalls(0),
    num_insts(0),
#ifdef BR_PRED
    num_br_predicts(0),
    num_br_mispredicts(0),
#endif
    num_flushes(0),
    ticks(0),
    pc(4),  // The first instruction is loaded from PC = 4.
    inst_memory(0),
    br_predictor(0),
    br_target_buffer(0),
    reg_file(0),
    alu(0),
    data_memory(0),
    data_cache(0) {
}

proc_t::~proc_t() {
    // Deallocate datapath elements.
    delete inst_memory;
    delete br_predictor;
    delete br_target_buffer;
    delete reg_file;
    delete alu;
    delete data_memory;
    delete data_cache;
}

// Processor initialization
void proc_t::init(const char *m_program_code) {
    inst_memory = new inst_memory_t(m_program_code);    // Create instruction memory.
    br_predictor = new br_predictor_t(16);              // Create branch predictor.
    br_target_buffer = new br_target_buffer_t(16);      // Create branch target buffer.
    reg_file = new reg_file_t();                        // Create register file.
    alu = new alu_t(&ticks);                            // Create ALU.

    data_memory = new data_memory_t(&ticks, 1024, 0);   // Create data memory.
    data_cache = new data_cache_t(&ticks, 1024, 8, 1);  // Create data cache.
    data_memory->connect(data_cache);                   // Connect memory to cache.
    data_cache->connect(data_memory);                   // Connect cache to memory.
}

// Run the processor pipeline.
void proc_t::run() {
    cout << "Start running ..." << endl;
    while(!ticks || if_id_preg.read()  || id_ex_preg.read()  ||
                    ex_mem_preg.read() || mem_wb_preg.read() ||
                    !alu->is_free()    || !data_cache->is_free()) {
        // Increment clock ticks.
        ticks++;
        // Process pipeline stages backwards.
        writeback();
        memory();
        execution();
        decode();
        fetch();
    }
    cout << "Done." << endl;
    // Print pipeline stats.
    print_stats();
}

// Writeback stage
void proc_t::writeback() {
    // Read instruction from MEM/WB pipeline register.
    inst_t *inst = mem_wb_preg.read();
    if(inst) {
        // Update the number of executed instructions.
        num_insts++;
        // Remove instruction from MEM/WB pipeline register.
        mem_wb_preg.clear();
        // Write result to register file. x0 is discarded.
        if(inst->rd_num > 0) {
            reg_file->write(inst, inst->rd_num, inst->rd_val);
        }
#ifdef DEBUG
        cout << ticks << " : writeback : " << get_inst_str(inst, true) << endl;
#endif
        // Update branch predictor and branch target buffer for conditional branches.
        if(inst->branch_target) {
#ifdef BR_PRED
            num_br_predicts++;
            bool is_taken = (inst->branch_target != (inst->pc+4));
            br_predictor->update(inst->pc, is_taken);
            if(is_taken) {
                br_target_buffer->update(inst->pc, inst->branch_target);
            }
            // Predicted branch target and actual branch target are different.
            // Flush and restart pipeline.
            if(inst->pred_target != inst->branch_target) {
                // Pipeline flushes because of branch mis-prediction.
                // Another cause is due to wrong BTB address despite correct prediction.
                num_br_mispredicts += (inst->pred_taken != is_taken);
                // Flush pipeline, and set correct PC.
                flush();
                pc = inst->branch_target;
#ifdef DEBUG
                cout << ticks << " : pipeline flush : restart at PC = " << pc << endl;
#endif
            }
#else       // No branch prediction is used. The next PC is set here.
            pc = inst->branch_target;
#endif
        }
        // Retire instruction.
        delete inst;
    }
}

// Memory stage
void proc_t::memory() {
    static inst_t *mem_inst = 0;
    // Memory stage makes a progress only if MEM/WB pipeline register is free.
    if(mem_wb_preg.is_free()) {
        // Instruction is read from EX/MEM pipeline register.
        if(data_cache->is_free() && (mem_inst = ex_mem_preg.read())) {
            // Remove instruction from EX/MEM pipeline register.
            ex_mem_preg.clear();
            // Access data memory for load or store.
            if(mem_inst->op == op_ld) { data_cache->read(mem_inst); }
            else if(mem_inst->op == op_sd) { data_cache->write(mem_inst); }
        }
        // Data cache is done with instruction.
        if(!data_cache->run()) {
            // Write instruction in MEM/WB pipeline register.
            mem_wb_preg.write(mem_inst); mem_inst = 0;
        }
    }
#ifdef DEBUG
    inst_t *inst = mem_wb_preg.read();
    if(inst) {
        cout << ticks << " : memory : " << get_inst_str(inst, true) << endl;
    }
#endif
}

// Execution stage
void proc_t::execution() {
    inst_t *inst = 0;
    // Execution stage makes a progress only if EX/MEM pipeline register is free.
    if(ex_mem_preg.is_free()) {
        // Instruction is read from ID/EX pipeline register, and ALU runs the instruction.
        if((inst = id_ex_preg.read()) && alu->is_free()) {
            id_ex_preg.clear();
            alu->run(inst);
        }
        // ALU is done with instruction.
        if((inst = alu->get_output())) {
            // Write instruction in EX/MEM pipeline register.
            ex_mem_preg.write(inst);
        }
    }
#ifdef DEBUG
    if((inst = ex_mem_preg.read())) {
        cout << ticks << " : execution : " << get_inst_str(inst, true) << endl;
    }
#endif
}

// Instruction decode stage
void proc_t::decode() {
    inst_t *inst = 0;
    // Decode stage makes a progress only if ID/EX pipeline register is free.
    if(id_ex_preg.is_free()) {
        // Read instruction from IF/ID pipeline register.
        if((inst = if_id_preg.read())) {
            // Check data dependency of instruction.
            if(!reg_file->dep_check(inst)) {
                // Remove instruction from IF/ID pipeline register.
                if_id_preg.clear();
                // Write instruction in ID/EX pipeline register.
                id_ex_preg.write(inst);
                // Update PC for unconditional jumps.
                if(inst->op == op_jalr) { pc = inst->rs1_val + inst->imm; }
                else if(inst->op == op_jal) { pc = inst->pc + (inst->imm<<1); }
            }
        }
    }
#ifdef DEBUG
    if((inst = id_ex_preg.read())) {
        cout << ticks << " : decode : " << get_inst_str(inst, true) << endl;
    }
#endif
}

// Instruction fetch stage
void proc_t::fetch() {
    inst_t *inst = 0;
    // Fetch stage makes a progress only if IF/ID pipeline register is free.
    if(if_id_preg.is_free()) {
        // Read instruction from instruction memory.
        if((inst = inst_memory->read(pc))) {
            // Update PC.
            pc += 4;
            // Write instruction in IF/ID pipeline register.
            if_id_preg.write(inst);
            // Do branch prediction for conditional branches.
            if(get_op_type(inst->op) == op_sb_type) {
#ifdef BR_PRED
                // Set PC to branch target if predicted to be taken.
                inst->pred_taken = br_predictor->is_taken(inst->pc);
                pc = inst->pred_target = inst->pred_taken ?
                                         br_target_buffer->get_target(inst->pc) : pc;
#else
                // No branch prediction is used.
                // Disable instruction fetching until the next PC is resolved.
                pc = 0;
#endif
            }
        }
    }
    else {
        // Preceding instruction is blocking. Pipeline stalls.
        stalls++;
    }
#ifdef DEBUG
    if((inst = if_id_preg.read())) {
        cout << ticks << " : fetch : " << get_inst_str(inst, true) << endl;
    }
#endif
}

// Flush pipeline.
void proc_t::flush() {
    inst_t *inst = 0;
    // Clear all pipeline registers.
    if((inst = if_id_preg.read()))  { delete inst; if_id_preg.clear();  }
    if((inst = id_ex_preg.read()))  { delete inst; id_ex_preg.clear();  }
    if((inst = ex_mem_preg.read())) { delete inst; ex_mem_preg.clear(); }
    if((inst = mem_wb_preg.read())) { delete inst; mem_wb_preg.clear(); }
    // Flush ALU.
    if((inst = alu->flush())) { delete inst; }
    // Flush dependency check state of register file.
    reg_file->flush();
    num_flushes++;
}

// Print pipeline stats.
void proc_t::print_stats() {
    cout << endl << "======== [Kite Pipeline Stats] =========" << endl;
    cout << "Total number of clock cycles = " << ticks << endl;
    cout << "Total number of stalled cycles = " << stalls << endl;
    cout << "Total number of executed instructions = " << num_insts << endl;
    cout.precision(3);
    cout << "Cycles per instruction = " << fixed
         << double(ticks) / double(num_insts) << endl;
#ifdef BR_PRED
    cout << "Number of pipeline flushes = " << num_flushes << endl;
    cout << "Branch prediction accuracy = " << fixed
         << (num_br_predicts ?
             double(num_br_predicts-num_br_mispredicts) / double(num_br_predicts) : 0)
         << " (" << num_br_predicts-num_br_mispredicts << "/" << num_br_predicts
         << ")" << endl;
#endif
    cout.precision(-1);
    // Print data cache stats.
    data_cache->print_stats();
    // Print register file state.
    cout << endl << "Register state:" << endl;
    for(unsigned i = 0; i < num_kite_regs; i++) {
        cout << "x" << i << " = " << reg_file->read(i) << endl;
    }
    // Print data memory state.
    data_memory->print_state();
    cout << endl << "======== [End of Pipeline Stats] =========" << endl;
}

