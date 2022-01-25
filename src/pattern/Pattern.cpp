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
    
    getNewNaming(this->filePattern);

    // regex to match variables
    std::regex e("(\\{(\\w+):([dc+]+)\\})|(\\(P\\?<(\\w+)>(.+)\\))"); // check for bracket expressions or named groups
    std::regex group("\\P\\?<(\\w+)>(.+)");
    std::regex var("\\{(\\w+):([dc+]+)\\}");

    map<char, string> patternMap; // map of variable types to regex equivalent 
    patternMap['d'] = "[0-9]"; 
    patternMap['c'] = "[a-zA-Z]";
    patternMap['+'] = "+";
    
    string str, rgx; // temp string and regex
    vector<pair<string,string>> matches; // map between bracket expression and regex
    vector<string> variables; // store variable names
    string patternCopy = this->filePattern; // get a copy of pattern since regex_search is inplace
    std::smatch sm, m; // regex matches

    string temp;
    // extract bracket expressions from pattern and store regex
    while (regex_search(patternCopy, m, e)){
        temp = m[0];
        if(temp.rfind("(P?<", 0) == 0) {
           while (regex_search(temp, sm, group)){
                rgx = sm[2];
                rgx.pop_back();

                this->variables.push_back(sm[1]);
                str = sm[0];
                str = "(" + str;
                matches.push_back(make_pair(str, rgx));
                temp = sm.suffix().str();
           }
        } else {
            while (regex_search(temp, sm, var)){
                str = sm[2];
                rgx = "";
                for(const auto c: str){
                    rgx += patternMap[c];
                }
                this->variables.push_back(sm[1]);
                matches.push_back(make_pair(sm[0], rgx));
                temp = sm.suffix().str();
            }
        }


        patternCopy = m.suffix().str(); 
    }

    this->regexFilePattern = filePattern;

    // Replace bracket groups with regex capture groups
    for(const auto& match: matches){
        this->namedGroups.push_back(match.first);

        // Create capture group
        str = "(" + match.second + ")";
        s::replace(this->regexFilePattern, match.first, str);
    }
}

