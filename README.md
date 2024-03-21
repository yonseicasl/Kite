## Kite: Architecture Simulator for RISC-V Instruction Set
*Kite* is an architecture simulator that models a five-stage pipeline of the RISC-V instruction set.
The initial version of Kite was developed in 2019 for educational purposes as a part of EEE3530 Computer Architecture.
Kite implements the five-stage pipeline model described in *Computer Organization and Design, RISC-V Edition: The Hardware and Software Interface by D. Patterson and J. Hennessey* ([Link to Amazon](https://www.amazon.com/Computer-Organization-Design-RISC-V-Architecture/dp/0128122757)).
The objective of Kite is to provide students with an easy-to-use simulation framework and the precise timing model as described in the book.
It supports most of the basic instructions introduced in the book, such as `add`, `slli`, `ld`, `sd`, and `beq` instructions.
Students can easily compose RISC-V assembly programs and execute them through the pipeline model for entry-level architecture studies.
Kite implements several basic functionalities in the pipeline mode, including instruction dependency check (i.e., data hazards), pipeline stalls, data forwarding (optional), branch predictions (optional), data cache structures, etc.

Kite implements the five-stage pipeline model in C++.
Since most computer architecture simulators are written in C/C++, it gives students hands-on experience in computer architecture simulations using a simple, easy-to-use framework.

\
**Prerequisite, Download, and Build:**\
The simple implementation of Kite is easy to install.
It requires only a g++ compiler to build and does not depend on other libraries or external tools.
It was validated in Ubuntu 16.04 (Xenial), 18.04 (Bionic Beaver), 20.04 (Focal Fossa), 22.04 (Jammy Jellyfish) and Mac OS 10.14 (Mojave), 10.15 (Catalina), 11 (Big Sur), 12 (Monterey), 13 (Ventura), 14 (Sonoma).
The latest release of Kite is v1.12 (as of March 2024).
To obtain a copy of Kite v1.12, use the following command in the terminal.
```
$ git clone ––branch v1.12 https://github.com/yonseicasl/kite
```

The following executes an example RISC-V assembly code, `program_code`.
```
$ cd kite/
$ make -j
$ ./kite program_code
```

\
**Documentation:**\
For more detailed information, refer to the documentation in doc/kite.pdf. Please use the following to reference this work.
```
@misc{song_kite2019,
    author       = {W. Song},
    title        = {{Kite: An Architecture Simulator for RISC-V Instruction Set}},
    howpublished = {Yonsei University},
    month        = {May},
    year         = {2019},
    note         = {[Online], Available: \url{https://casl.yonsei.ac.kr/kite}},
}
```

