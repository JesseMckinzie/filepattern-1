#include "vectorpattern.hpp"

using namespace std;

VectorPattern::VectorPattern(const string& path, const string& filePattern, bool suppressWarnings){
    this->suppressWarnings = suppressWarnings;
    this->path = path; // store path to target directory

    this->infile.open(path);
    if(!infile.is_open()) throw invalid_argument("Invalid path \"" + path + "\".");

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex version of pattern

    this->matchFiles(); // match lines of stitching vector to the pattern
    this->sortFiles();
}

void VectorPattern::matchFiles(){   
    this->filePatternToRegex(); // get regex version of the pattern

    this->STITCH_REGEX = "corr: (.*); position: \\((.*), (.*)\\); grid: \\((.*), (.*)\\);"; // regex of a stitching vector line
    this->STITCH_VARIABLES = {"correlation","posX","posY","gridX","gridY"}; // stitching vector variables

    this->regexExpression = regex(regexFilePattern); // cast pattern to regex

    string line, file;
    Tuple temp;
    smatch sm;
    while(getline(infile, line)){
        file = VectorParser::getFileName(line); // get filename from line of stitching vector
        if(regex_match(file, sm, this->regexExpression)){ // match to regex. groups are in sm
            temp = getVariableMap(file, sm); // get the variable map for the filename
            VectorParser::parseVectorLine(temp, line, this->STITCH_VARIABLES, this->STITCH_REGEX, this->variables); // parse the vector line
            this->validFiles.push_back(temp); // add file mapping to matched files
        }
    }
    infile.close();
}

string VectorPattern::inferPattern(const string& path, string& variables){
    string file;
    ifstream infile(path);
    vector<string> files;

    // get filenames from stitching vector 
    while(getline(infile, file)){
        file = VectorParser::getFileName(file);
        files.push_back(file);
    }

    // infer pattern from vector of filenames
    return inferPatternInternal(files, variables);
}