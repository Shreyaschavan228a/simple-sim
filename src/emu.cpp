/*
Name: Shreyas Chavan
Roll No: 2001CS67
Date: 16-09-2022
GitHub: https://github.com/Shreyaschavan228a/simple-sim
*/

#include <bits/stdc++.h>
using namespace std;

#define MEMORY_SIZE 10000

void read_file(string file_path);
void dump_memory(int* ptr, string file_path);
void execute();
void trace(string instruction, int operand, int pc);
pair<string, int> convert(string code);
vector<pair<string, int>> split_file(string* s);

map<string, bool> options =  {
    {"trace", false},
    {"isa", false},
    {"before", false},
    {"after", false},
    {"decode", false},
    {"noexec", false}
};

vector<pair<string, int>> binary_program;
void set_option(string option);

unordered_map<int, string> instructions = {
    {19, "data"},
    {0, "ldc"},
    {1, "adc"},
    {2, "ldl"},
    {3, "stl"},
    {4, "ldnl"},
    {5, "stnl"},
    {6, "add"},
    {7, "sub"},
    {8, "shl"},
    {9, "shr"},
    {10, "adj"},
    {11, "a2sp"},
    {12, "sp2a"},
    {13, "call"},
    {14, "return"},
    {15, "brz"},
    {16, "brlz"},
    {17, "br"},
    {18, "HALT"},
    {20, "SET"}
};

unordered_map<string, int> no_operand_instructions = {
    {"add", 6},
    {"sub", 7},
    {"shl", 8},
    {"shr", 9},
    {"a2sp", 11},
    {"sp2a", 12},
    {"return", 14},
    {"HALT", 18}
};


int* memory = reinterpret_cast<int *> (malloc(MEMORY_SIZE * sizeof(int)));
int sp = 0;
int a = 0;
int b = 0;


int main(int argc, char** argv) {
    string usage = "Usage : ./emu [options] [filename].o\noptions:\n\t-decode : print decoded program\n\t-trace : show program trace"
    "\n\t-before : show memory dump before execution\n\t-after : show memory dump after execution\n\t-isa : show assembly isa"
    "\n\t-noexec : dont execute the program. (use with before and/or decode)\n";
    if(argc == 1){
        cerr << usage;
        exit(0);
    }
    else if(argc > 8){
        cerr << "Too many arguments\n";
        cerr << usage;
        exit(0);
    }
    else{
        for(size_t i = 1; i < (size_t)(argc-1); i++){
            set_option(string(argv[i]).substr(1));
        }
    }

    string file_path = argv[argc-1];
    if(file_path.substr(file_path.size() - 2, 2) != ".o"){
        cout << "Invalid file name\n";
        exit(0);
    }

    if(options.at("isa")){
        cout << "Instruction, Opcode" << endl;
        for(int i = 0; i <= 20; i++){
            cout << instructions.at(i) << " "  << i << endl;
        }
        cout << endl;
    }


    read_file(file_path);

    if(options.at("decode")){
        for(size_t i = 0; i < binary_program.size(); i++){
            cout << binary_program[i].first << " " << binary_program[i].second << endl;
        }
        cout << endl;
    }

    if(options.at("before")){
        dump_memory(memory, file_path);
    }

    if(!options.at("noexec")){
        execute();
    }

    if(options.at("after")){
        dump_memory(memory, file_path);
    }
    
    return 0;
}


void read_file(string file_path){
    ifstream obj_file(file_path, ifstream::binary);
    if(!obj_file.good()){
        cerr << "Failed to open file " << file_path << endl;
        exit(0);
    }
    else{
        // get size of object file
        obj_file.seekg(0, ios::end);
        streamsize size = obj_file.tellg();
        obj_file.seekg(0, ios::beg);
        // read from object file and store its contents into buffer
        char buffer[size];
        obj_file.read(buffer, size);
        // make std::string from the buffer
        string file_contents(buffer);
        binary_program = split_file(&file_contents);
    }

    obj_file.close();
}

