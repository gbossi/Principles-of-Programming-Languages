#ifndef VM_HPP
#define VM_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <chrono>

//define the default stack size 2^16

#define MAX_PRG    4096
#define MAX_STACK  4096
#define MAX_HEAP   4096
#define HLT 0x00
#define JMP 0x01
#define JNZ 0x02
#define DUP 0x03
#define SWP 0x04
#define DRP 0x05
#define PU4 0x06
#define PU2 0x07
#define PU1 0x08
#define ADD 0x09
#define SUB 0x0a
#define MUL 0x0b
#define DIV 0x0c
#define MOD 0x0d
#define EQ  0x0e
#define NE  0x0f
#define LT  0x10
#define GT  0x11
#define LE  0x12
#define GE  0x13
#define NOT 0x14
#define AND 0x15
#define OR  0x16
#define INP 0x17
#define OUT 0x18
#define CLK 0x2a

#define CNS 0x30
#define HD  0x31
#define TL  0x32

//#define getMarkers(k) (((k) & (3<<30))>>30)

typedef uint8_t byte;
typedef uint32_t word;

#endif
