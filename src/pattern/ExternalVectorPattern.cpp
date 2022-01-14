#include "ExternalVectorPattern.hpp"

using namespace std;

ExternalVectorPattern::ExternalVectorPattern(const string& path, const string& filePattern, const string& blockSize):
ExternalPattern(path, blockSize, false){
    this->path = path; // store path to target directory

    this->blockSize = Block::parseblockSize(blockSize);
    this->stream = {path, true, blockSize};

    this->vectorReader.open(path);
    if(!vectorReader.is_open()) throw invalid_argument("Invalid path \"" + path + "\".");

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex version of pattern

    this->totalFiles = 0; // Number of files matched (to be removed)
    this->mapSize = 0; //To be updated later in program, set for compiling
    this->validFilesPath = stream.getValidFilesPath(); // Store path to valid files txt file
    this->firstCall = true; // first call to next() has not occured

    this->matchFiles();

    this->groupStream.open(stream.getValidFilesPath());
    this->infile.open(validFilesPath); // open temp file for the valid files
    this->endOfFile = false; // end of valid files 
}

void ExternalVectorPattern::matchFiles(){   
    this->filePatternToRegex();

    this->STITCH_REGEX = "corr: (.*); position: \\((.*), (.*)\\); grid: \\((.*), (.*)\\);";
    this->STITCH_VARIABLES = {"correlation","posX","posY","gridX","gridY"};

    this->mapSize = variables.size() + STITCH_VARIABLES.size();

    this->regexExpression = regex(regexFilePattern);

    string line, file;
    Tuple temp;
    smatch sm;
    while(getline(vectorReader, line)){
        file = VectorParser::getFileName(line);
        if(regex_match(file, sm, this->regexExpression)){
            temp = getVariableMap(file, sm);
            VectorParser::parseVectorLine(temp, line, this->STITCH_VARIABLES, this->STITCH_REGEX, this->variables);
            this->stream.writeValidFiles(temp);
        }
    }
    vectorReader.close();
}