#include <iostream>
using namespace std;


int main(int argc, char** argv) {
    int32_t a,b;
    int32_t pc;
    int32_t sp;
    string usage = "Usage: ./asm [filename].asm\n";
    if(argc == 1 || argc > 2){
        cout << usage;
        exit(0);
    }
    string file_path = argv[1];
    if(file_path.substr(file_path.size() - 4, 4) != ".asm"){
        cout << "Invalid file name\n";
        cout << usage;
    }
    
    return 0;
}