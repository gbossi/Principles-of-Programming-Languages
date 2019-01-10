#include "gc.hpp"

#define DEBUG 1
#define DEBUG_GC 1

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

    int END_OF_HEAP = MAX_HEAP / 2 - 1;

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
        hp=0;
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
        }while(running&&ip<89);
    }


    int findNextFree();
    void clearHeapMarkers();
    void mark();
    int getMarkers( word addr );
    int getHeapAddressFromElement( word element );
    void markAndCheck( int addr );
};





void VirtualMachine::halt(){
    running = false;
}

void VirtualMachine::jump(){
    ip++;
    byte a = program[ip];

    ip++;
    byte b = program[ip];

    ip = (a | b<<8)-1;

    if(DEBUG)
        printf("JUMP =\t ip=%d\n",ip);
}

void VirtualMachine::jnz(){
    word a = stack[sp];
    sp--;
    if(DEBUG)
        printf("JNZ =\t %d\n",a);
    if(a!=0){
        jump();
    }else{
        ip = ip+2;
    }
}


void VirtualMachine::dup(){
    ip++;
    int diff = sp-program[ip];
    sp++;
    stack[sp]=stack[diff];
    if(DEBUG)
        printf("CP =\t %d in sp\n",stack[diff]);
}



void VirtualMachine::swap(){
    ip++;
    int diff = program[ip];
    word save = stack[diff];
    stack[diff] = stack[sp];
    stack[sp]=save;
    if(DEBUG)
        printf("SWAP =\t %d@%d with %d@%d\n",save,diff,stack[sp],sp);
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

    if(DEBUG)
        printf("PUSH4 =\t stack[sp]=%d\n",stack[sp]);

    return;
}

void VirtualMachine::push2(){
    ip++;
    byte a = program[ip];
    ip++;
    byte b = program[ip];

    sp++;
    stack[sp]=(a+(b<<8));

    if(DEBUG)
        printf("PUSH2 =\t stack[sp]=%d\n",stack[sp]);

    return;
}


void VirtualMachine::push1(){
    ip++;
    byte a = program[ip];
    sp++;
    stack[sp]=a;

    if(DEBUG)
        printf("PUSH1 =\t stack[sp]=%d\n",stack[sp]);
}

void VirtualMachine::add(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)+(b<<2))>>2|((markerA|markerB)<<30);
    sp--;

    if(DEBUG)
        printf("ADD =\t %d+%d=%d\n",a,b,stack[sp]);
}

void VirtualMachine::sub(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)-(b<<2))>>2|((markerA|markerB)<<30);
    sp--;

    if(DEBUG)
        printf("SUB =\t %d-%d=%d\n",a,b,stack[sp]);
}

void VirtualMachine::mul(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)*(b<<2))>>2|((markerA|markerB)<<30);
    sp--;
    if(DEBUG)
        printf("MUL =\t %d*%d=%d\n",a,b,stack[sp]);
}

void VirtualMachine::div(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)/(b<<2))>>2|((markerA|markerB)<<30);
    sp--;
    if(DEBUG)
        printf("DIV =\t %d/%d=%d\n",a,b,stack[sp]);
}

void VirtualMachine::mod(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)%(b<<2))>>2|((markerA|markerB)<<30);
    sp--;
    if(DEBUG)
        printf("MOD =\t %d*/%d=%d\n",a,b,stack[sp]);
}

