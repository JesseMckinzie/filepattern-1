#include "StringPattern.hpp"
#include <fstream>

using namespace std;

StringPattern::StringPattern(const string& fileName, const string& filePattern) {

    this->fileName = fileName; // store path to target directory
    this->filePattern = filePattern; // cast input string to regex
    this->filesSorted = false;
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
    filePatternToRegex();
    Map mapping;
    vector<string> parsedRegex;

    int i, j;
    string s;
    string filePath;
    string file;
    Tuple member;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);

    for (const auto& filePath : this->files) {
        // Get the current file
        file = "";

        // cut off path to leave just the filename
        i = 0;

        if(filePath.find('/') != string::npos) {
            i = filePath.size()-1;

            while(filePath[i] != '/'){
                file.insert(0, 1, filePath[i]); 
                --i;
            }     
        } else {
            file = filePath;
        }
        //cout << file << endl;
        // Check if filename matches filepattern
        mapping.clear();
        std::get<1>(member).clear();
        if(regex_match(file, patternRegex)) {
            mapping["file"] = file;
            // loop over the variables in the file pattern, creating a mapping
            // between the variable name and value
            mapping = this->matchFilesLoop(mapping, file, patternRegex, parsedRegex);
            get<0>(member)= mapping;
            get<1>(member).push_back(filePath);
            validFiles.push_back(member);
        }
    }
    validGroupedFiles.push_back(validFiles);
}

