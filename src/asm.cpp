#include <bits/stdc++.h>
using namespace std;

void read_file(string filepath);
void lex_line(string *line, int *pc);
vector<string> clean_file(string *s);
vector<string> split_string(string *s);
void generate_binary();
string string_to_binary(string decimal_string, int pc);
string int_to_binary(int decimal);
bool valid_number(string s);
string trim(string *s);
void create_list_file(string file_path);
string int_to_hex(int a);
string int_to_hex(string a);
void create_log_file(string file_path);


struct program_thing {
    int pc;
    string instruction_or_label;
    string name;
    string operand;
};

unordered_map<string, int> labels;
vector<program_thing> program;
vector<pair<int, string>> binary_program;
vector<string> errors;


bool debug_mode = false;
unordered_map<string, int> instructions = {
    {"data", 19},
    {"ldc", 0},
    {"adc", 1},
    {"ldl", 2},
    {"stl", 3},
    {"ldnl", 4},
    {"stnl", 5},
    {"add", 6},
    {"sub", 7},
    {"shl", 8},
    {"shr", 9},
    {"adj", 10},
    {"a2sp", 11},
    {"sp2a", 12},
    {"call", 13},
    {"return", 14},
    {"brz", 15},
    {"brlz", 16},
    {"br", 17},
    {"HALT", 18},
    {"SET", 20}
};
unordered_map<string, int> value_instructions = {
    {"data", 19},
    {"ldc", 0},
    {"adc", 1},
    {"adj", 10},
    {"SET", 20}
};
unordered_map<string, int> offset_instructions = {
    {"ldl", 2},
    {"stl", 3},
    {"ldnl", 4},
    {"stnl", 5},
    {"call", 13},
    {"brz", 15},
    {"brlz", 16},
    {"br", 17}
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

int main(int argc, char** argv) {
    string file_path;
    string usage = "Usage: ./asm [filename].asm [options]\noptions:\n\t-b : Display debug tokens (file contents, parsed labels)\n";
    if(argc == 1){
        cout << usage;
        exit(0);
    }
    else if(argc == 2 || argc == 3){
        file_path = argv[1];
        if(file_path.substr(file_path.size() - 4, 4) != ".asm"){
            cout << "Invalid file name\n";
            cout << usage;
            exit(0);
        }

        if(argc == 3){
            if(((string) argv[2]).compare("-b") == 0){
                debug_mode = true;
            }
            else{
                cout << "Invalid arguments\n";
                cout << usage;
                exit(0);
            }
        }
    }

    //pass 1
    read_file(file_path);

    


    // print parsed labels and final program
    if(debug_mode){
        cout << "\nParsed labels:\n";
        unordered_map<string, int> :: iterator itr;
        for(itr = labels.begin(); itr != labels.end(); itr++){
            cout << itr->first << " " << itr->second << endl;
        }
        cout << "\nFinal parsed program:\n";
        cout << "(pc, instruction_or_label, name, operand)\n";
        for(size_t i = 0; i < program.size(); i++){
            cout << program[i].pc << ", "<< program[i].instruction_or_label << ", " << program[i].name << ", " << program[i].operand << endl;
        }

    }
    
    //pass 2
    generate_binary();

    // output
    create_list_file(file_path);
    create_log_file(file_path);

    if(debug_mode){
        cout << "\nbinary encoded program:\n";
        for(size_t i = 0; i < binary_program.size(); i++){
            cout << binary_program[i].first << " " << program[i].name << " " << binary_program[i].second << endl;
        }
    }

    if(errors.size() != 0){
        cout << "Errors found in " << file_path << endl;
        cout << "Running o file in emu may have unexpected behaviour" << endl;
    }
    return 0;
}

void read_file(string filepath){
    int pc = 0;
    fstream asmFile;
    asmFile.open(filepath, ios::in);

    vector<string> lines;

    if(asmFile.is_open()){
        std::stringstream buffer;
        buffer << asmFile.rdbuf();
        string file_contents = buffer.str();
        lines = clean_file(&file_contents);


        if(debug_mode){
            cout << "File contents:\n";
            cout << file_contents << "\n";
            cout << "Cleaned File:\n";
        }

        for(size_t i = 0; i < lines.size(); i++){
            if(debug_mode){
                cout << i << " " << lines[i] << "\n";
            }
            lex_line(&lines[i], &pc);
        }
    }
    else{
        cerr << "Failed to open file" << filepath << endl;
        exit(0);
    }
    asmFile.close();
}


void lex_line(string *line, int *pc){
    //line was a comment. ignore
    if(*line == ""){
        return;
    }

    vector<string> tokens = split_string(line);

    // check if the 1st token is a valid instruction
    if(instructions.find(tokens[0]) != instructions.end()){
        if(tokens.size() == 2){
            program_thing a = {
                .pc = *pc,
                .instruction_or_label = "i",
                .name = tokens[0],
                .operand = tokens[1] 
            };
            
            program.push_back(a);
        }
        else{
            program_thing a = {
                .pc = *pc,
                .instruction_or_label = "i",
                .name = tokens[0],
                .operand = ""
            };
            program.push_back(a);
        }
        *pc += 1;
        return;
    }
    else{
        if(tokens.size() == 1){
            if(labels.find(tokens[0]) != labels.end()){
                errors.push_back("Duplicate label detected : " + tokens[0]);
                // cerr << "Duplicate label detected:" << tokens[0] << endl;
            }
            if((tokens[0][0] >= 'a' && tokens[0][0] <= 'z') && tokens[0][tokens[0].size() - 1] == ':'){
                labels.insert(make_pair(tokens[0].substr(0, tokens[0].size() - 1), *pc));
                program_thing a = {
                    .pc = *pc,
                    .instruction_or_label = "l",
                    .name = tokens[0].substr(0, tokens[0].size() - 1),
                    .operand = ""
                };
                program.push_back(a);
            }
            else{
                errors.push_back("Invalid label : " + tokens[0]);
                // cerr << "Invalid label " << tokens[0] << endl;
            }
        }
        else if(tokens.size() == 2){
            if(tokens[1] == ":" && (tokens[0][0] >= 'a' && tokens[0][0] <= 'z')){
                if(labels.find(tokens[0]) != labels.end()){
                    errors.push_back("Duplicate label detected : " + tokens[0]);
                    // cerr << "Duplicate label detected:" << tokens[0] << endl;
                }
                else{
                    labels.insert(make_pair(trim(&tokens[0]), *pc));
                    program_thing a = {
                        .pc = *pc,
                        .instruction_or_label = "l",
                        .name = tokens[0],
                        .operand = ""
                    };
                    program.push_back(a);
                }

            }
            else{
                errors.push_back("Invalid label : " + tokens[0]);
                // cerr << "Invalid label " << tokens[0] << endl;
            }
        }
    }
}

//splits string at ' ' and ':'
vector<string> split_string(string *s){
    vector<string> result;
    string cur = "";
    for(size_t i = 0; i < (*s).size(); i++){
        if(((*s)[i] == ' ' && cur == "") || ((*s)[i] == '\t' && cur == "")){
            continue;
        }
        else if((*s)[i] == ' ' || (*s)[i] == ':'){
            if(cur != ""){
                if((*s)[i] == ':'){
                    cur += ':';
                }
                result.push_back(trim(&cur));
                cur = "";
            }
            else if(cur == "" && (*s)[i] == ':'){
                result.push_back(":");
            }
            continue;
        }
        cur += (*s)[i];
    }

    if(cur.size() != 0){
        result.push_back(trim(&cur));
    }

    return result;
}
// splits the file into lines and removes comments;
vector<string> clean_file(string *s){
    vector<string> result;
    string cur = "";
    for(size_t i = 0; i < (*s).size(); i++){
        if((*s)[i] == ';'){
            //if cur == "" then the line was a comment
            result.push_back(cur);
            cur = "";
            while((*s)[i] != '\n'){
                i++;
            }
            i++;
        }
        else if(((*s)[i] == ' ' && cur == "") || ((*s)[i] == '\t' && cur == "")){
            continue;
        }
        else if((*s)[i] == '\n' || (*s)[i] == ':'){
            if(cur != ""){
                if((*s)[i] == ':'){
                    cur += ':';
                }
                result.push_back(cur);
                cur = "";
            }
            continue;
        }
        cur += (*s)[i];
    }

    if(cur.size() != 0){
        result.push_back(cur);
    }
    return result;
}


void generate_binary(){
    for(size_t i = 0; i < program.size(); i++){
        if(program[i].instruction_or_label == "l"){
            binary_program.push_back(make_pair(program[i].pc, ""));
            continue;
        }
        else{
            string instruction = program[i].name;
            string operand = program[i].operand;
            int pc = program[i].pc;
            if(operand == ""){
                
            }
            // instruction takes numerical value as 
            if(value_instructions.find(instruction) != value_instructions.end()){
                if(valid_number(operand)){
                    binary_program.push_back(make_pair(pc, string_to_binary(operand, i) + int_to_binary(instructions.at(instruction))));
                }
                // data and SET operations only support numerical values as valid operand
                // for other instructions if operand is a valid label then the next instruction next to the label has to be data or SET
                else if((instruction != "data" && instruction != "SET") && labels.find(operand) != labels.end()){
                    int label_pc = labels.at(operand);
                    bool found_valid_instruction = false;
                    for(size_t j = label_pc; j < program.size()-1; j++){
                        if(program[j].name ==  operand && (program[j+1].name == "data" || program[j+1].name == "SET")){
                            found_valid_instruction = true;
                            string new_operand = program[j+1].operand;
                            if(!valid_number(new_operand)){
                                errors.push_back("data and SET instructions only support a valid numerical operand");
                                // cerr << "data and SET instructions only support a valid numerical operand" << endl;
                                binary_program.push_back(make_pair(pc, "00000000000000000000000000000000"));
                            }
                            else{
                                binary_program.push_back(make_pair(pc,string_to_binary(new_operand, i) + int_to_binary(instructions.at(instruction))));
                            }
                            break;
                        }
                    }

                    if(!found_valid_instruction){
                        binary_program.push_back(make_pair(pc, "00000000000000000000000000000000"));
                        errors.push_back("Invalid argument to " + instruction + " : " + operand);
                        // cerr << "Invalid argument to " << instruction << ": " << operand << endl;
                        // exit(0);
                    }
                }
                else{
                    binary_program.push_back(make_pair(pc, "00000000000000000000000000000000"));
                    errors.push_back("Invalid argument to " + instruction + " : " + operand);
                    // cerr << "Invalid argument to " << instruction << ": " << operand << endl;
                    // exit(0);
                }
            }

            // instruction takes offset value
            else if(offset_instructions.find(instruction) != offset_instructions.end()){
                if(valid_number(operand)){
                    binary_program.push_back(make_pair(pc, string_to_binary(operand, i) + int_to_binary(instructions.at(instruction))));
                }
                else if(labels.find(operand) != labels.end()){
                    int offset = labels[operand] - program[i].pc - 1;
                    binary_program.push_back(make_pair(pc, string_to_binary(to_string(offset), i) + int_to_binary(instructions.at(instruction))));
                }
                else{
                    binary_program.push_back(make_pair(pc, "00000000000000000000000000000000"));
                    errors.push_back("Invalid argument to " + instruction + " : " + operand);
                    // cerr << "Invalid argument to " << instruction << ": " << operand << endl;
                    // exit(0);
                }
            }

            // instruction doesnt take any operands
            else if(instructions.find(instruction) != instructions.end()){
                if(operand != ""){
                    binary_program.push_back(make_pair(pc, "00000000000000000000000000000000"));
                    errors.push_back("Unexpected operand " + to_string(pc) + " " + instruction);
                    // cerr << "unexpected operand " << pc << " " << instruction << endl;
                }
                binary_program.push_back(make_pair(pc,"000000000000000000000000" + int_to_binary(instructions.at(instruction))));
            }
        }
    }
}

string string_to_binary(string decimal_string, int pc){
    string res = "";
    if(!valid_number(decimal_string)){
        errors.push_back("Invalid number " + decimal_string);
        // cerr << "Invalid number" << decimal_string << endl;
        // exit(0);
        return "000000000000000000000000";
    }

    // octal number
    if(decimal_string.substr(0,2) == "0o"){
        for(size_t i = 2; i < decimal_string.size(); i++){
            switch(decimal_string[i]){
                case '1': {
                    res += "001";
                    break;
                }
                case '0': {
                    res += "000";
                    break;
                }
                case '2': {
                    res += "010";
                    break;
                }
                case '3': {
                    res += "011";
                    break;
                }
                case '4': {
                    res += "100";
                    break;
                }
                case '5': {
                    res += "101";
                    break;
                }
                case '6': {
                    res += "110";
                    break;
                }
                case '7': {
                    res += "111";
                    break;
                }
                default: {
                    cerr << "Invalid Number " << decimal_string << endl;
                    exit(0);
                }
            }
        }
        if(res.size() > 24){
            errors.push_back("Operand causes overflow " + decimal_string);
            // cerr << "Operand causes overflow " << decimal_string << endl;
            return "000000000000000000000000";
        }
        while(res.size() < 24){
            res = "0" + res;
        }

        return res;
    }

    // hexadecimal number
    else if(decimal_string.substr(0,2) == "0x"){
        for(size_t i = 2; i < decimal_string.size(); i++){
            switch(decimal_string[i]){
                case '0': {
                    res += "0000";
                    break;
                }
                case '1': {
                    res += "0001";
                    break;
                }
                case '2': {
                    res += "0010";
                    break;
                }
                case '3': {
                    res += "0011";
                    break;
                }
                case '4': {
                    res += "0100";
                    break;
                }
                case '5': {
                    res += "0101";
                    break;
                }
                case '6': {
                    res += "0110";
                    break;
                }
                case '7': {
                    res += "0111";
                    break;
                }
                case '8': {
                    res += "1000";
                    break;
                }
                case '9': {
                    res += "1001";
                    break;
                }
                case 'a': 
                case 'A': {
                    res += "1010";
                    break;
                }
                case 'b': 
                case 'B': {
                    res += "1011";
                    break;
                }
                case 'c': 
                case 'C': {
                    res += "1100";
                    break;
                }
                case 'd': 
                case 'D': {
                    res += "1101";
                    break;
                }
                case 'e': 
                case 'E': {
                    res += "1110";
                    break;
                }
                case 'f': 
                case 'F': {
                    res += "1111";
                    break;
                }
            }
        }
        if(res.size() > 24){
            errors.push_back("Operand causes overflow " + decimal_string);
            // cerr << "Operand causes overflow " << decimal_string << endl;
            return "000000000000000000000000";
        }
        while(res.size() < 24){
            res = "0000" + res;
        }
        return res;
    }

    //decimal number
    else{
        int a;
        try{
            a = stoi(decimal_string);
        }
        catch(invalid_argument& e){
            errors.push_back("Invalid arguments to " + to_string(program[pc].pc) + " " + program[pc].name);
            // cerr << "Invalid arguments found for " << program[pc].pc << " " << program[pc].name << endl;
            return "000000000000000000000000";
        }
        for(int i = 23; i >= 0; i--){
            if((a >> i) & 1){
                res.push_back('1');
            }
            else{
                res.push_back('0');
            }
        }

        return res;
    }
}

// convert int to binary. for opcodes
string int_to_binary(int decimal){
    string res = "";
    for(int i = 7; i >= 0; i--){
        if((decimal >> i) & 1){
            res.push_back('1');
        }
        else{
            res.push_back('0');
        }
    }
    return res;
}

string int_to_hex(int a){
    stringstream s;
    s <<  "0x" << setfill('0') << setw(8) << hex << setw(8) << a;
    return s.str();
}

string int_to_hex(string a){
    if(a == ""){
        return "          ";
    }
    stringstream s;
    bitset<32> set(a);
    s <<  "0x" << setfill('0') << setw(8) << hex  << set.to_ulong();
    return s.str();
}

//check if the given string is a valid hexadecimal / octal / decimal number
bool valid_number(string s){
    s = trim(&s);

    // check octal
    if(s.substr(0,2) == "0o"){
        for(size_t i = 2; i < s.size(); i++){
            if(s[i] == ' '){
                break;
            }
            if(s[i] > '7' || s[i] < '0'){
                return false;
            }
        }
    }

    // check hexadecimal
    else if(s.substr(0,2) == "0x"){
        for(size_t i = 2; i < s.size(); i++){
            if(s[i] == ' '){
                break;
            }
            if(!(s[i] >= '0' && s[i] <= '9') && !(s[i] >= 'a' && s[i] <= 'f') && !(s[i] >= 'A' && s[i] <= 'F')){
                return false;
            }
        }
    }

    // check decimal
    else{
        for(size_t i = 0; i < s.size(); i++){
            if(s[i] == ' '){
                break;
            }
            if(i == 0 && (s[i] == '-' || s[i] == '+')){
                if(s.size() > 1){
                    continue;
                }
                else{
                    return false;
                }
            }
            if(s[i] > '9' || s[i] < '0'){
                return false;
            }
        }
    }

    return true;
}

string trim(string *s){
    string res = "";
    bool started = false;
    for(size_t i = 0; i < (*s).size(); i++){
        if(!started && ((*s)[i] == ' ' || (*s)[i] == '\t')){
            continue;
        }
        else if(started && ((*s)[i] == ' ' || (*s)[i] == '\t')){
            break;
        }
        else{
            started = true;
            res += (*s)[i];
        }
    }
    return res;
}


void create_list_file(string file_path){
    ofstream list_file(file_path.substr(0, file_path.size() - 4) + ".l", ios::out);
    for(size_t i = 0; i < program.size(); i++){
        string str = int_to_hex(program[i].pc) + " " + int_to_hex(binary_program[i].second) + " " + program[i].name + " " + program[i].operand + "\n";
        list_file << str;
    }
    list_file.close();
}



void create_log_file(string file_path){
    ofstream log_file(file_path.substr(0, file_path.size() - 4) + ".log", ios::out);
    if(errors.size() == 0){
        log_file << "No errors found\n";
    }
    else{
        log_file << errors.size() << " error(s) found\n";
        for(size_t i = 0; i < errors.size(); i++){
            log_file << errors[i] << "\n";
        }
    }
    log_file.close();
}