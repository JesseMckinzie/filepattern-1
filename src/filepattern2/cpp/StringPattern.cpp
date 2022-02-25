#include "StringPattern.hpp"
#include <fstream>

using namespace std;

StringPattern::StringPattern(const string& fileName, const string& filePattern) {
    this->fileName = fileName; // store path to target directory
    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = "";
    this->readFile(); // read file into memory
    this->matchFiles(); // match files to pattern
    this->sortFiles();
}

void StringPattern::readFile(){
    string str;
    ifstream in(fileName);
    if(!in.is_open()) {
        throw runtime_error("File \"" + fileName + "\" not found.");
    }
    // read filenames into memory
    while(getline(in, str)){
        if(str.size()) files.push_back(str);
    }

}

void StringPattern::matchFiles(){
    filePatternToRegex(); // get regex equivalent of filepattern

    string filePath;
    regex patternRegex = regex(this->regexFilePattern); // convert to regex
    smatch sm; // store matching groups
    for (const auto& filePath : this->files) {
        // Get the current file
        if(regex_match(filePath, sm, patternRegex)){
            validFiles.push_back(getVariableMap(filePath, sm)); // write to txt file
        }
    }
}

