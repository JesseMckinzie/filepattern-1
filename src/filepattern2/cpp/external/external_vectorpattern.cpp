#include "external_vectorpattern.hpp"

using namespace std;

ExternalVectorPattern::ExternalVectorPattern(const string& path, const string& filePattern, const string& blockSize, bool suppressWarnings):
ExternalPattern(path, blockSize, false){
    this->suppressWarnings = suppressWarnings;
    this->path = path; // store path to target directory
    this->fp_tmpdir = "";

    this->blockSize = Block::parseblockSize(blockSize);
    //this->stream = {path, true, blockSize};

    this->vectorReader.open(path);
    if(!vectorReader.is_open()) throw invalid_argument("Invalid path \"" + path + "\".");

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex version of pattern

    this->mapSize = 0; //To be updated later in program, set for compiling
    this->validFilesPath = stream.getValidFilesPath(); // Store path to valid files txt file
    this->tmpDirectories.push_back(validFilesPath);
    this->firstCall = true; // first call to next() has not occurred

    this->matchFiles();

    this->groupStream.open(stream.getValidFilesPath());
    this->infile.open(validFilesPath); // open temp file for the valid files
}

ExternalVectorPattern::~ExternalVectorPattern(){
    // Loop over stored paths and delete contents and directory
    for(auto& dir: this->tmpDirectories){
        if(dir != "") d::remove_dir(dir);

    }
}

void ExternalVectorPattern::matchFiles(){   
    this->filePatternToRegex();

    this->STITCH_REGEX = "corr: (.*); position: \\((.*), (.*)\\); grid: \\((.*), (.*)\\);"; // pattern of a line for a stitching vector
    this->STITCH_VARIABLES = {"correlation","posX","posY","gridX","gridY"}; // variables in a stitching vector

    this->mapSize = variables.size() + STITCH_VARIABLES.size(); // Change the size of the map to include stitching variables

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

string ExternalVectorPattern::inferPattern(const string& path, string& variables, const string& blockSize){
    long block = Block::parseblockSize(blockSize); // parse string
    
    vector<string> files;
    string file;
    ifstream infile(path);

    long size = sizeof(vector<string>) + sizeof(vector<vector<int>>); // memory footprint
    
    string pattern = "";
    // while the stitching vector contains another line:
    //      get block of file from stitching vector and call internal memory version of infer pattern on the block
    while(getline(infile, file)){
        if(size < block){
            file = VectorParser::getFileName(file);
            files.push_back(file);
            size += sizeof(string) + file.length() + sizeof(int)*2*file.length(); // account for size of filenames
            
        } else {
            pattern = inferPatternInternal(files, variables, pattern);
            size = sizeof(vector<string>) + sizeof(vector<vector<int>>);
            files.clear();
        }
    }
    // when no more lines are left, process the last block if its nonempty
    if(files.size() != 0){
        pattern = inferPatternInternal(files, variables, pattern);
    }

    return pattern;
}