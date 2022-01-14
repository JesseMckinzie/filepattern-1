#include "VectorPattern.hpp"

using namespace std;

VectorPattern::VectorPattern(const string& path, const string& filePattern){
    this->path = path; // store path to target directory

    this->infile.open(path);
    if(!infile.is_open()) throw invalid_argument("Invalid path \"" + path + "\".");

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex version of pattern

    this->matchFiles();
}

void VectorPattern::matchFiles(){   
    this->filePatternToRegex();

    this->STITCH_REGEX = "corr: (.*); position: \\((.*), (.*)\\); grid: \\((.*), (.*)\\);";
    this->STITCH_VARIABLES = {"correlation","posX","posY","gridX","gridY"};

    this->regexExpression = regex(regexFilePattern);

    string line, file;
    Tuple temp;
    smatch sm;
    while(getline(infile, line)){
        file = VectorParser::getFileName(line);
        if(regex_match(file, sm, this->regexExpression)){
            temp = getVariableMap(file, sm);
            VectorParser::parseVectorLine(temp, line, this->STITCH_VARIABLES, this->STITCH_REGEX, this->variables);
            this->validFiles.push_back(temp);
        }
    }
    infile.close();
}