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
        i = 0;
            i = filePath.size()-1;

        while(filePath[i] != '/'){
            file.insert(0, 1, filePath[i]); 
            --i;
        }     
        // Check if filename matches filepattern
        mapping.clear();
        get<1>(member).clear();
        if(regex_match(file, patternRegex)) {
            //mapping["file"] = file;
            // loop over the variables in the file pattern, creating a mapping
            // between the variable name and value
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
    Types temp;
    string val;
    for (const auto& entry : this->recursiveIterator) {
        //cout << entry.path() << endl;
        // Get the current file
        filePath = entry.path();
        file = "";

        // cut off path to leave just the filename

        i = filePath.size()-1;

        while(filePath[i] != '/'){
            file.insert(0, 1, filePath[i]); 
            --i;
        }     
        
        // Check if filename matches filepattern
        mapping.clear();
        get<1>(member).clear();
        if(regex_match(file, patternRegex)) {
            matched = false;
            for(int i = 0; i < validFiles.size(); i++){ 
                temp = get<0>(validFiles[0])["file"];
                val = s::to_string(temp);
                if(val == file){
                    matched = true;
                    get<1>(validFiles[i]).push_back(filePath);
                    break;
                } 
            }
            if(!matched){
                mapping["file"] = file;
                //mapping["filePath"] = filePath;
            
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
