#include "ExternalStringPattern.hpp"

using namespace std;

ExternalStringPattern::ExternalStringPattern(const string& path, const string& filePattern, const string& blockSize="50 MB"):
ExternalPattern(path, blockSize, false) {

    this->path = path; // store path to target directory
    //this->stream = {path, true, blockSize};
    this->blockSize = Block::parseblockSize(blockSize);
    this->fp_tmpdir = "";

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex equivalent of the pattern
    this->totalFiles = 0; // Number of files matched (to be removed)
    this->mapSize = 0; //To be updated later in program, set for compiling
    this->validFilesPath = stream.getValidFilesPath(); // Store path to valid files txt file
    this->tmpDirectories.push_back(validFilesPath);
    this->firstCall = true; // first call to next() has not occured
    this->matchFiles(); // match files to pattern
    this->groupStream.open(stream.getValidFilesPath());
    this->infile.open(validFilesPath); // open temp file for the valid files
    this->endOfFile = false; // end of valid files 
    
}

ExternalStringPattern::~ExternalStringPattern(){
    for(auto& dir: this->tmpDirectories){
        if(dir != "") d::remove_dir(dir);
    }
}

void ExternalStringPattern::matchFiles(){
    this->filePatternToRegex();

    this->mapSize = variables.size();

    vector<string> block;

    regex patternRegex = regex(this->regexFilePattern);
    string file;
    smatch sm;

    int count = 0;
    // iterate over files    
    while(!this->stream.isEmpty()){

        block = stream.getBlock();

        //if(count == 0 && block.size() == 0) throw runtime_error("Block size too small.");
        
        for (const auto& file : block) {
            //file = s::getBaseName(filePath);
        
            if(regex_match(file, sm, patternRegex)){
                stream.writeValidFiles(getVariableMap(file, sm)); // write to txt file
                ++count;
            }
        }
    }
    
}
