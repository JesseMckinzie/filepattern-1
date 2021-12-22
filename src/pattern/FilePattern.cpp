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
    this->filesSorted = false; // To be removed
    this->regexFilePattern = ""; // Regex version of pattern
    this->recursive = recursive; // Iterate over subdirectories

    this->matchFiles();
}

void FilePattern::printFiles(){
    for(const auto& file: this->iterator){
        cout << file << endl;
    }
}

void FilePattern::matchFilesOneDir(){
    Map mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string filePath;
    string file;
    Tuple member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    for (const auto& entry : this->iterator) {
        // Get the current file
        filePath = entry.path();
        file = "";

        // cut off path to leave just the filename
        file = s::getBaseName(filePath);   

        // Check if filename matches filepattern
        mapping.clear();
        get<1>(member).clear();

        // Check if basename matches the pattern
        if(regex_match(file, patternRegex)) {
            //mapping["file"] = file; // Add basename to mapping

            // loop over the variables in the file pattern, creating a mapping
            mapping = this->matchFilesLoop(mapping, file, patternRegex, parsedRegex);
            get<0>(member) = mapping;
            get<1>(member).push_back(filePath);
            validFiles.push_back(member);
        }
    }

}

void FilePattern::matchFilesMultDir(){
    string pattern;
    Map mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string filePath;
    string file;
    bool matched;
    Tuple member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    string temp;
    string val;

    // Iterate over directories and subdirectories
    for (const auto& entry : this->recursiveIterator) {

        // Get the current file
        filePath = entry.path();
        file = "";

        // Get basename
        file = s::getBaseName(filePath);

        mapping.clear();
        get<1>(member).clear();

        // Check if basename matches pattern
        if(regex_match(file, patternRegex)) {
            matched = false;

            for(int i = 0; i < validFiles.size(); i++){ 
                //temp = get<1>(validFiles[0])["file"]; // to be removed
                temp = s::getBaseName(s::to_string(get<1>(validFiles[0])[0]));
               // val = s::to_string(temp); to be removed
                if(temp == file){
                    matched = true;
                    get<1>(validFiles[i]).push_back(filePath);
                    break;
                } 
            }
            if(!matched){
                //mapping["file"] = file; // Add basename to mapping
            
                mapping = this->matchFilesLoop(mapping, file, patternRegex, parsedRegex);
                get<0>(member) = mapping;
                get<1>(member).push_back(filePath);
                validFiles.push_back(member);
            }
        }
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
