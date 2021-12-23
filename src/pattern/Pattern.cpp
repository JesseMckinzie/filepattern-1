#include "Pattern.hpp"

using namespace std;

vector<string> Pattern::getVariables(){
    return this->variables;
}

void Pattern::filePatternToRegex(){

    std::regex e("\\{(\\w+):([dc+]+)\\}");

    map<char, string> patternMap;
    patternMap['d'] = "[0-9]"; 
    patternMap['c'] = "[a-zA-Z]";
    patternMap['+'] = "+";
    
    string str;
    string rgx;
    vector<pair<string,string>> matches;
    vector<string> variables;
    string patternCopy = this->filePattern;
    std::smatch m;

    while (regex_search(patternCopy, m, e)){
        str = m[2];
        rgx = "";
        for(const auto c: str){
            rgx += patternMap[c];
        }

        this->variables.push_back(m[1]);
        matches.push_back(make_pair(m[0], rgx));

        patternCopy = m.suffix().str(); 
    }

    this->regexFilePattern = filePattern;

    for(const auto& match: matches){

        // Create capture group
        str = "(";
        str += match.second;
        str += ")";

        s::replace(this->regexFilePattern, match.first, str);
    }
}


Tuple Pattern::getVariableMapMultDir(const string& filePath, const smatch& sm){
    Tuple tup;
    
    bool matched = false;
    string temp;
    string file = s::getBaseName(filePath);
    for(int i = 0; i < validFiles.size(); i++){ 
        //temp = get<1>(validFiles[0])["file"]; // to be removed
        temp = s::getBaseName(s::to_string(get<1>(validFiles[i])[0]));
        // val = s::to_string(temp); to be removed
        if(temp == file){
            matched = true;
            get<1>(validFiles[i]).push_back(filePath);
            break;
        } 
    }

    if(!matched){
        tup = getVariableMap(filePath, sm);
    }


    return tup;
}

Tuple Pattern::getVariableMap(const string& filePath, const smatch& sm){
    
    Tuple tup;
    // filename matches the pattern
    std::get<1>(tup).push_back(filePath);

    string str;
    for(int i = 1; i < sm.size(); ++i){
        str = sm[i];
        s::is_number(str) ? get<0>(tup)[variables[i-1]] = stoi(str) : 
                            get<0>(tup)[variables[i-1]] = str;
    }
    
    return tup;
}

string Pattern::getPattern(){
    return this->filePattern;
}

void Pattern::setPattern(const string& pattern){
    this->filePattern = pattern;
}

string Pattern::getRegexPattern(){
    return this->regexFilePattern;
}


void Pattern::printVariables(){
    int i = 0;
    cout << "The variables are: ";
    int size = this->getVariables().size();
    for(const auto& var: this->getVariables()){
        cout << var;
        if(i<size-1){
            cout << ", ";
        }
        i++;
    }
    cout << endl;
}