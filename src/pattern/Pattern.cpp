#include "Pattern.hpp"

using namespace std;

vector<string> Pattern::getVariables(){
    return this->variables;
}

void Pattern::filePatternToRegex(){

    // regex to match variables
    std::regex e("\\{(\\w+):([dc+]+)\\}");

    map<char, string> patternMap; // map of variable types to regex equivalent 
    patternMap['d'] = "[0-9]"; 
    patternMap['c'] = "[a-zA-Z]";
    patternMap['+'] = "+";
    
    string str; // temp string
    string rgx; // temp regex
    vector<pair<string,string>> matches; // map between bracket expression and regex
    vector<string> variables; // store variable names
    string patternCopy = this->filePattern; // get a copy of pattern since regex_search is inplace
    std::smatch m; // regex matches

    // extract bracket expressions from pattern and store regex
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

    // Replace bracket groups with regex capture groups
    for(const auto& match: matches){
        // Create capture group
        str = "(" + match.second + ")";

        s::replace(this->regexFilePattern, match.first, str);
    }
}


Tuple Pattern::getVariableMapMultDir(const string& filePath, const smatch& sm){
    Tuple tup;
    
    bool matched = false;
    string basename;
    string file = s::getBaseName(filePath);
    for(int i = 0; i < validFiles.size(); i++){ 
        basename = s::getBaseName(s::to_string(get<1>(validFiles[i])[0])); // store the basename
        if(basename == file){
            matched = true;
            get<1>(validFiles[i]).push_back(filePath); // Add path to existing mapping
            break;
        } 
    }

    // Create new mapping if not already present
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
    // Extract capture groups from filename and store in mapping
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