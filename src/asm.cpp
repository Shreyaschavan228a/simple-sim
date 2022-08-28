#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <utility>
#include <sstream>
using namespace std;

void read_file(string filepath);
void parse_labels(string line, int line_number);
vector<string> clean_file(string *s);
vector<string> split_file(string *s);

unordered_map<string, int> labels; 
bool debug_mode = false;

int main(int argc, char** argv) {
    int32_t a,b;
    int32_t pc;
    int32_t sp;
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

    // print parsed labels
    if(debug_mode){
        cout << "\nParsed labels:\n";
        unordered_map<string, int> :: iterator itr;
        for(itr = labels.begin(); itr != labels.end(); itr++){
            cout << itr->first << " " << itr->second << endl;
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
        lines = split_file(&file_contents);


        if(debug_mode){
            cout << "File contents:\n";
            cout << file_contents << "\n";
        }
        
        for(size_t i = 0; i < lines.size(); i++){
            if(debug_mode){
                cout << i << " " << lines[i] << "\n";
            }
            parse_labels(lines[i], i);
        }
    }
    else{
        cerr << "Failed to open file" << filepath << endl;
    }
    asmFile.close();
}



void parse_labels(string line, int line_number){
    string label = "";
    bool started = false;
    bool isLabel = false;
    for(char& c : line){
        if(c == ' ' && started == false){
            continue;
        }
        else if ((c == ' ' && started == true) || c == ':'){
            if(c == ':'){
                isLabel = true;
            }
            break;
        }
        else if((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')){
            if(c >= '0' && c <= '9' && !started){
                cerr << "Invalid label at line" << line_number;
                exit(0);
                break;
            }
            started = true;
            label += c;
        }
        else if(c == ';'){
            break;
        }
        else{
            break;
        }
    }
    if(label != "" && isLabel){
        labels.insert(make_pair(label, line_number));
    }
}

//splits the file into lines without removing comments
vector<string> split_file(string *s){
    vector<string> result;
    string cur = "";
    for(size_t i = 0; i < (*s).size(); i++){
        if((*s)[i] == ' ' && cur == ""){
            continue;
        }
        else if((*s)[i] == '\n'){
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
        else if((*s)[i] == '\n'){
            if(cur != ""){
                result.push_back(cur);
                cur = "";
            }
            continue;
        }
        else if((*s)[i] == ';'){
            if(cur != ""){
                result.push_back(cur);
            }
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