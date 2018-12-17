#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>


//define the default stack size 2^16

#define MAX_PRG    4096
#define MAX_STACK  1024
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


typedef unsigned char byte;
typedef uint32_t word;


using namespace std;

class VirtualMachine {
private:
    byte program[MAX_PRG];
    word stack[MAX_STACK];

    bool running;
    int ip;
    int sp;
   
public:
    VirtualMachine();
    void halt(); 
    void jump(byte,byte);
    void jnz(byte,byte);
    void dup(); // I don't know
    void swap(); //I don't know
    void drop();
    void push1(byte);
    void push2(byte,byte);
    void push4(byte,byte,byte,byte);
    void add();
    void mul();
    void div();
    void mod();
    void eq();


    int prepare(const char* filename){
        FILE* fin= fopen(filename,"rb");

        if (!fin)
            return 1;
    
        fseek(fin,0,SEEK_END);
        long fsize = ftell(fin);

        if(fsize>=MAX_PRG)
            return 1;

        fseek(fin,0,SEEK_SET);
        fread(program,fsize,1,fin);
        fclose(fin);
        program[fsize]=0x00;

        return 0;
    }


    void execute(){
        running=true;        
        byte op;
        do{
            op = program[ip];
            printf("%04x\n", op);
            switch(op){
                case HLT: halt();break;
                } 
             ip++;
        }while(running);
    }
};



VirtualMachine::VirtualMachine(){
    stack[0]=0;
    running=false;    
    ip=0;
    sp=0;
}


void VirtualMachine::halt(){
    ip++;
    running = false;
    return;
}

void VirtualMachine::jump(byte a ,byte b){
    ip++;
    int offset= (int)a + (int) b; ///how to pick the sign
    ip = offset;
    return;
}


int main(int argc,char *argv[]) {
    if(argc!=2){
        printf("Please write the name of the input file\n");
        return 0;
    }
   
    VirtualMachine vm;
    switch(vm.prepare(argv[1])){
        case 0: vm.execute();break;
        case 1: perror("Error in Filename");break;
        case 2: perror("Error file exceeds preallocated space");break;
    }

    return 0;
}