pair<string, int> convert(string code){
    int opcode;
    stringstream instruction;
    bitset<32> i(code.substr(25, 8));
    instruction << i.to_ulong();
    opcode = (int) stoul(instruction.str());

    stringstream operand;
    int op;    
    // for +ve numbers
    if(code[0] == '0'){
        bitset<32> o(code.substr(0,24));
        operand << o.to_ulong();
        op = (int) stoul(operand.str());
    }
    // for -ve numbers
    else{
        bitset<32> o(code.substr(0,24) + "00000000");
        operand << o.to_ulong();
        op = (int) stol(operand.str()) >> 8;
    }
    return make_pair(instructions.at(opcode), op);
}

vector<pair<string, int>> split_file(string* s){
    vector<pair<string, int>> result;
    string cur = "";
    for(size_t i = 0; i < (*s).size(); i++){
        cur += (*s)[i];
        if(cur.size() % 32 == 0){
            result.push_back(convert(cur));
            cur = "";
        }
    }
    return result;
}


void set_option(string option){
    options[option] = true;
}

void dump_memory(int* ptr, string file_path){
    stringstream ss;
    for(int i = 0; i < MEMORY_SIZE; i++){
        ss << (ptr+i) << " : " << *(ptr+i) << "\n";
    }

    ofstream dump(file_path.substr(0, file_path.size() - 1) + "dump");
    dump << ss.str();
    dump.close();
}


void execute(){
    for(size_t pc = 0; pc < binary_program.size();){
        string instruction = binary_program[pc].first;
        int operand = binary_program[pc].second;
        bool is_parameter = true;
        pc++;
        if(no_operand_instructions.find(instruction) != no_operand_instructions.end()){
            is_parameter = false;
        }
        // output trace before executing the instruction
        if(options.at("trace")){
            if(is_parameter){
                trace(instruction, operand, pc - 1);
            }
            else{
                trace(instruction, INT32_MIN, pc - 1);
            }
        }
       
        if(instruction == "data"){
            *(memory + sp) = operand;
            sp += 1;
        }
        else if(instruction == "ldc"){
            b = a;
            a = operand;
        }
        else if(instruction == "adc"){
            a += operand;
        }
        else if(instruction == "ldl"){
            b = a;
            a = *(memory + sp + operand);
        }
        else if(instruction == "stl"){
            *(memory + sp + operand) = a;
            a = b;
        }
        else if(instruction == "ldnl"){
            a = *(memory + a + operand);
        }
        else if(instruction == "stnl"){
            *(memory + a + operand) = a;
        }
        else if(instruction == "add"){
            a = a + b;
            is_parameter = false;
        }
        else if(instruction == "sub"){
            a = b - a;
            is_parameter = false;
        }
        else if(instruction == "shl"){
            a = b << a;
            is_parameter = false;
        }
        else if(instruction == "shr"){
            a = b >> a;
            is_parameter = false;
        }
        else if(instruction == "adj"){
            sp += operand;
        }
        else if(instruction == "a2sp"){
            sp = a;
            a = b;
            is_parameter = false;
        }
        else if(instruction == "sp2a"){
            b = a;
            a = sp;
            is_parameter = false;
        }
        else if(instruction == "call"){
            b = a;
            a = pc;
            pc += operand;
        }
        else if(instruction == "return"){
            pc = a;
            a = b;
            is_parameter = false;
        }
        else if(instruction == "brz"){
            if(a == 0){
                pc += operand;
            }
        }
        else if(instruction == "brlz"){
            if(a < 0){
                pc += operand;
            }
        }
        else if(instruction == "br"){
            pc += operand;
        }
        else if(instruction == "HALT"){
            is_parameter = false;
            break;
        }
        else if(instruction == "SET"){
            continue;
        }

    }
}


void trace(string instruction, int operand, int pc){
    cout << "PC " << pc << "\t";
    cout << "A " << a << "\t";
    cout << "B " << b << "\t";
    cout << "SP " << sp << "\t";
    cout << instruction << "\t";
    if(operand != INT32_MIN){
        cout << operand << " ";
    }
    cout << "\n";
}