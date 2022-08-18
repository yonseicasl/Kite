#include <iostream>
#include "proc.h"

using namespace std;

static string banner = "\
***********************************************************\n\
* Kite: Architecture Simulator for RISC-V Instruction Set *\n\
* Developed by William J. Song                            *\n\
* Intelligent Computing Systems Lab, Yonsei University    *\n\
* Version: 1.9                                            *\n\
***********************************************************\n\
";

int main(int argc, char **argv) {
    cout << banner << endl;

    if(argc != 2) {
        cerr << "Usage: " << argv[0] << " [program_code]" << endl;
        exit(1);
    }

    proc_t proc;            // Kite processor
    proc.init(argv[1]);     // Processor initialization
    proc.run();             // Processor runs.
    return 0;
}

