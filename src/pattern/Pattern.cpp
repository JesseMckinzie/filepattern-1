#include "Pattern.hpp"

using namespace std;

void Pattern::setGroup(const string& group){
    if(find(this->variables.begin(), this->variables.end(), group) != variables.end()) {
        this->group = group;
    } else {
        throw invalid_argument("Group by variable must be contained in the pattern.");
    }
}

vector<string> Pattern::getVariables(){
    return this->variables;
}

void Pattern::filePatternToRegex(){
    if(this->filePattern.find(':') == std::string::npos){
        getNewNaming(this->filePattern);
    }

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
        this->variableOccurences[variables[i-1]][get<0>(tup)[variables[i-1]]] += 1;
        this->uniqueValues[variables[i-1]].insert(get<0>(tup)[variables[i-1]]);
    }
    
    return tup;
}

/*
std::map<string, std::map<Types, int>> Pattern::getOccurences(){
    return this->variableOccurences;
}
*/



//(variableName, variableValue)
std::map<string, std::map<Types, int>> Pattern::getOccurences(const vector<tuple<string, vector<Types>>>& mapping){
    if(mapping.size() == 0){
        return this->variableOccurences;
    }

    std::map<Types, int> temp;
    std::map<string, std::map<Types, int>> occurences;
    string variable;
    for(const auto& tup: mapping){
        if(get<1>(tup).size() == 0){
            occurences[get<0>(tup)] = this->variableOccurences[get<0>(tup)];
        } else {
            for(const auto& value: get<1>(tup)){
                variable = get<0>(tup);
                //if(s::is_number(this->variableOccurences[variable][value]){
                //    value.erase(0, min(value.find_first_not_of('0'), value.size()-1));
                //}

                temp[value] = this->variableOccurences[get<0>(tup)][value];
            }
            occurences[variable] = temp;
        }
    }

    return occurences;

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

map<string, set<Types>> Pattern::getUniqueValues(const vector<string>& vec){
    if(vec.size() == 0) return this->uniqueValues;

    map<string, set<Types>> temp;
    for(const auto& str: vec){
        temp[str] = uniqueValues[str];
    }
    return temp;
}


void Pattern::getNewNaming(string& pattern){
    cout << "WARNING: The old style of pattern was used. This style may become deprecated in future releases." << endl;


    string vars = "\\{(["; // initialize begining of group
    vars += "rtczyxp"; // add in valid variables
    vars += "+]+)\\}"; // add in last part of group

    std::regex e(vars);

    string str; // temp string
    vector<pair<string,string>> matches; // map between bracket expression and regex
    string patternCopy = pattern; // get a copy of pattern since regex_search is inplace
    std::smatch m; // regex matches

    // extract bracket expressions from pattern and store regex
    while (regex_search(patternCopy, m, e)){
        str = m[1];

        matches.push_back(make_pair(m[0], m[1]));
    
        patternCopy = m.suffix().str(); 
        
    }


    // Replace bracket groups with regex capture groups
    for(const auto& match: matches){
        // Create capture group

        str = "{";
        str += match.second[0]; 
        str += ":";
        for(const auto& c: match.second){
           if(c != '+') str += "d";
           else str += "+";
        }
        str += "}";

        s::replace(pattern, match.first, str);
    }
    cout << "The recommended pattern to use is: " << pattern << 
            ". See the documenation for details about the new style." << endl;
}