#include <bits/stdc++.h>

using std::vector;
using std::unordered_map;
using std::string;
using std::bitset;

class binary {
public:
    binary() = default;
    binary (const string& str) {
        bool neg = 0;
        string str2;
        if (str.at(0) == '#'){
            if (str.at(1) == '-') neg = 1;
            int tmp = atoi(string(str.c_str() + 1 + neg).c_str());
            while (tmp){
                str2 = (tmp % 2 ? "1" : "0") + str2;
                tmp >>= 1;
            }
        }else{
            if (str.at(1) == '-') neg = 1;
            size_t i = 1 + neg;
            while (i < str.size()){
                switch (str.at(i++)){
                case '0': str2 = str2 + "0000"; break;
                case '1': str2 = str2 + "0001"; break;
                case '2': str2 = str2 + "0010"; break;
                case '3': str2 = str2 + "0011"; break;
                case '4': str2 = str2 + "0100"; break;
                case '5': str2 = str2 + "0101"; break;
                case '6': str2 = str2 + "0110"; break;
                case '7': str2 = str2 + "0111"; break;
                case '8': str2 = str2 + "1000"; break;
                case '9': str2 = str2 + "1001"; break;
                case 'A': str2 = str2 + "1010"; break;
                case 'B': str2 = str2 + "1011"; break;
                case 'C': str2 = str2 + "1100"; break;
                case 'D': str2 = str2 + "1101"; break;
                case 'E': str2 = str2 + "1110"; break;
                case 'F': str2 = str2 + "1111"; break;
                }
            }
        }
        while (str2.size() < 16) str2 = "0" + str2;
        if (neg) {
            auto t = (bitset<16>(str2).flip()).to_ullong() + 1ull;
            bin = bitset<16>(t);
        }else {
            bin = bitset<16>(str2);
        }
    }
    const string getBit() const {return bin.to_string();}
    const auto to_ullong() const {return bin.to_ullong(); }
    
private:
    bitset<16> bin;
};

//pseudos 
const unordered_map<string, int> pseudos({
    {".ORIG", 1},
    {".END", 2}, 
    {".STRINGZ", 3}, 
    {".FILL", 4},
    {".BLKW", 5}
});

//commands
const unordered_map<string, int> commands({
    {"ADD", 1},
    {"AND", 2},
    {"JMP", 3},
    {"JSR", 4},
    {"LD", 5},
    {"LDI", 6},
    {"LDR", 7},
    {"LEA", 8},
    {"NOT", 9},
    {"RET", 10},
    {"RTI", 11},
    {"ST", 12},
    {"STI", 13},
    {"STR", 14},
    {"TRAP", 15},
    {"BR", 16},
    {"BRN", 17},
    {"BRZ", 18},
    {"BRP", 19},
    {"BRNZ", 20},
    {"BRNP", 21},
    {"BRZP", 22},
    {"BRNZP", 23}
});


void assemble(const string&, const string&);

bool isCmd(const string&);
string imm_2(const string&);
int imm_10(const string&);

int main (int argc, char** argv){
    string input_file(argv[1]);
    string output_file(argv[2]);

    assemble(input_file, output_file);

    std::cout << "\nassemble successfully!" << std::endl;

    return 0;
}

void firstPass(const string&, unordered_map<string, int>&);
void secondPass(const string&, const string&, unordered_map<string, int>&);
//assemble function
void assemble(const string& in, const string& out){
    unordered_map<string, int> table;
    firstPass(in, table);

    secondPass(in, out, table);
}

bool isCmd(const string&);

