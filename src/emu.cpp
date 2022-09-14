#include <bits/stdc++.h>
using namespace std;

void read_file(string file_path);
pair<string, int> convert(string code);
vector<pair<string, int>> split_file(string* s);

vector<pair<string, int>> binary_program;
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

int main(int argc, char** argv) {
    string usage = "Usage : ./emu [filename].o\n";
    if(argc == 1){
        cerr << usage;
        exit(0);
    }
    else if(argc > 2){
        cerr << "Too many arguments\n";
        cerr << usage;
        exit(0);
    }

    string file_path = argv[1];
    if(file_path.substr(file_path.size() - 2, 2) != ".o"){
        cout << "Invalid file name";
        exit(0);
    }

    read_file(file_path);

    for(size_t i = 0; i < binary_program.size(); i++){
        cout << binary_program[i].first << " " << binary_program[i].second << endl;
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
        vector<pair<string, int>> split = split_file(&file_contents);
        for(size_t i = 0; i < split.size(); i++){
            cout << split[i].first << " " << split[i].second << endl;
        }
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