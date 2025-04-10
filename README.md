# Operating Systems - Project Collection

This repository contains a set of three academic assignments completed during the Operating Systems course in my Computer Science degree. Each project explores different core principles of low-level programming and system-level process management in C. 

All works are written in C and tested in a Linux environment using standard command-line tools and system calls.

---

## WORK1 - File Comparison Utility (`comp.c`)

A simple command-line utility written in C that compares two files byte-by-byte and returns an appropriate exit code:

- **0** - Files are identical  
- **1** - Files differ in content  
- **2** - Error in opening one of the files

This utility demonstrates:
- Use of system calls (`open`, `read`, `close`)
- Return codes to communicate results
- Terminal-based testing using `$?`

Included - Screenshot of terminal usage (`PartA_terminal.png`)

---

## WORK2 - Client-Server Communication via Files

A simulation of inter-process communication using the file system and Unix signals. The goal is to pass a number from the client to the server, validate it, and return a status code.

### Key Components :
- `client.c`, `server.c` with compiled outputs
- Uses `fork()`, `kill()`, `signal()`, and `wait()`
- Temporary files used to simulate message passing
- Implements timeout mechanism for server response

This assignment focuses on synchronization, signal handling, and safe process communication.

Included - PDFs explaining the solution and sample outputs

---

## WORK3 - Process Scheduler Simulator (`main.c`)

A full C implementation of a scheduling simulator for multiple classic strategies:

- **FCFS** - First-Come First-Served  
- **LCFS (NP)** - Last-Come First-Served, Non-preemptive  
- **LCFS (P)** - Preemptive version  
- **RR** - Round Robin with 2-time quantum  
- **SJF** - Shortest Job First (preemptive)

### Features:
- Parses `input.txt` format as defined by the instructions
- Calculates and prints average turnaround time per strategy
- Clean modular structure for each algorithm

Included - Assignment PDF with detailed rules and example input/output

---

## Compilation & Execution

Each assignment is standalone and can be compiled using `gcc`. For example :

```bash
# WORK1
gcc comp.c -o comp
./comp file1.txt file2.txt

# WORK2
gcc client.c -o client
gcc server.c -o server
./server &
./client

# WORK3
gcc main.c -o main
./main input1.txt
```

---

## Authors

- [Adir Edri](https://github.com/adiredri)
- [Ofir Almog](https://github.com/Ofigu)

---

This project reflects hands-on experience with systems programming, process control, and scheduling logic. Feel free to explore each directory for code, tests, and documentation.
