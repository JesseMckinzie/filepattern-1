#include "StringPattern.hpp"
#include <fstream>

using namespace std;

StringPattern::StringPattern(const string& fileName, const string& filePattern) {

    this->fileName = fileName; // store path to target directory
    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = "";
    this->readFile();
    this->matchFiles();
}

void StringPattern::readFile(){
    string str;
    ifstream in(fileName);
    if(!in.is_open()) {
        throw runtime_error("File \"" + fileName + "\" not found.");
    }
    while(getline(in, str)){
        if(str.size()) files.push_back(str);
    }

}

void StringPattern::matchFiles(){
    filePatternToRegex();
    Map mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string file, filePath;
    Tuple member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    cout << regexFilePattern << endl;
    smatch sm;
    for (const auto& filePath : this->files) {
        // Get the current file
        cout << filePath << endl;
        if(regex_match(filePath, sm, patternRegex)){
            validFiles.push_back(getVariableMap(filePath, sm)); // write to txt file
        }
    }
}

