#include "ExternalFilePattern.hpp"

using namespace std;

ExternalFilePattern::ExternalFilePattern(const string& path, const string& filePattern, const string& blockSize="50 MB", bool recursive=false):
stream(FilesystemStream(path, true, blockSize)) {
    this->path = path; // store path to target directory

    this->blockSize = Block::parseblockSize(blockSize);

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex equivalent of the pattern
    this->recursive = recursive; // Recursive directory iteration
    this->totalFiles = 0; // Number of files matched (to be removed)
    this->mapSize = 0; //To be updated later in program, set for compiling
    this->validFilesPath = stream.getValidFilesPath(); // Store path to valid files txt file
    this->firstCall = true; // first call to next() has not occured
    this->matchFiles(); // match files to pattern
    //this->next();
    this->groupStream.open(stream.getValidFilesPath());
    this->infile.open(validFilesPath); // open temp file for the valid files
    this->endOfFile = false; // end of valid files 
    
}

ExternalFilePattern::~ExternalFilePattern(){
    fs::path path = this->validFilesPath; 
    path.remove_filename();
    uintmax_t n = fs::remove_all(path);
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

void ExternalFilePattern::matchFilesOneDir(){
    vector<string> block;

    regex patternRegex = regex(this->regexFilePattern);
    string file;
    smatch sm;

    int count = 0;
    // iterate over files    
    while(!this->stream.isEmpty()){

        block = stream.getBlock();
        if(count == 0 && block.size() == 0) throw runtime_error("Block size too small.");
        
        for (const auto& filePath : block) {
            file = s::getBaseName(filePath);

            if(regex_match(file, sm, patternRegex)){
                stream.writeValidFiles(getVariableMap(filePath, sm)); // write to txt file
                ++count;
            }
        }
    }

    if(count == 0){
        cout << ("WARNING: No files matched. Check that the pattern is correct.") << endl;
    }
}

void ExternalFilePattern::matchFilesMultDir(){
    
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
/*
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


void ExternalFilePattern::matchFiles() {
    
    filePatternToRegex(); // Get regex of filepattern

    this->mapSize = variables.size();
    
    if(recursive){
        this->matchFilesMultDir();
    } else {
        this->matchFilesOneDir();
    }
    
    //this->validGroupedFiles.push_back(validFiles);
}

/**
 * @brief In progress
 * 
 * @tparam Args 
 * @param t 
 * @param args 
 * @return vector<Tuple> 
 */
 
 /*
template <typename... Args>
vector<Tuple> ExternalFilePattern::getMatching(string& t, Args... args){
    //remove spaces if present
    //"x=[1,2]"
    vector<string> vec;
    for(const auto& arg: args...){
        vec.push_back(arg);
    }

    for(const auto& v: vec){
        v.erase(std::remove_if(variables.begin(), variables.end(), ::isspace), variables.end());
    }

    //split on commas
    std::pair<string,string> splitVaraibles = split(variables, "=");

    bool validVariable = false;
    for(const auto& var: variables.varaibles){
        if(splitVaraibles.first == std::get<0>(var)) {
            validVariable = true;
        }
    }
    if(!validVariable) throw std::invalid_argument("\"" + splitVaraibles.first + "\" is not a variable.")


    //vector<pair<string,string>> variableValues;

    //std::pair<string, string> pair;
    //size_t position;


    for(const auto& variable: splitVaraibles) {
        position = variable.find("=");
        pair.first = variable.substr(0, position);
        pair.second = variable.substr(position+1);
        if(!validFiles[0].count(pair.first)) throw invalid_argument("Variable must be in file pattern.");
        variableValues.push_back(pair);
    }
    ifstream infile(this->validFilesPath, ios_base::app);

    string out = tmpdir + "matched";
    ofstream outfile(out);

    Tuple mapping;
    while(getMap(infile, mapping)){
        if(get<0>(mapping)[variable] == value){
            writeMap(outfile, mapping);
        }
    }
    
    //vector<pair<string, int> variablesVec;
    vector<Tuple> matching;
    Types temp;
    bool match; 
    if(!stream.endOfValidFiles()){
        this->validFiles = stream.getValidFilesBlock();
        for(auto& file: this->validFiles){
            match = true;
            for(const auto& variable: variableValues) {
                temp = std::get<0>(file)[variable.first];
                if(!(s::to_string(temp) == variable.second)) match = false; 
            }
            if(match) matching.push_back(file);
        }
    }

    return matching;
    
}
*/
void ExternalFilePattern::next(){
    this->currentBlock.clear();
    // If first call, call groupby if supplied
    if(firstCall && this->group != ""){
        this->groupBy(this->group);
    } 

    if(this->group != ""){
        
        // add mapping from previous call to return block
        if(!firstCall && get<0>(this->temp).size() != 0) this->currentBlock.push_back(this->temp);
        
        // check if end of file
        streampos ptr = groupStream.tellg();
        string str;
        if(!(this->groupStream >> str)){
            // reset variables incase of another call
            this->currentBlock.clear();
            this->groupStream.close();
            this->groupStream.open(this->validFilesPath);
            this->firstCall = true;
            return;
        }
        groupStream.seekg(ptr, ios::beg);

        // iterate over vaild files temp file while the group variable is constant
        while(m::getMap(groupStream, this->temp, this->mapSize)){
            m::preserveType(temp);

            if(firstCall) {
                this->currentValue = get<0>(temp)[this->group];
                this->currentBlock.push_back(temp);
                this->firstCall = false;
            } else {

                // add to block if value matches current value
                if(get<0>(this->temp)[this->group] == this->currentValue) {
                     this->currentBlock.push_back(this->temp);
                } else { 
                    // update variable value and end loop on variable value change
                            // sort block by basename
                    sort(this->currentBlock.begin(), this->currentBlock.end(), [](Tuple& m1, Tuple& m2){
                        return get<1>(m1)[0] < get<1>(m2)[0];
                    });
                    this->currentValue = get<0>(this->temp)[this->group];
                    return;
                };
            }
        }
        sort(this->currentBlock.begin(), this->currentBlock.end(), [](Tuple& m1, Tuple& m2){
            return get<1>(m1)[0] < get<1>(m2)[0];
        });

    } else { // get a valid files block if no grouping
        this->currentBlock = this->getValidFilesBlock(); // get block of valid files
    }

    this->firstCall = false; // first call was made
}

int ExternalFilePattern::currentBlockLength(){
    return this->currentBlock.size();
}


std::vector<Tuple> ExternalFilePattern::getValidFilesBlock(){

    if(stream.endOfValidFiles()){
        std::vector<Tuple> empty;
        return empty;
    }

    return stream.getValidFilesBlock();

}

void ExternalFilePattern::groupBy(const string& groupBy) {
    this->setGroup(groupBy);
    // sort valid files externally 
    string path = stream.getValidFilesPath();
    ExternalMergeSort sort = ExternalMergeSort(std_map, 
                                               path, 
                                               path,
                                               stream.getBlockSizeStr(),
                                               groupBy,
                                               stream.mapSize);

    
}

