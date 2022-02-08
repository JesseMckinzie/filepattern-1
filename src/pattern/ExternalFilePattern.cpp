#include "ExternalFilePattern.hpp"

using namespace std;

ExternalFilePattern::ExternalFilePattern(const string& path, const string& filePattern, const string& blockSize="50 MB", bool recursive=false):
ExternalPattern(path, blockSize, recursive) {

    this->path = path; // store path to target directory
    this->stream = {path, true, blockSize};
    this->blockSize = Block::parseblockSize(blockSize);
    this->fp_tmpdir = "";

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex equivalent of the pattern
    this->recursive = recursive; // Recursive directory iteration
    this->totalFiles = 0; // Number of files matched (to be removed)
    this->mapSize = 0; //To be updated later in program, set for compiling
    this->validFilesPath = stream.getValidFilesPath(); // Store path to valid files txt file
    this->tmpDirectories.push_back(validFilesPath);
    this->firstCall = true; // first call to next() has not occured
    this->matchFiles(); // match files to pattern
    //this->next();
    this->groupStream.open(stream.getValidFilesPath());
    this->infile.open(validFilesPath); // open temp file for the valid files
    this->endOfFile = false; // end of valid files 

    
    
}

ExternalFilePattern::~ExternalFilePattern(){
    for(auto& dir: this->tmpDirectories){
        if(dir != "") d::remove_dir(dir);

    }
    //d::remove_dir(this->validFilesPath);
    //if(this->fp_tmpdir != "") d::remove_dir(this->fp_tmpdir);
}


void ExternalFilePattern::printFiles(){
    bool after = false;
    vector<Tuple> files;
    while(true){
        files = stream.getValidFilesBlock();
        for(const auto& file: files){
            totalFiles++;
            if(std::get<0>(file).size() < stream.mapSize) continue;
            
            for(const auto& element: std::get<0>(file)){
               cout << element.first << ":" << s::to_string(element.second) << endl;
            }
            for(const auto& element: std::get<1>(file)){
                cout << "file: " << element << endl;
            }
            cout << endl;
        }

        after = true;
        if (stream.endOfValidFiles()) break;
        
    }
}

void ExternalFilePattern::matchFiles() {
    
    filePatternToRegex(); // Get regex of filepattern

    this->mapSize = variables.size(); // Store map size for reading from txt file
    
    if(recursive){
        this->matchFilesMultDir();
    } else {
        this->matchFilesOneDir();
    }
}

void ExternalFilePattern::matchFilesOneDir(){
    vector<string> block;

    regex patternRegex = regex(this->regexFilePattern);
    string file;
    smatch sm;

    int count = 0;
    // iterate over files    
    while(!this->stream.isEmpty()){

        block = stream.getBlock();
        
        for (const auto& filePath : block) {
            file = s::getBaseName(filePath);

            if(regex_match(file, sm, patternRegex)){
                stream.writeValidFiles(getVariableMap(filePath, sm)); // write to txt file
                ++count;
            }
        }
    }
}

/* In progress - need to find a way to avoid copying file on every write */
void ExternalFilePattern::matchFilesMultDir(){
    /*
    string pattern;
    Map mapping;
    Tuple member;
    
    vector<string> parsedRegex;

    int i, j;
    string s;
    string filePath;
    string file;
    bool matched;
    vector<string> block;
    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    string str = "";
    this->validFilesPath = stream.getValidFilesPath();
    ifstream infile(validFilesPath);
    Tuple current;
    string temp;

    // iterate over directory and subdirectory in blockSize chunks
    while(!this->stream.isEmpty()){
        block = stream.getBlock(); // get block of files from directory iterator
        for (const auto& filePath : block) {
            // Get the current file

            file = s::getBaseName(filePath);

            // Check if filename matches filepattern
            mapping.clear();
            if(regex_match(file, patternRegex)) {
                matched = false;
                
                infile.open(stream.getValidFilesPath());  // open another stream to check if filename exists

                while(m::getMap(infile, current, this->mapSize)) {
                    temp = s::getBaseName(std::get<1>(current)[0]);

                    // filename has already been found in another subdirectory
                    if(temp == file){
                    
                        streampos ptr = infile.tellg();
                        matched = true; // found match

                        ptr -= 1; // move back one line in file
                        infile.seekg(ptr, ios::beg);
                        
                        str = ' ' + filePath;
                        //infile << str << endl;
                        break;
                    } 
                }
                infile.close();

                // If file was not found in another subdirectory
                if(!matched){
                    //mapping["file"] = file;
                    std::get<1>(member).push_back(filePath);

                    //std::get<0>(member)["file"] = file; // Add basename to mapping
                    // loop over the variables in the file pattern, creating a mapping
                    std::get<0>(member) = this->matchFilesLoop(mapping, file, patternRegex, parsedRegex);
                    this->mapSize = std::get<0>(member).size();

                    stream.writeValidFiles(member);
                    std::get<1>(member).clear();
                }
            }
        }
    }
    */
}