void firstPass(const string& in , unordered_map<string, int>& table){
    std::ifstream ifs(in);
    string tmp, cur_sec;
    std::stringstream ss;
    std::getline(ifs, tmp);
    ss << tmp;
    ss >> cur_sec;
    ss >> cur_sec;
    //.ORIG ????
    auto LC = static_cast<int>(binary(cur_sec).to_ullong());
    while ( std::getline(ifs, tmp) ) {
        //每行一开始都是(伪)操作，利用stringstream分割空格

        ss.str("");
        ss.clear();

        ss << tmp;
        ss >> cur_sec;
        if (!isCmd(cur_sec)){
            string instr;
            ss >> instr;
            table[cur_sec] = LC;
            if (instr == ".STRINGZ"){
                string t;
                ss >> t;
                LC += (t.length() - 2);
            }else if (instr == ".BLKW") {
                string t;
                ss >> t;
                int addition = binary(t).to_ullong() - 1ull;
                LC += addition;
            }
        }
        ++LC;
    }
}

inline bool isCmd(const string& str){
    return pseudos.find(str) != pseudos.cend() || commands.find(str) != commands.cend(); 
}

vector<string> TransPseudos(const string&,int& LC, const unordered_map<string,int>&);
string TransCommands(const string&, int LC,const unordered_map<string,int>&);

void secondPass(const string& in, const string& out, unordered_map<string, int>& table){
    std::ifstream ifs(in);
    std::ofstream ofs(out);

    string tmp, cur_sec;
    std::stringstream ss;
    std::getline(ifs, tmp);
    ss << tmp;
    ss >> cur_sec;
    ss >> cur_sec;
    //.ORIG ????
    auto start = binary(cur_sec);
    ofs << start.getBit() << "\n";
    int LC = start.to_ullong();

    while ( std::getline(ifs, tmp) ) {

        ss.str("");
        ss.clear();

        ss << tmp;
        ss >> cur_sec;

        if (table.find(cur_sec) != table.cend()) {
            ss >> cur_sec;
        }
        if (pseudos.find(cur_sec) != table.cend()) {
            auto a = TransPseudos(tmp, LC, table); //operate LC inside due to .stringz and .blkw
            for (auto& t: a) ofs << t << "\n";
        }else{
            ofs << TransCommands(tmp, LC, table) << "\n";
            ++LC;
        }
    }
}

string Ascii(char c);

vector<string> TransPseudos(const string& str, int& LC, const unordered_map<string,int>& table) {
    std::stringstream ss;
    string cur_sec;
    vector<string> ret;
    ss << str;
    ss >> cur_sec;

    if (table.find(cur_sec) != table.cend()) {
        ss >> cur_sec;
    }

    switch (pseudos.at(cur_sec)) {
        case 1:
            std::cout << "double .ORIG";
            exit(1);
            break;
        case 2:
            break;
        case 3: //STRINGZ
            ss >> cur_sec;
            for (int i = 1; i < cur_sec.size(); ++i){
                if (cur_sec[i] != '"') {
                    ret.push_back(Ascii(cur_sec[i]));
                    ++LC;
                }else break;
            }
            ret.push_back("0000000000000000");
            break;
        case 4: //FILL
            ss >> cur_sec;
            ret.push_back(binary(cur_sec).getBit());
            ++LC;
            break;
        case 5: //BLKW
            ss >> cur_sec;
            int num = binary(cur_sec).to_ullong();
            LC += num;
            for (int i = 0; i < num; ++i){
                ret.push_back("0000000000000000");
            }
            break;
    }
    return ret;
}

string Ascii(char c){
    int i = c & 0x7f;
    string ret;
    while (i) {
        ret = (i % 2 ? "1" : "0") + ret; 
        i >>= 1;
    }
    while (ret.size() < 16) {
        ret = "0" + ret;
    }
    return ret;
}

inline string reg(char c);

