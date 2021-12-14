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
    this->filesSorted = false;
    this->regexFilePattern = "";
    this->recursive = recursive;

    this->matchFiles(true, "");
}

void FilePattern::printFiles(){
    for(const auto& file: this->iterator){
        cout << file << endl;
    }
}

void FilePattern::matchFilesOneDir(bool cutPath){
    map<string, string> mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string filePath;
    string file;
    p::pair<map<string, string>, vector<string>> member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    for (const auto& entry : this->iterator) {
        // Get the current file
        filePath = entry.path();
        file = "";

        // cut off path to leave just the filename
        i = 0;
        if(cutPath) {
            i = filePath.size()-1;

            while(filePath[i] != '/'){
                file.insert(0, 1, filePath[i]); 
                --i;
            }     
        }
        // Check if filename matches filepattern
        mapping.clear();
        member.second.clear();
        if(regex_match(file, patternRegex)) {
            mapping["file"] = file;
            // loop over the variables in the file pattern, creating a mapping
            // between the variable name and value
            mapping = this->matchFilesLoop(mapping, file, patternRegex, parsedRegex);
            member.first = mapping;
            member.second.push_back(filePath);
            validFiles.push_back(member);
        }
    }

}

void FilePattern::matchFilesMultDir(bool cutPath){
    string pattern;
    map<string, string> mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string filePath;
    string file;
    bool matched;
    p::pair<map<string, string>, vector<string>> member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    for (const auto& entry : this->recursiveIterator) {
        //cout << entry.path() << endl;
        // Get the current file
        filePath = entry.path();
        file = "";

        // cut off path to leave just the filename
        i = 0;
        if(cutPath) {
            i = filePath.size()-1;

            while(filePath[i] != '/'){
                file.insert(0, 1, filePath[i]); 
                --i;
            }     
        }
        // Check if filename matches filepattern
        mapping.clear();
        if(regex_match(file, patternRegex)) {
            matched = false;
            for(int i = 0; i < validFiles.size(); i++){ 
                if(validFiles[i].first["file"] == file){
                    matched = true;
                    validFiles[i].second.push_back(filePath);
                    break;
                } 
            }
            if(!matched){
                mapping["file"] = file;
                //mapping["filePath"] = filePath;
            
                i = 0; // pointer for filename string

                // loop over the variables in the file pattern, creating a mapping
                // between the variable name and value
                for(j = 0; j < variables.variables.size(); ++j){
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
                member.first = mapping;
                member.second.push_back(filePath);
                validFiles.push_back(member);
            }
        }
    }
}


void FilePattern::matchFiles(const bool& cutPath=true, const string& groupBy="") {
    
    filePatternToRegex(); // Get regex of filepattern

    //Check if valid groupBy variable
    bool validGroup = false;
    for(int i = 0; i < variables.getNumberOfVariables(); i++){
        if(variables.getVariable(i) == groupBy) {
            validGroup = true;
        }
    }

    if(!(groupBy == "" || validGroup)) { 
        throw invalid_argument("groupBy must be a variable that appears in the file pattern");
    }

    if(recursive){
        this->matchFilesMultDir(cutPath);
    } else {
        this->matchFilesOneDir(cutPath);
    }
    
    this->validGroupedFiles.push_back(validFiles);
}
