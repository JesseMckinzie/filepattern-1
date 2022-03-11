#include "filepattern.hpp"
#include <chrono>

using namespace std;

FilePattern::FilePattern(const string& path, const string& filePattern, bool recursive=false) {
    if(filePattern == ""){
        this->getPathFromPattern(path); // set path and filePattern
        try {
            this->recursiveIterator = fs::recursive_directory_iterator(this->path);
            this->recursive = true;
            this->justPath = true;
        } catch (const std::runtime_error& e) {
            string error = "No directory found. Invalid path \"" + path + "\"."; 
            throw std::runtime_error(error);
        }
    } else {
        this->justPath = false;
        this->path = path; // store path to target directory
        this->filePattern = filePattern; // cast input string to regex
        this->recursive = recursive; // Iterate over subdirectories
        try {
            if(recursive){
                this->recursiveIterator = fs::recursive_directory_iterator(this->path);
            } else{ 
                this->iterator = fs::directory_iterator(this->path); // store iterator for target directory
            }
        } catch (const std::runtime_error& e) {
            string error = "No directory found. Invalid path \"" + path + "\"."; 
            throw std::runtime_error(error);
        }
    }
    
    this->regexFilePattern = ""; // Regex version of pattern
    

    this->matchFiles();
    this->sortFiles();
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
        filePath = entry.path().string();
        replace(filePath.begin(), filePath.end(), '\\', '/');
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
        filePath = entry.path().string();
        replace(filePath.begin(), filePath.end(), '\\', '/');
        if(this->justPath){
            file = s::eraseSubStr(filePath, this->path);
        } else {
            file = s::getBaseName(filePath);
        }
        
        if(regex_match(file, sm, patternRegex)){
            if(this->justPath) tup = getVariableMap(filePath, sm);
            else tup = getVariableMapMultDir(filePath, sm);
            if(get<0>(tup).size() > 0){
            validFiles.push_back(tup); // write to txt file
            }
        }
    }
}

void FilePattern::matchFiles() {
    
    filePatternToRegex(); // Get regex of filepattern

    if(recursive){
       this->matchFilesMultDir();
    } else {
        this->matchFilesOneDir();
    }
}
