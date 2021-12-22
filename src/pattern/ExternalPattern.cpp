#include "ExternalPattern.hpp"

using namespace std;


void ExternalPattern::filePatternToRegex(){

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

            // store variable information in variable structure
            variables.addNode(str, regexPattern, stringLength);

            regexPattern = "";
            str = "";
            stringLength = 0;

        } else if (c == ':') {

            afterColon = true;

        } else if (inBrackets) { 

            if(afterColon) {
                // check if the character is a valid key (c, d, or +)
                if(patternMap.find(c) == patternMap.end()){
                    error = "Invalid pattern \"" + c + "\" found in filepattern. Patterns must be \"d\", \"c\", or \"+\".";
                    throw invalid_argument(error);
                }
                regexFilePattern += patternMap[c]; // Add corresponding regex 
                regexPattern += patternMap[c];
            } else {
                // character is a part of the variable name
                str += c;
            }
            

        } else { // Add character without modifications if not in brackets
            // check if character is valid
            if(isalpha(c) || isdigit(c) || c == '.' || c == '-' || c == '_'){
                regexFilePattern += c;
                ++stringLength;
            } else {
                error = "Invalid character \"" + c + "\" in filepattern.";
                throw std::invalid_argument(error);
            }
        } 
    }

    // iteration finished inside of the brackets
    if (inBrackets) {
        cout << "Missing closing bracket in file pattern" << endl;
        exit(1);
    }

}

/*
 * From https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
 */
vector<string> ExternalPattern::split (string& s, const string& delimiter) {
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




void ExternalPattern::printValidFiles() {
    // If no valid files
    if(validFiles.size() == 0) {
        cout << "No valid files." << endl; 
        return;
    }
/*
    for(const auto & fileVector : this->validGroupedFiles) {
        cout << "[";
        for(const auto & file : fileVector) {
            cout << "{";
            for(const auto m:) {
                std::cout << it->first << ", " << it->second << "; ";
            }
            cout << "}" << endl;
        }
        cout << "]" << endl;
    }
    */
}

string ExternalPattern::getPattern(){
    return this->filePattern;
}

void ExternalPattern::setPattern(const string& pattern){
    this->filePattern = pattern;
}

string ExternalPattern::getRegexPattern(){
    return this->regexFilePattern;
}

void ExternalPattern::setGroup(const string& group){
    this->group = group;
}