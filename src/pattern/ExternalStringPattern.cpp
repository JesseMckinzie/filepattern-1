#include "ExternalStringPattern.hpp"

using namespace std;

ExternalStringPattern::ExternalStringPattern(const string& path, const string& filePattern, const string& blockSize="50 MB"):
ExternalPattern(path, blockSize, false) {

    this->path = path; // store path to target directory
    this->stream = {path, true, blockSize};
    this->blockSize = Block::parseblockSize(blockSize);

    this->filePattern = filePattern; // cast input string to regex
    this->regexFilePattern = ""; // Regex equivalent of the pattern
    this->totalFiles = 0; // Number of files matched (to be removed)
    this->mapSize = 0; //To be updated later in program, set for compiling
    this->validFilesPath = stream.getValidFilesPath(); // Store path to valid files txt file
    this->firstCall = true; // first call to next() has not occured
    this->matchFiles(); // match files to pattern
    this->groupStream.open(stream.getValidFilesPath());
    this->infile.open(validFilesPath); // open temp file for the valid files
    this->endOfFile = false; // end of valid files 
    
}

ExternalStringPattern::~ExternalStringPattern(){
    /*
    fs::path path = this->validFilesPath; 
    path.remove_filename();
    uintmax_t n = fs::remove_all(path);
    */
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