string TransCommands(const string& str,int LC ,const unordered_map<string,int>& table) {
    std::stringstream ss;
    string cur_sec, ret;
    ss << str;
    ss >> cur_sec;

    if (table.find(cur_sec) != table.cend()) {
        ss >> cur_sec;
    }
    switch (commands.at(cur_sec)) {
        case 1: //ADD
            ret = "0001";
            //DR
            ss >> cur_sec;
            ret = ret + reg(cur_sec[1]);
            //SR1
            ss >> cur_sec;
            ret = ret + reg(cur_sec[1]);
            //SR2 or IMM2
            ss >> cur_sec;
            if (cur_sec[0] == 'R'){
                ret = ret + "000" + reg(cur_sec[1]);
            }else {
                ret = ret + "1" + binary(cur_sec).getBit().substr(11,5);
            }
            break;

        case 2: //AND
            ret = "0101";
            //DR
            ss >> cur_sec;
            ret = ret + reg(cur_sec[1]);
            //SR1
            ss >> cur_sec;
            ret = ret + reg(cur_sec[1]);
            //SR2 or IMM2
            ss >> cur_sec;
            if (cur_sec[0] == 'R'){
                ret = ret + "000" + reg(cur_sec[1]);
            }else {
                ret = ret + "1" + binary(cur_sec).getBit().substr(11,5);
            }
            break;

        case 3: //JMP
            ss >> cur_sec;
            ret = "1100000" + reg(cur_sec[1]) + "000000";
            break;

        case 4: //JSR
            ss >> cur_sec;
            ret = "01001";

            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<11>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(5,11);
            }
            break;

        case 5: //LD
            ss >> cur_sec;
            ret = "0010" + reg(cur_sec[1]);
            ss >> cur_sec;

            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }

            break;
        
        case 6: //LDI
            ss >> cur_sec;
            ret = "1010" + reg(cur_sec[1]);

            ss >> cur_sec;

            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }

            break;

        case 7: //LDR
            ss >> cur_sec;
            ret = "0110" + reg(cur_sec[1]);
            ss >> cur_sec;
            ret = ret + reg(cur_sec[1]); // SR
            ss >> cur_sec;

            ret = ret + binary(cur_sec).getBit().substr(10,6); // offset6
            break;

        case 8: //LEA
            ss >> cur_sec;
            ret = "1110" + reg(cur_sec[1]);
            ss >> cur_sec;

            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            
            break;
        case 9: //NOT
            ss >> cur_sec;
            ret = "1001" + reg(cur_sec[1]);
            ss >> cur_sec;
            ret = ret + reg(cur_sec[1]) + "111111"; //SR
            
            break;

        case 10: //RET
            ret = "1100000111000000";
            break;
        case 11: //RTI
            ret = "1000000000000000";
            break;
        case 12: //ST
            ss >> cur_sec;
            ret = "0011" + reg(cur_sec[1]);

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            
            break;
        case 13: //STI
            ss >> cur_sec;
            ret = "1011" + reg(cur_sec[1]);

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }

            break;

        case 14: //STR
            ss >> cur_sec;
            ret = "0111" + reg(cur_sec[1]);
            ss >> cur_sec;
            ret = ret + reg(cur_sec[1]);

            ss >> cur_sec;
            ret = ret + binary(cur_sec).getBit().substr(10,6); 

            break;

        case 15: //TRAP
            ss >> cur_sec;
            ret = "11110000" + binary(cur_sec).getBit().substr(8,8);
            break;
        case 16: // BR
            ss >> cur_sec;
            ret = "0000111";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;

        case 17: // BRN
            ss >> cur_sec;
            ret = "0000100";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;
        case 18: // BRZ
            ss >> cur_sec;
            ret = "0000010";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;
        case 19: // BRP
            ss >> cur_sec;
            ret = "0000001";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;
        case 20: // BRNZ
            ss >> cur_sec;
            ret = "0000110";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;
        case 21: // BRNP
            ss >> cur_sec;
            ret = "0000101";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;
        case 22: // BRZP
            ss >> cur_sec;
            ret = "0000011";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;
        case 23: // BRNZP
            ss >> cur_sec;
            ret = "0000111";

            ss >> cur_sec;
            if (table.find(cur_sec) != table.cend()) { //LABEL
                ret = ret + bitset<9>(table.at(cur_sec) - LC - 1).to_string();
            }else {
                ret = ret + binary(cur_sec).getBit().substr(7,9);
            }
            break;
    }
    return ret;
}

inline string reg(char c){
    return std::bitset<3>(c).to_string();
}
