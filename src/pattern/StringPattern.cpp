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
    try {
        ifstream in(fileName);
        if(!in) {
            throw invalid_argument("File \"" + fileName + "\" not found.");
        }
        while(getline(in, str)){
            if(str.size()) files.push_back(str);
        }
    } catch (const exception& e){
        cout << e.what();
    }
}

void StringPattern::matchFiles(){
    Map mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string file, filePath;
    Tuple member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    smatch sm;
    for (const auto& entry : this->files) {
        // Get the current file
        filePath = entry;
        file = s::getBaseName(filePath);
        if(regex_match(file, sm, patternRegex)){
            validFiles.push_back(getVariableMap(filePath, sm)); // write to txt file
        }
    }
}

