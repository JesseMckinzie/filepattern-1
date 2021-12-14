#include "Pattern.hpp"

using namespace std;


vector<string> Pattern::getVariables(){
    vector<string> vars;

    for(const auto& variable: this->variables.variables){
        vars.push_back(variable.first);
    }
    
    return vars;
}

void Pattern::filePatternToRegex(){

    this->regexFilePattern = ""; // Store the regex conversion of the filePattern

    string str = ""; // store the name associated with numeric or character values (i.e. row, column, channel, etc.)
    bool inBrackets = false; // currently in {} brackets when iterating over file pattern
    bool afterColon = false; // passed colon in brackets, i.e. after ":"" in {row:dd}

    // Create hashtable to map a pattern to corresponding regex 
    map<char, string> patternMap;
    patternMap['d'] = "[0-9]"; 
    patternMap['c'] = "[a-zA-Z]";
    patternMap['+'] = "+";

    string error;
    int stringLength = 0;
    string regexPattern = "";
    for(const auto & c : this->filePattern){

        if (c == '{'){
            if(inBrackets) {
                throw invalid_argument("Missing closing bracket in filepattern.");
            }
            inBrackets = true;
        } else if (c == '}'){
            if(!inBrackets) {
                throw invalid_argument("Missing opening bracket in filepattern.");
            }
            if(!afterColon){
                throw invalid_argument("Missing variable name in file pattern. Format must be {variable_name:variable}");
            }
            inBrackets = false;
            afterColon = false;
            variables.addNode(str, regexPattern, stringLength);

            regexPattern = "";
            str = "";
            stringLength = 0;

        } else if (c == ':') {

            afterColon = true;
            //variables[str];

        } else if (inBrackets) { 

            if(afterColon) {
                // check if the character is a valid key (c, d, or +)
                if(patternMap.find(c) == patternMap.end()){
                    error = "Invalid pattern \"";
                    error.push_back(c);
                    error += "\" found in filepattern. Patterns must be \"d\", \"c\", or \"+\".";
                    throw invalid_argument(error);
                }
                regexFilePattern += patternMap[c]; // Add corresponding regex 
                regexPattern += patternMap[c];
            } else str += c;
            

        } else { // Add character without modifications if not in brackets

            if(isalpha(c) || isdigit(c) || c == '.' || c == '-' || c == '_'){
                regexFilePattern += c;
                ++stringLength;
            } else {
                error = "Invalid character \"";
                error.push_back(c);
                error += "\" in filepattern.";
                throw std::invalid_argument(error);
            }
        } 
    }
    
    if (inBrackets) {
        cout << "Missing closing bracket in file pattern" << endl;
        exit(1);
    }

}

void Pattern::groupBy(const string& groupBy) {

    validGroupedFiles.clear();

    /*
    std::vector<std::p::pair<std::string, std::p::pair<std::string, int>>> var = variables.variables;
    string groupBy;
    for(const auto& v: var){
        if(v.first != groupBy) groupBy = v.first;
    }
    */p::pair<map<string, string>, vector<string>> member;
    
    sort(validFiles.begin(), validFiles.end(), [&groupBy = as_const(groupBy)](p::pair<map<string, string>, vector<string>>& p1, p::pair<map<string, string>, vector<string>>& p2){
        return p1.first[groupBy] < p2.first[groupBy];
    });

    string currentValue = validFiles[0].first[groupBy];
    vector<p::pair<map<string, string>, vector<string>>> emptyVec;
    int i = 0;
    int group_ptr = 0;

    while(i < this->validFiles.size()){
        this->validGroupedFiles.push_back(emptyVec);
        while(this->validFiles[i].first[groupBy] == currentValue) {
            this->validGroupedFiles[group_ptr].push_back(this->validFiles[i]);

            sort(validGroupedFiles[group_ptr].begin(), validGroupedFiles[group_ptr].end(), [](p::pair<map<string, string>, vector<string>>& m1, p::pair<map<string, string>, vector<string>>& m2){
                return m1.first["file"] < m2.first["file"];
            });

            ++i;
            if (i >= this->validFiles.size()) break;
        }

        if (i < this->validFiles.size()) currentValue = this->validFiles[i].first[groupBy];
        ++group_ptr;
    }
}

vector<string> Pattern::split (string& s, const string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

map<string, string> Pattern::matchFilesLoop(map<string, string>& mapping, const string& file, const regex& patternRegex, vector<string>& parsedRegex){
    int i = 0;
    string pattern;
    string s = "";
    for(int j = 0; j < variables.variables.size(); ++j){
        i += variables.getDistance(j);
        pattern = variables.getRegex(j);
        if(pattern == "[0-9]+" || pattern == "[a-zA-Z]+"){
            s.push_back(file[i]); // char -> string
            while(regex_match(s, regex(pattern))) {
                mapping[variables.getVariable(j)] += file[i];
                i++;
                s = "";
                s.push_back(file[i]);
            }
            s = "";
        } else {
            parsedRegex = variables.parseRegex(j);
            for(const auto& expr: parsedRegex){
                mapping[variables.getVariable(j)] += file[i];
                i++;
            }
        }
    }
    return mapping;
}

vector<p::pair<map<string, string>, vector<std::string>>> Pattern::getMatching(string variables){

    //remove spaces if present
    variables.erase(std::remove_if(variables.begin(), variables.end(), ::isspace), variables.end());
    
    //split on commas
    vector<string> splitVaraibles = split(variables, ",");
    vector<p::pair<string,string>> variableValues;

    p::pair<string, string> pair;
    size_t position;
    for(const auto& variable: splitVaraibles) {
        position = variable.find("=");
        pair.first = variable.substr(0, position);
        pair.second = variable.substr(position+1);
        if(!validFiles[0].first.count(pair.first)) throw invalid_argument("Variable must be in file pattern.");
        variableValues.push_back(pair);
    }

    //vector<p::pair<string, int> variablesVec;
    vector<p::pair<map<string, string>, vector<string>>> matching;

    
    bool match;

    for(auto& file: this->validFiles){
        match = true;
        for(const auto& variable: variableValues) {
            if(!(file.first[variable.first] == variable.second)) match = false; 
        }
        if(match) matching.push_back(file);
    }

    return matching;
    
}


void Pattern::printValidFiles() {
    // If no valid files
    if(validFiles.size() == 0) {
        cout << "No valid files." << endl; 
        return;
    }

    for(const auto & fileVector : this->validGroupedFiles) {
        cout << "[";
        for(const auto & file : fileVector) {
            cout << "{";
            for(auto it = file.first.cbegin(); it != file.first.cend(); ++it) {
                std::cout << it->first << ", " << it->second << "; ";
            }
            cout << "}" << endl;
        }
        cout << "]" << endl;
    }
}


std::vector<p::pair<std::map<std::string, std::string>, std::vector<std::string>>> Pattern::getValidFiles(){
    return this->validFiles;
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

void Pattern::printFilePaths(){
    for(const auto& vec: validGroupedFiles){
        for(const auto& pair: vec){
            for(const auto& file: pair.second){
                cout << file << ", " << endl;
            }
            cout << endl;
        }
    }
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

vector<vector<string>> Pattern::getValidFilePaths(){
    vector<vector<string>> vec;
    for (const auto& element: this->validFiles){
        vec.push_back(element.second);
    }

    return vec;
}