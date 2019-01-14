#include "VM.hpp"



using namespace std;
using namespace std::chrono;

class GarbageCollector{
private:
    word* stack;
    cell* heap;
    int hp;

    void markTheChain(int current){
        if(heap[current].mark)
            return; 
        heap[current].mark=true;
        int left = heap[current].head-ST_ADDRESS;
        int right= heap[current].tail-ST_ADDRESS;

        if(left>=0)
            markTheChain(left);
        if(right>=0)
            markTheChain(right);

    }



    int collectGarbage(int sp){
        int i,k;
        for(i=0;i<=sp;i++){
            k = stack[i]-ST_ADDRESS;
            if(k>=0){
                markTheChain(k);
            }
        }

        for(i=0;i<MAX_HEAP;i++){
            if(!heap[i].mark)
                heap[i].used=false;
            else
                heap[i].mark=false;
            
        }
        return 0;
    }

public:
    void initialize(word (&VMstack)[MAX_STACK],cell (&VMheap)[MAX_HEAP]){
        stack = VMstack;
        heap = VMheap;
        hp = 0;
    }

    int getFreeAddress(int sp){
        if(hp>=MAX_HEAP)
            hp=collectGarbage(sp);
        
        for(;hp<MAX_HEAP;hp++){
            if(!heap[hp].used){
                heap[hp].used = true;
                return hp;
            }
        }
        return getFreeAddress(sp);
    }    
};


class VirtualMachine {
private:
    byte program[MAX_PRG];
    word stack[MAX_STACK];
    cell heap[MAX_HEAP];

    GarbageCollector gc;

    bool running;
    int ip;
    int sp;
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
        
        gc.initialize(stack,heap);

        for(int i=0;i<MAX_STACK;i++)
            stack[i]=0;
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
}

void VirtualMachine::jump(){
    ip = (program[ip+1] | program[ip+2]<<8)-1;
}

void VirtualMachine::jnz(){
    if(stack[sp]!=0){
        sp--;
        ip = (program[ip+1] | program[ip+2]<<8)-1;
    }else{
        sp--;
        ip = ip+2;
    }
}


void VirtualMachine::dup(){
    ip++;
    int diff = sp-program[ip];
    sp++;
    stack[sp]=stack[diff];
}



void VirtualMachine::swap(){
    ip++;
    int diff = sp-program[ip];
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
}

void VirtualMachine::push2(){
    ip++;
    byte a = program[ip];
    ip++;
    byte b = program[ip];

    sp++;
    stack[sp]=(a+(b<<8));
}


void VirtualMachine::push1(){
    ip++;
    byte a = program[ip];
    sp++;
    stack[sp]=a;
}

void VirtualMachine::add(){
    stack[sp-1]=stack[sp-1]+stack[sp];
    sp--; 
}

void VirtualMachine::sub(){
    stack[sp-1]=stack[sp-1]-stack[sp];
    sp--; 
}

void VirtualMachine::mul(){
    stack[sp-1]=stack[sp-1]*stack[sp];
    sp--; 
}

void VirtualMachine::div(){
    stack[sp-1]=stack[sp-1]/stack[sp];
    sp--; 
}

void VirtualMachine::mod(){
    stack[sp-1]=stack[sp-1]%stack[sp];
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
    stack[sp]=~stack[sp];
}

void VirtualMachine::bit_and(){
    stack[sp-1]=stack[sp-1]&stack[sp];
    sp--; 
}

void VirtualMachine::bit_or(){
    stack[sp-1]=stack[sp-1]|stack[sp];
    sp--; 
}

void VirtualMachine::input(){
    byte a;
    scanf("%c", &a);
    sp++;
    stack[sp] = (word)a;
}

void VirtualMachine::output(){
    printf("%c", stack[sp]);
    sp--;
}


void VirtualMachine::clock(){
    steady_clock::time_point time_now = steady_clock::now(); 
    double time_elapsed= duration_cast<microseconds>(time_now-time_start).count();
    printf("%0.6lf\n",time_elapsed/(1000000));

}


void VirtualMachine::cons(){
    int addr = gc.getFreeAddress(sp);
    heap[addr].tail = stack[sp];
    heap[addr].head = stack[sp-1];
    sp--;
    stack[sp]= addr + ST_ADDRESS; 
}

void VirtualMachine::head(){
    int addr = stack[sp] - ST_ADDRESS;
    stack[sp]= heap[addr].head;

}

void VirtualMachine::tail(){
    int addr = stack[sp] - ST_ADDRESS;
    stack[sp]= heap[addr].tail;
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
