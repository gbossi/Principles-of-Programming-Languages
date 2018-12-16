#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(int argc,char *argv[]) {

    if(argc!=2){
        cout << "Input error. Please write the name of the input file\n";
        return 0;
    }
    char ch;
    fstream fin(argv[1], fstream::in);

    while (fin >> noskipws >> ch) {
        cout << ch; 
    }
}

/*    VirtualMachine vm;

    while(running){
        vm.eval();
        ip++; // increment the ip every iteration
    }
}
*/