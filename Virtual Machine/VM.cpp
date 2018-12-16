#include <iostream>
#include <string>
#include <fstream>


//define the default stack size 2^16

#define DEFAULT_STACK_SIZE 4096
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
typedef byte[2] word;

using namespace std;

class VirtualMachine {
private:
    char stack[DEFAULT_STACK_SIZE];
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

    bool isRunning();
};



VirtualMachine::VirtualMachine(){
    stack[0]=0;
    running=true;    
    ip=0;
    sp=4;
}


void VirtualMachine::halt(){
    ip++;
    running = false;
    return;
}

void VirtualMachine::jump(byte a ,byte b){
    ip++;
    int offset= (int)a + (int) b; ///how to pick the sign
    ip += offset;
    return;
}


bool VirtualMachine::isRunning(){
    return running;
}

byte getb(FILE* fin){
    return (byte) fgetc(fin);
}

int main(int argc,char *argv[]) {
    VirtualMachine vm;
    byte op;

    if(argc!=2){
        cout << "Please write the name of the input file\n";
        return 0;
    }

    FILE* fin=fopen(argv[1], "r");

    if (!fin){
        perror ("Error opening file\n");
        return 0;
    }
    

    do{
        op=getb(fin);
        printf("%04x-",op);
        printf("%04x||",HLT);
        switch(op){
            case HLT: vm.halt();break;
            case JMP: vm.jump(getb (fin),getb (fin));break; 

        } 
    }while(vm.isRunning() && op != EOF);

    fclose(fin);
}