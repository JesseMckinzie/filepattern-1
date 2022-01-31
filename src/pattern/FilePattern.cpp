#include "FilePattern.hpp"
#include <chrono>

using namespace std;

FilePattern::FilePattern(const string& path, const string& filePattern, bool recursive=false) {

    this->path = path; // store path to target directory
    
    try {
        if(recursive){
            this->recursiveIterator = fs::recursive_directory_iterator(path);
        } else{ 
            this->iterator = fs::directory_iterator(path); // store iterator for target directory
        }
    } catch (const std::runtime_error& e) {
        string error = "No directory found. Invalid path \"" + path + "\"."; 
        throw std::runtime_error(error);
    }
    
    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex version of pattern
    this->recursive = recursive; // Iterate over subdirectories

    this->matchFiles();
}

void FilePattern::printFiles(){
    for(const auto& file: this->iterator){
        //cout << file << endl;
    }
}

void FilePattern::matchFilesOneDir(){
    Map mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string file, filePath;
    Tuple member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    smatch sm;
    for (const auto& entry : this->iterator) {
        // Get the current file
        filePath = entry.path();
        file = s::getBaseName(filePath);
        if(regex_match(file, sm, patternRegex)){
            validFiles.push_back(getVariableMap(filePath, sm)); // write to txt file
        }
    }
    /*
    if(validFiles.size() == 0){
        throw std::runtime_error("No files matched. Check that the pattern is correct.");
    }
    */
}

void FilePattern::matchFilesMultDir(){
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    Tuple tup;
    smatch sm;
    string file, filePath;
    // Iterate over directories and subdirectories
    for (const auto& entry : this->recursiveIterator) {
        filePath = entry.path();
        file = s::getBaseName(filePath);
        if(regex_match(file, sm, patternRegex)){
            tup = getVariableMapMultDir(filePath, sm);
            if(get<0>(tup).size() > 0){
            validFiles.push_back(tup); // write to txt file
            }
        }
    }
    if(validFiles.size() == 0){
        throw std::runtime_error("No files matched. Check that the pattern is correct.");
    }
}

void FilePattern::matchFiles() {
    
    filePatternToRegex(); // Get regex of filepattern

    //Check if valid groupBy variable
    /*
    bool validGroup = false;
    for(int i = 0; i < variables.getNumberOfVariables(); i++){
        if(variables.getVariable(i) == groupBy) {
            validGroup = true;
        }
    }

    if(!(groupBy == "" || validGroup)) { 
        throw invalid_argument("groupBy must be a variable that appears in the file pattern");
    }
    */
    if(recursive){
        this->matchFilesMultDir();
    } else {
        this->matchFilesOneDir();
    }
    
    this->validGroupedFiles.push_back(validFiles);
}
