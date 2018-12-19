#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <chrono>

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
    byte stack[MAX_STACK];

    bool running;
    int ip;
    int sp;
    int pc;
    chrono::steady_clock::time_point time_start;

    void halt();
    void jump();
    void jnz();
    void dup(); 
    void swap(); 
    void drop();
    void push1();
    void push2();
    void push4();
    void add();
    void mul();
    void div();
    void mod();
    void eq();


    void input();
    void output();
    void clock();
   
public:

    VirtualMachine(){
        stack[0]=0;
        running=false;    
        ip=-1;
        sp=-1;
    }


    int prepare(const char* filename){
        FILE* fin= fopen(filename,"rb");

        if (!fin)
            return 1;
    
        fseek(fin,0,SEEK_END);
        long fsize = ftell(fin);

        if(fsize>=MAX_PRG)
            return 2;

        fseek(fin,0,SEEK_SET);
        fread(program,fsize,1,fin);
        fclose(fin);
        program[fsize]=0x00;

        return 0;
    }


    void execute(){
        time_start = chrono::steady_clock::now();
        running=true;        
        byte op;
        do{
            ip++;
            op = program[ip];
            printf("%04x\n", op);
            switch(op){
                case HLT:   halt();   break;
                case JMP:   jump();   break;
                case JNZ:   jnz();    break;
                case DUP:   dup();    break;
                case SWP:   swap();   break;
                case DRP:   drop();   break;                
                case PU4:   push4();  break;
                case PU2:   push2();  break;
                case PU1:   push1();  break;
 /*
                case ADD:   add();    break;           
                case SUB:   sub();  break;
                case MUL:   push1();  break;
                case DIV:   push1();  break;
                case MOD:   push1();  break;
                case EQ:    push1();  break;
                case NE:    push1();  break;
                case LT:    push1();  break;
                case GT:   push1();  break;
                case LE:   push1();  break;
                case GE:   push1();  break;
                case NOT:   push1();  break;
*/

                case INP:   input();  break;
                case OUT:   output();  break;
                case CLK:   clock();  break;
                }

        }while(running);
    }
};





void VirtualMachine::halt(){
    running = false;
    return;
}

void VirtualMachine::jump(){
    ip++;
    byte a = program[ip];
    ip++;
    byte b = program[ip];

    int offset= (int)a + (int) b; ///how to pick the sign
    ip = offset;
    return;
}

void VirtualMachine::jnz(){
    sp++;
    byte a =stack[sp];


    if(a!=0)
        jump();
}


void VirtualMachine::dup(){
    byte a,b;
    if(stack[sp]!=0)
        
        ip++;

}



void VirtualMachine::swap(){
    ip++;

}

void VirtualMachine::drop(){
    ip++;

}


void VirtualMachine::push4(){
    push2();
    push2();
    return;
}

void VirtualMachine::push2(){
    push1();
    push1();
    return;
}


void VirtualMachine::push1(){
    ip++;
    byte a = program[ip];
    sp++;
    stack[sp]=a;
    return;
}

void VirtualMachine::input(){
    byte a;
    scanf("%c", &a);
    sp++;
    stack[sp]=a;
    return;
}

void VirtualMachine::output(){
    byte a = stack[sp];
    sp--;
    printf("%c\n", a);
}



void VirtualMachine::clock(){
    chrono::steady_clock::time_point time_now = chrono::steady_clock::now(); 
    cout << "Elapsed time since start:" << chrono::duration_cast<chrono::microseconds>(time_now-time_start).count()<< endl;

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