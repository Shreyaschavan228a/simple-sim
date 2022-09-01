#include <bits/stdc++.h>
using namespace std;

void read_file(string filepath);
void lex_line(string *line, int line_number);
vector<string> clean_file(string *s);
vector<string> split_string(string *s);

unordered_map<string, int> labels;
vector<pair<string, string>> program; 

bool debug_mode = false;
unordered_map<string, int> instructions = {
    {"data", NULL},
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
    {"SET", NULL}
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


    read_file(file_path);

    // print parsed labels and final program
    if(debug_mode){
        cout << "\nParsed labels:\n";
        unordered_map<string, int> :: iterator itr;
        for(itr = labels.begin(); itr != labels.end(); itr++){
            cout << itr->first << " " << itr->second << endl;
        }
        cout << "\nFinal parsed program:\n";
        for(size_t i = 0; i < program.size(); i++){
            cout << program[i].first << " " << program[i].second << endl;
        }
    }
    return 0;
}

void read_file(string filepath){
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
            lex_line(&lines[i], i);
        }
    }
    else{
        cerr << "Failed to open file" << filepath << endl;
        exit(0);
    }
    asmFile.close();
}


void lex_line(string *line, int line_number){
    //line was a comment. ignore
    if(*line == ""){
        return;
    }

    vector<string> tokens = split_string(line);

    // this block only runs when the instruction is associated with a label 
    //and there is a space between ':' and the first letter of the instruction.
    // find a way to fix so that this isnt needed
    if(tokens.size() == 3){
        if(instructions.find(tokens[1]) != instructions.end()){
            program.push_back(make_pair(tokens[1], tokens[2]));
            return;
        }
    }

    //check if the 0th token is a valid instruction
    if(!(tokens[0][0] <= 'z' && tokens[0][0] >= 'a')){
        tokens[0]  = tokens[0].substr(1, tokens[0].size() - 1);
    }
    
    if(instructions.find(tokens[0]) != instructions.end()){
        //found valid instruction thats not part of a label
        
        // instructions associated with a label
        if(tokens.size() == 3){
            program.push_back(make_pair(tokens[1], tokens[2]));
        }
        //free instructions
        else if(tokens.size() == 2){
            program.push_back(make_pair(tokens[0], tokens[1]));
        }
        else{
            program.push_back(make_pair(tokens[0], ""));
        }
        return;
    }
    else{
        if(tokens[0][0] >= 'a' && tokens[0][0] <= 'z'){
            labels.insert(make_pair(tokens[0], line_number));
            program.push_back(make_pair(tokens[0], "label"));
        }
    }
}

//splits string at ' ' and ':'
vector<string> split_string(string *s){
    vector<string> result;
    string cur = "";
    for(size_t i = 0; i < (*s).size(); i++){
        if((*s)[i] == ' ' && cur == ""){
            continue;
        }
        else if((*s)[i] == ' ' || (*s)[i] == ':'){
            if(cur != ""){
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
// splits the file into lines and removes comments;
vector<string> clean_file(string *s){
    vector<string> result;
    string cur = "";
    for(size_t i = 0; i < (*s).size(); i++){
        if((*s)[i] == ' ' && cur == ""){
            continue;
        }
        else if((*s)[i] == '\n' || (*s)[i] == ':'){
            if(cur != ""){
                if((*s)[i] == ':'){
                    // weird hack to properly parse instructions directly in front of labels;
                    (*s)[i] = '\t';
                    i -= 1;
                    cur += ':';
                }
                result.push_back(cur);
                cur = "";
            }
            continue;
        }
        else if((*s)[i] == ';'){
            //if cur == "" then the line was a comment
            result.push_back(cur);
            cur = "";
            while((*s)[i] != '\n'){
                i++;
            }
            i++;
        }
        cur += (*s)[i];
    }

    if(cur.size() != 0){
        result.push_back(cur);
    }
    return result;
}