void VirtualMachine::eq(){
    if(DEBUG)
        printf("EQ =\t %d==%d",stack[sp],stack[sp-1]);
    if(stack[sp-1]==stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
    if(DEBUG)
        printf(" -> %d\n", stack[sp]);
}

void VirtualMachine::ne(){
    if(DEBUG)
        printf("NE =\t %d!=%d",stack[sp],stack[sp-1]);
    if(stack[sp-1]!=stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
    if(DEBUG)
        printf(" -> %d\n", stack[sp]);
}

void VirtualMachine::lt(){
    if(DEBUG)
        printf("LT =\t %d<%d",stack[sp-1],stack[sp]);
    if(stack[sp-1]<stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
    if(DEBUG)
        printf(" -> %d\n", stack[sp]);
}

void VirtualMachine::gt(){
    if(DEBUG)
        printf("GT =\t %d>%d",stack[sp-1],stack[sp]);
    if(stack[sp-1]>stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
    if(DEBUG)
        printf(" -> %d\n", stack[sp]);
}

void VirtualMachine::le(){
    if(DEBUG)
        printf("LE =\t %d<=%d",stack[sp-1],stack[sp]);
    if(stack[sp-1]<=stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
    if(DEBUG)
        printf(" -> %d\n", stack[sp]);
}

void VirtualMachine::ge(){
    if(DEBUG)
        printf("LT =\t %d>=%d",stack[sp-1],stack[sp]);
    if(stack[sp-1]>=stack[sp])
        stack[sp-1]=1;
    else
        stack[sp-1]=0;
    sp--;
    if(DEBUG)
        printf(" -> %d\n", stack[sp]);
}

void VirtualMachine::bit_not(){
    word a = stack[sp];
    word markerA = getMarkers(a);
    stack[sp]=(~(a>>2))<<2|(markerA<<30);
    if(DEBUG)
        printf("BITNOT =\t %d= ~%d\n",a,stack[sp]);
}

void VirtualMachine::bit_and(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)&(b<<2))>>2|((markerA|markerB)<<30);
    sp--;
    if(DEBUG)
        printf("BITAND =\t %d&%d=%d\n",a,b,stack[sp]);
}

void VirtualMachine::bit_or(){
    word b = stack[sp];
    word a = stack[sp-1];
    word markerB = getMarkers(b);
    word markerA = getMarkers(a);
    stack[sp-1]=((a<<2)|(b<<2))>>2|((markerA|markerB)<<30);
    sp--;
    if(DEBUG)
        printf("BITOR =\t %d|%d=%d\n",a,b,stack[sp]);
}

void VirtualMachine::input(){
    byte a;
    scanf("%c", &a);
    sp++;
    stack[sp] = (word)a;
    if(DEBUG)
        printf("INPUT =\t %c\n", stack[sp]);
}

void VirtualMachine::output(){
    printf("%c", stack[sp]);
    if(DEBUG)
        printf("\nOUT =\t %d\n", stack[sp]);
    sp--;
}


void VirtualMachine::clock(){
    steady_clock::time_point time_now = steady_clock::now();
    double time_elapsed= duration_cast<microseconds>(time_now-time_start).count();
    printf("%0.6lf\n",time_elapsed/(1000000));

}


void VirtualMachine::cons()
{


    //FIND NEXT FREE HEAP LOCATION
    //IF THERE ISN'T ANY FREE LOCATION (hp == end)
    //CALL THE GARBAGE COLLETCTOR THAT
    //WILL RETURN THE FIRST FREE ADDRESS
    //OF THE HEAP

    word a = stack[sp];
    sp--;

    // store first element
    int location = findNextFree();
    heap[location] = a;

    a = stack[sp];
    sp--;

    // store second element
    heap[location + 1] = a;

    sp++;
    // return location back to stack (marked as address to heap)

    if( DEBUG_GC )
    	printf("location: %04x\n", location);

    stack[sp] = location | (1 << 30);

	location = location | (1 << 30);

	if( DEBUG_GC )
	{
		printf("location: %04x\n", location);
		printf("markers: %d\n", location >> 30);
	}

}

void VirtualMachine::head()
{
	int addr = stack[sp];

	if( DEBUG_GC )
		printf("getting first...\n");

	// check if address to cons cell
	if( getMarkers( addr ) > 0b00 )
	{
		stack[sp]=heap[getHeapAddressFromElement(addr)];

		if( DEBUG_GC )
			printf("********** returning first! ***********\n");
	}

}

void VirtualMachine::tail()
{
    int addr = stack[sp];

    if( DEBUG_GC )
    {
		printf("getting tail..\n");
		printf("markers: %d\n", getMarkers(addr));
    }

    // check if element is address to cons cell
    if( getMarkers( addr ) > 0b00 )
    {
    	stack[sp]=heap[getHeapAddressFromElement(addr)+1];
    	if(DEBUG_GC)
    		printf(" * * * * *returning tail!\n");
    }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// Garbage Collector
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int VirtualMachine::findNextFree()
{
  // all memory used, clear memory
  if( hp >= END_OF_HEAP )
  {
	clearHeapMarkers();
    mark();
    hp = 0;
  }

  if( DEBUG_GC )
	  printf("hp = %i\n", hp);

  for( int index = 0; index < END_OF_HEAP; index++)
  {
	//printf("current element: %04x\n", heap[( index ) * 2]);
	//printf( "current Markers: %i\n", getMarkers(heap[( index ) * 2] ) < 2 );
    if( getMarkers(heap[( index ) * 2] ) < 0b10 )
    {
    	//printf("filling index %i\n", index);
    	hp = index;
    	return index;
    }
  }
  printf("# # # # # # # # Error, found no empty space!\n");
  exit(-1);
}

void VirtualMachine::clearHeapMarkers()
{
	if( DEBUG_GC )
		printf("Clearing heap markers...\n");

  for( int i = 0; i < END_OF_HEAP; i++ )
  {
    heap[i * 2] = heap[i * 2] & 0x7fffffff;
  }
}

void VirtualMachine::mark()
{
	if( DEBUG_GC )
		printf("marking used...\n");

  // find and mark used elements
  for( int index = 0; index <= sp; index++ )
  {
	  if( DEBUG_GC )
	  {
		  printf("index: %d\n", index);
		  printf("markers: %d\n", getMarkers( stack[index] ));
		  printf("val: %04x\n", stack[index]);
	  }

    if( getMarkers( stack[index] ) > 0 ) // check if element is pointer to heap
    {

      int addr = getHeapAddressFromElement( stack[index] );
      markAndCheck( addr );
    }
  }

  if( DEBUG_GC )
	  printf("Done.\n");
}

void VirtualMachine::markAndCheck( int addr )
{
  if( getMarkers(heap[addr * 2] ) & 0b10 )
  {
	  if( DEBUG_GC )
		  printf("already visited cell, returning.");

	  return; // already visited
  }

  if( DEBUG_GC )
	  printf("marking new cell.");

  heap[addr * 2] = heap[addr * 2] | (1 << 31); // mark cell as visited by gc

  /*
  //no circular dependencies
  // check if cell contains address to other cell(s)
  if( getMarkers( heap[addr * 2] ) & 0b01 )
  {
    markAndCheck( getHeapAddressFromElement( heap[addr * 2] ) );
    markAndCheck( getHeapAddressFromElement( heap[addr * 2 + 1] ) );
  }
  */
}

// return bits 32 and 31
int VirtualMachine::getMarkers( word addr )
{
  return (int) addr >> 30;
}


int VirtualMachine::getHeapAddressFromElement( word element )
{
  return (int) element & 0x3fffffff; // get 30 bits
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
