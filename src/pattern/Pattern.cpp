#include "Pattern.hpp"

using namespace std;


vector<string> Pattern::getVariables(){
    vector<string> vars;

    for(const auto& variable: this->variables.variables){
        vars.push_back(get<0>(variable));
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
    Tuple member;
    
    sort(validFiles.begin(), validFiles.end(), [&groupBy = as_const(groupBy)](Tuple& p1, Tuple& p2){
        return get<0>(p1)[groupBy] < get<0>(p2)[groupBy];
    });

    Types currentValue = get<0>(validFiles[0])[groupBy];
    vector<Tuple> emptyVec;
    int i = 0;
    int group_ptr = 0;

    while(i < this->validFiles.size()){
        this->validGroupedFiles.push_back(emptyVec);
        while(std::get<0>(this->validFiles[i])[groupBy] == currentValue) {
            this->validGroupedFiles[group_ptr].push_back(this->validFiles[i]);

            sort(validGroupedFiles[group_ptr].begin(), validGroupedFiles[group_ptr].end(), [](Tuple& m1, Tuple& m2){
                return get<0>(m1)["file"] < get<0>(m2)["file"];
            });

            ++i;
            if (i >= this->validFiles.size()) break;
        }

        if (i < this->validFiles.size()) currentValue = get<0>(this->validFiles[i])[groupBy];
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

Map Pattern::matchFilesLoop(Map& mapping, const string& file, const regex& patternRegex, vector<string>& parsedRegex){
    int i = 0;
    string pattern;
    string s = "";
    string temp = "";
    regex expr;
    for(int j = 0; j < variables.variables.size(); ++j){
        i += variables.getDistance(j);
        pattern = variables.getStringRegex(j);
        temp = "";
        if(pattern == "[0-9]+" || pattern == "[a-zA-Z]+"){
            expr = variables.getRegex(j);
            s.push_back(file[i]); // char -> string
            while(regex_match(s, expr)) {
                temp += file[i];
                i++;
                s = "";
                s.push_back(file[i]);
            }

            s = "";
        } else {
            //parsedRegex = variables.parseRegex(j);
            for(int k = 0; k < variables.length(j); ++k){
                temp += file[i];
                i++;
            }
        }

        if(s::is_number(temp)){
            mapping[variables.getVariable(j)] = stoi(temp);
        } else {
            mapping[variables.getVariable(j)] = temp;
        }
    }

    return mapping;
}

vector<Tuple> Pattern::getMatching(string& variables){

    //remove spaces if present
    variables.erase(std::remove_if(variables.begin(), variables.end(), ::isspace), variables.end());
    
    //split on commas
    vector<string> splitVaraibles = split(variables, ",");
    vector<std::pair<string,string>> variableValues;

    std::pair<string, string> pair;
    size_t position;
    for(const auto& variable: splitVaraibles) {
        position = variable.find("=");
        pair.first = variable.substr(0, position);
        pair.second = variable.substr(position+1);
        if(!get<0>(validFiles[0]).count(pair.first)) throw invalid_argument("Variable must be in file pattern.");
        variableValues.push_back(pair);
    }

    //vector<Tuple<string, int> variablesVec;
    vector<Tuple> matching;

    bool match;
    Types temp;
    for(auto& file: this->validFiles){
        match = true;
        for(const auto& variable: variableValues) {
            temp = get<0>(file)[variable.first];
            if(!(s::to_string(temp) == variable.second)) match = false; 
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
    Types temp;
    for(const auto & fileVector : this->validGroupedFiles) {
        cout << "[";
        for(const auto & file : fileVector) {
            cout << "{";
            for(auto it = std::get<0>(file).cbegin(); it != std::get<0>(file).cend(); ++it) {
                temp = it->second;
                std::cout << it->first << ", " << s::to_string(temp) << "; ";
            }
            cout << "}" << endl;
        }
        cout << "]" << endl;
    }
}


std::vector<Tuple> Pattern::getValidFiles(){
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
            for(const auto& file: std::get<1>(pair)){
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
        vec.push_back(std::get<1>(element));
    }

    return vec;
}