tuple<string, vector<string>, vector<string>> Pattern::getRegex(string& pattern){
    
    getNewNaming(pattern);

    // regex to match variables
    std::regex e("(\\{(\\w+):([dc+]+)\\})|(\\(P\\?<(\\w+)>(.+)\\))"); // check for bracket expressions or named groups
    std::regex group("\\P\\?<(\\w+)>(.+)");
    std::regex var("\\{(\\w+):([dc+]+)\\}");

    map<char, string> patternMap; // map of variable types to regex equivalent 
    patternMap['d'] = "[0-9]"; 
    patternMap['c'] = "[a-zA-Z]";
    patternMap['+'] = "+";
    
    string str, rgx; // temp string and regex
    vector<pair<string,string>> matches; // map between bracket expression and regex
    vector<string> variables; // store variable names
    string patternCopy = pattern; // get a copy of pattern since regex_search is inplace
    std::smatch sm, m; // regex matches

    string temp;
    // extract bracket expressions from pattern and store regex
    while (regex_search(patternCopy, m, e)){
        temp = m[0];
        if(temp.rfind("(P?<", 0) == 0) {
           while (regex_search(temp, sm, group)){
                rgx = sm[2];
                rgx.pop_back();

                variables.push_back(sm[1]);
                str = sm[0];
                str = "(" + str;
                matches.push_back(make_pair(str, rgx));
                temp = sm.suffix().str();
           }
        } else {
            while (regex_search(temp, sm, var)){
                str = sm[2];
                rgx = "";
                for(const auto c: str){
                    rgx += patternMap[c];
                }
                variables.push_back(sm[1]);
                matches.push_back(make_pair(sm[0], rgx));
                temp = sm.suffix().str();
            }
        }


        patternCopy = m.suffix().str(); 
    }

    string regexFilePattern = pattern;
    vector<string> namedGroups;

    // Replace bracket groups with regex capture groups
    for(const auto& match: matches){
        namedGroups.push_back(match.first);

        // Create capture group
        str = "(" + match.second + ")";
        s::replace(regexFilePattern, match.first, str);
    }

    return make_tuple(regexFilePattern, variables, namedGroups);
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
    
    string vars = "\\{([rtczyxp+]+)\\}"; // check for old naming style or named grouped

    std::regex e(vars);

    string str; // temp string
    vector<pair<string,string>> matches; // map between bracket expression and regex
    string patternCopy = pattern; // get a copy of pattern since regex_search is inplace
    std::smatch m; // regex matches
    bool replaced = false;
    // extract bracket expressions from pattern and store regex
    while (regex_search(patternCopy, m, e)){
        str = m[1];

        matches.push_back(make_pair(m[0], m[1]));

        //regexMatches.push_back(m);
        patternCopy = m.suffix().str(); 
        
    }

    // Replace bracket groups with regex capture groups
    for(const auto& match: matches){
        // Create capture group

        if(match.first.find(':') == std::string::npos) {

            replaced = true;
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
    }
    if(replaced){
        cout << "WARNING: The old style of pattern was used. This style may become deprecated in future releases." << endl;
        cout << "The recommended pattern to use is: " << pattern << 
                ". See the documenation for details about the new style." << endl;
    }
}

void Pattern::replaceOutputName(Tuple& min, Tuple& max, const string& var, string& outputName, const int idx, string& temp, const regex& patternRegex){
        string file;
        smatch sm;
        if(get<0>(min)[var] == get<0>(max)[var]){

            file = s::getBaseName(get<1>(min)[0]);
            regex_match(file, sm, patternRegex);
        
            s::replace(outputName, this->namedGroups[idx], sm[idx+1]);

        } else { // if min is different than max, put range in outputname

            temp = "("; 

            file = s::getBaseName(get<1>(min)[0]); // get basename of filepath
            regex_match(file, sm, patternRegex); // find variables

            temp += sm[idx+1];
            temp += "-";

            file = s::getBaseName(get<1>(max)[0]);
            regex_match(file, sm, patternRegex);

            temp += sm[idx+1];
            temp += ")";

            s::replace(outputName, this->namedGroups[idx], temp);
        }
}

string Pattern::outputNameHelper(vector<Tuple>& vec){
    if(vec.size() == 0){
        vec = this->validFiles;
    }

    string outputName = this->filePattern;

    int idx = 0;
    int min, max;
    smatch sm;
    string temp, file;
    
    regex patternRegex(this->regexFilePattern);

    for(auto& var: variables){
        
        min = m::getMinIdx(vec, var); 
        max = m::getMaxIdx(vec, var);

        // if min is the same as max, put variable value in output
        this->replaceOutputName(vec[min], vec[max], var, outputName, idx, temp, patternRegex);
        
        ++idx;
    }

    return outputName;

}

string Pattern::inferPattern(vector<string>& files, string& variables){

    variables += "rtczyxp";

    string pattern = files[0];
    string regexStr;
    string patternCpy = pattern;
    regex rgx = regex(get<0>(getRegex(patternCpy)));

    cout << "starting pattern: " << pattern << endl;
    cout << "starting regex: " << get<0>(getRegex(pattern)) << endl;

    for(auto& file : files){
        cout << "file: " << file << endl;
        if(!regex_match(file, rgx)) {
            pattern = swSearch(pattern, file, variables);
            patternCpy = pattern;
            regexStr = get<0>(getRegex(patternCpy));
            rgx = regex(regexStr);

            cout << "file: " << file << endl;
            cout << "new pattern: " << pattern << endl; 
            cout << "new regex: " << regexStr << endl;
        }
    }
    cout << "final pattern: " << endl;

    //getNewNaming(pattern);

    return pattern;

}

string Pattern::swSearch(string& pattern, string& filename, const string& variables){
    string numbers = "0123456789<>";
    string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@$";
    smatch sm;
    string rgxStr;
    regex rgx; 
    string temp, smStr;
    int cnt;
    bool match;

    //vlist = [(0,0,"", "")]
    for (const auto& v: variables){

        rgxStr = "\\{(";
        rgxStr += v;
        rgxStr += "):([dc+]+)\\}";

        rgx = regex(rgxStr);
        match = false;
        while(regex_search(pattern, sm, rgx)){
            for(const auto& match: sm){
                cout << "sm " << match << endl;
            }
            match = true;
            // check if + is in group
            smStr = sm[0];
            if(s::contains(smStr, '+')) {
                if(sm[1] == "d+") s::replace(pattern, sm[0], ">");// check this
                else s::replace(pattern, sm[0], "@");

            } else {
                temp = "";
                smStr = sm[2].str();
                if(s::contains(smStr, 'd')){
                    temp.insert(0, s::getCount(sm[0].str(), 'd'), '<');
                    s::replace(pattern, sm[0], temp);// check this
                } else {
                    temp.insert(0, s::getCount(sm[0].str(), 'c'), '$');
                    s::replace(pattern, sm[0], temp);// check this
                }
            }
        } 
        if(!match) {
            break;
        }
    }

    cout << "swSearch updated pattern: " << pattern << endl;
    cout << "swSearch filename: " << filename << endl;

    map<string, map<string, int>> sab = {
        {"numeric", {
            {"match", 2}, 
            {"penalty", 1}
        }},
        {"alpha", {
            {"match", 5}, 
            {"penalty", 3}
        }}
    };

    int m = pattern.length();
    int n = filename.length();
    int matrix[m+1][n+1];
    fill(matrix[0], matrix[0] + (m+1) * (n+1), 0);
    //fill( &matrix[0][0], &matrix[0][0] + sizeof(matrix), 0);

    int p_idx = 0;
    int f_idx = 0;
    bool pIsNumeric, fIsNumeric;
    vector<int> scores;
    int s, wi, wj;

    for(const auto& p: pattern){
        pIsNumeric = count(numbers.begin(), numbers.end(), p);

        f_idx = 0;
        for(const auto& f: filename){
            
            fIsNumeric = count(numbers.begin(), numbers.end(), f);
            
            scores.clear();
            scores.push_back(0);

            if(fIsNumeric){
                if(pIsNumeric){
                    s = matrix[p_idx][f_idx] + sab["numeric"]["match"];
                } else {
                    s = matrix[p_idx][f_idx] - sab["numeric"]["match"];
                }
            } else {
                if (p == f){
                    s = matrix[p_idx][f_idx] + sab["alpha"]["match"];
                } else {
                    s = matrix[p_idx][f_idx] - sab["alpha"]["match"];
                }
            }

            scores.push_back(s);

            if (fIsNumeric){
                if (pIsNumeric) {
                    wi = matrix[p_idx+1][f_idx] - sab["numeric"]["penalty"];
                    wj = matrix[p_idx][f_idx+1] - sab["numeric"]["penalty"];
                } else {
                    wi = matrix[p_idx+1][f_idx] - sab["alpha"]["penalty"];
                    wj = matrix[p_idx][f_idx+1] - sab["alpha"]["penalty"];
                }
            } else {
                wi = matrix[p_idx+1][f_idx] - sab["alpha"]["penalty"];
                wj = matrix[p_idx][f_idx+1] - sab["alpha"]["penalty"];
            }

            scores.push_back(wi);
            scores.push_back(wj);

            matrix[p_idx+1][f_idx+1] = *max_element(scores.begin(), scores.end());

            ++f_idx;

        }
        ++p_idx;
    }

    int bestScore = matrix[m][n];
    int row       = m;
    int col       = n;

    cout << "before nested loop" << endl;
    for(int r = 1; r < m+1; ++r){
        for(int c = 1; c < n+1; ++c){
            if(matrix[r][c] > bestScore){
                bestScore = matrix[r][c];
                row = r;
                col = c;
            }
        }
    }

    string patternTemplate = ""; //(1, filename[col-1]);
    patternTemplate.push_back(filename[col-1]);
    int lastRow = row;
    int lastCol = col;
    int r, c;

    /*
    for(const auto& vec: matrix){
        for(const auto& v: vec){
            cout << v << " ";
        }
        cout << endl;
    }
    */

    while(true){
        r = row - 1;
        c = col - 1;
        bestScore = matrix[r][c];

        if(matrix[row][col-1] > bestScore){
            bestScore = matrix[row][col-1];
            r = row;
            c = col - 1;
        }

        if(matrix[row-1][col] > bestScore){
            bestScore = matrix[row-1][col];
            r = row - 1;
            c = col;
        }

        if (bestScore == 0) break;

        row = r;
        col = c;
        
        if(filename[col-1] == pattern[row-1] && lastCol != col && lastRow != row){
            patternTemplate = filename[col-1] + patternTemplate;
        } else {
            
            if(!count(numbers.begin(), numbers.end(), filename[col-1]) || 
            !count(numbers.begin(), numbers.end(), pattern[row-1])) {
                if(count(alphabet.begin(), alphabet.end(), filename[col-1]) &&
                   count(alphabet.begin(), alphabet.end(), pattern[row-1])){
                    if(lastCol != col && lastRow != row){
                        if (pattern[row-1] == '@'){
                            patternTemplate = '@' + patternTemplate;
                        } else {
                            patternTemplate = '$' + patternTemplate;
                        }
                    } else {
                        patternTemplate = "@" + patternTemplate;
                    }
                } else {
                    cout << "ERROR" << endl;
                    cout << "filename: " << filename[col-1] << endl;
                    cout << "pattern: " << pattern[row-1] << endl;
                    throw runtime_error("Non-numeric, non-alphabetic characters found that do not match");
                }

                //throw std::runtime_error("There are nonnumeric characters that do not match");    
            } else if(lastCol != col && lastRow != row){
                if(pattern[row-1] == '>'){
                    patternTemplate = '>' + patternTemplate;
                } else {
                    patternTemplate = '<' + patternTemplate;
                }
                
            } else {
                patternTemplate = ">" + patternTemplate;
            }
        }

        lastRow = row;
        lastCol = col;
    }

    pattern = patternTemplate;
    int vi = 0;
    rgx = "[<>\\$@]+";
    string vdef;
    cout << "pattern template: " << patternTemplate << endl;
    for (auto i = sregex_iterator(patternTemplate.begin(), patternTemplate.end(), rgx); i != sregex_iterator(); ++i) {
        temp = (*i).str();

        if(temp.find('>') != string::npos){
            vdef = "{"; 
            vdef += variables[vi];
            vdef += ":d+}";
        } else if (temp.find('<') != string::npos){
            vdef = "{";
            vdef += variables[vi];
            vdef += ":";
            vdef.insert(vdef.length(), temp.length(), 'd');
            vdef += "}";
        } else if (temp.find('@') != string::npos) {
            vdef = "{"; 
            vdef += variables[vi];
            vdef += ":c+}";
        } else if (temp.find('$') != string::npos){
            vdef = "{";
            vdef += variables[vi];
            vdef += ":";
            vdef.insert(vdef.length(), temp.length(), 'c');
            vdef += "}";
        }

        cout << "vdef " << vdef << endl;
        cout << "pattern " << pattern << endl;
        cout << "temp " << temp << endl;
        s::replace(pattern, temp, vdef);
        cout << "new pattern " << pattern << endl;
        ++vi;
        
    }
    
    return pattern;
} 
