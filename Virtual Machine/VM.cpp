#include "VM.hpp"

using namespace std;
using namespace std::chrono;

class VirtualMachine {
private:
    byte program[MAX_PRG];
    word stack[MAX_STACK];
    word heap[MAX_HEAP];


    bool running;
    int ip;
    int sp;
    int hp;
    steady_clock::time_point time_start;

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
    void sub();
    void mul();
    void div();
    void mod();
    void eq();
    void ne();
    void lt();
    void gt();
    void le();
    void ge();
    void bit_not();
    void bit_and();
    void bit_or();
    void input();
    void output();
    void clock();
   
    void cons();
    void head();
    void tail();

public:

    VirtualMachine(){
        stack[0]=0;
        running=false;    
        ip=-1;
        sp=-1;
        hp=-1;
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
        time_start = steady_clock::now();
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
                case ADD:   add();    break;           
                case SUB:   sub();    break;
                case MUL:   mul();    break;
                case DIV:   div();    break;
                case MOD:   mod();    break;
                case EQ:    eq();     break;
                case NE:    ne();     break;
                case LT:    lt();     break;
                case GT:    gt();     break;
                case LE:    le();     break;
                case GE:    ge();     break;
                case NOT:   bit_not();break;
                case AND:   bit_and();break;
                case OR:    bit_or(); break;
                case INP:   input();  break;
                case OUT:   output(); break;
                case CLK:   clock();  break;

                case CNS:   cons();   break;
                case HD:    head();   break;
                case TL:    tail();   break;
                }

        }while(running);
    }
};





void VirtualMachine::halt(){
    running = false;
    printf("\n\nSTACK\n");
    for(int i=0;i<=sp;i++){
        printf("%i\n",stack[i]);
    }
    return;
}

void VirtualMachine::jump(){
    ip++;
    byte a = program[ip];
    ip++;
    byte b = program[ip];

    int offset= (int) a + (int) b * 16;
    ip = offset;
    return;
}

void VirtualMachine::jnz(){
    word a = stack[sp];
    sp--;
    if(a!=0)
        jump();
}


void VirtualMachine::dup(){
    ip++;
    int diff = program[ip];
    sp++;
    stack[sp]=stack[diff];
}



void VirtualMachine::swap(){
    ip++;
    int diff = program[ip];
    word save = stack[diff];
    stack[diff] = stack[sp];
    stack[sp]=save;
}

void VirtualMachine::drop(){
    sp--;
}


void VirtualMachine::push4(){
    ip++;
    byte a = program[ip];
    ip++;
    byte b = program[ip];
    ip++;
    byte c = program[ip];
    ip++;
    byte d = program[ip];


    sp++;
    stack[sp]=(a+(b<<8)+(c<<16)+(d<<24));

    return;
}

void VirtualMachine::push2(){
    ip++;
    byte a = program[ip];
    ip++;
    byte b = program[ip];

    sp++;
    stack[sp]=(a+(b<<8));

    return;
}


void VirtualMachine::push1(){
    ip++;
    byte a = program[ip];
    sp++;
    stack[sp]=a;
    return;
}

void VirtualMachine::add(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)+(b<<2))>>2|((markerA|markerB)<<30);
    sp--; 
}

void VirtualMachine::sub(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);  
    stack[sp-1]=((a<<2)-(b<<2))>>2|((markerA|markerB)<<30);
    sp--; 

}

void VirtualMachine::mul(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);  
    stack[sp-1]=((a<<2)*(b<<2))>>2|((markerA|markerB)<<30);
    sp--; 
}

void VirtualMachine::div(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);  
    stack[sp-1]=((a<<2)/(b<<2))>>2|((markerA|markerB)<<30);
    sp--; 
}

void VirtualMachine::mod(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);  
    stack[sp-1]=((a<<2)%(b<<2))>>2|((markerA|markerB)<<30);
    sp--; 
}

void VirtualMachine::eq(){
    if(stack[sp-1]==stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;    
}

void VirtualMachine::ne(){
    if(stack[sp-1]!=stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
}

void VirtualMachine::lt(){
    if(stack[sp-1]<stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
}

void VirtualMachine::gt(){
    if(stack[sp-1]>stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
}

void VirtualMachine::le(){
    if(stack[sp-1]<=stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
}

void VirtualMachine::ge(){
    if(stack[sp-1]>=stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
}

void VirtualMachine::bit_not(){
    word a = stack[sp];
    word markerA = getMarkers(a);
    stack[sp]=(~(a>>2))<<2|(markerA<<30); 
}

void VirtualMachine::bit_and(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);  
    stack[sp-1]=((a<<2)&(b<<2))>>2|((markerA|markerB)<<30);
    sp--; 
}

void VirtualMachine::bit_or(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);  
    stack[sp-1]=((a<<2)|(b<<2))>>2|((markerA|markerB)<<30);
    sp--; 
}

void VirtualMachine::input(){
    byte a;
    scanf("%c", &a);
    sp++;
    stack[sp] = (word)a;
}

void VirtualMachine::output(){
    printf("%c\n", stack[sp]);
    sp--;
}


void VirtualMachine::clock(){
    steady_clock::time_point time_now = steady_clock::now(); 
    cout << "Microseconds elapsed since start:" << (double) (duration_cast<microseconds>(time_now-time_start).count()/10^9) << endl;

}


void VirtualMachine::cons(){


    //FIND NEXT FREE HEAP LOCATION
    //IF THERE ISN'T ANY FREE LOCATION (hp == end)
    //CALL THE GARBAGE COLLETCTOR THAT 
    //WILL RETURN THE FIRST FREE ADDRESS
    //OF THE HEAP

    int location= hp;

    word a = stack[sp];
    sp--;

    hp++;
    heap[hp] = a;

    a = stack[sp];
    sp--;

    hp++;
    heap[hp] = a;
}

void VirtualMachine::head(){

    int addr = stack[sp];
    stack[sp]=heap[addr];
    sp--; 
}

void VirtualMachine::tail(){
    int addr = stack[sp];
    stack[sp]=heap[addr+1];
    sp--;
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