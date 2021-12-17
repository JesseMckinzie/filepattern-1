#include "ExternalFilePattern.hpp"

using namespace std;

ExternalFilePattern::ExternalFilePattern(const string& path, const string& filePattern, const string& blockSize="50 MB", bool recursive=false):
stream(FilesystemStream(path, true, blockSize)) {

    this->path = path; // store path to target directory
    //this->iterator = fs::directory_iterator(path);
    //FilesystemStream stream = FilesystemStream(this->iterator, blockSize);   
    
    this->filePattern = filePattern; // cast input string to regex
    this->filesSorted = false;
    this->regexFilePattern = "";
    this->recursive = recursive;
    this->totalFiles = 0;
    this->mapSize = 0; //To be updated later in program, set for compiling
    this->validFilesPath = stream.getValidFilesPath();
    this->matchFiles(true, "");
    this->infile.open(validFilesPath);

}

int ExternalFilePattern::getCounter(){
    return stream.counter;
}

vector<Tuple> ExternalFilePattern::get(){
    vector<Tuple> vec;
    Tuple member;
    
    long size = sizeof(vector<Tuple>);

    Map map;
    string str;
    string key, value;
    Type result;
    int valueLength;
    size_t pos;

    while(size < blockSize && this->infile >> str){
        
        if (map.size() == (this->mapSize)) {
            
            size += sizeof(map) + sizeof(vector<string>);
            
            //sizeof(Tuple<map<string, BaseObject>, vector<string>>) +
            for(const auto& item : map){
                size += item.first.length() + item.second.length();
            }
            std::get<0>(member) = map;
            std::get<1>(member).push_back(str);
            vec.push_back(member);
            map.clear();
            std::get<1>(member).clear();
            //infile >> str;
        } 

        pos = str.find(":");
        key = str.substr(0, pos);
        valueLength = str.length() - pos;
        value = str.substr(pos+1, valueLength);
        if(is_number(value)){
            result = stoi(value);
        } else {
            result = value;
        }
        map[key] = value;
        size += valueLength + pos;
    }

/*
    streampos ptr = infile.tellg();
    if(!(this->infile >> str)){
        validFilesEmpty = true;
    }
    infile.seekg(ptr, ios::beg);
*/
    return vec;
    
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
               cout << element.first << ":" << element.second << endl;
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

Map ExternalFilePattern::matchFilesLoop(Map& mapping, const string& file, const regex& patternRegex, vector<string>& parsedRegex){
    
    int i = 0;
    string pattern;
    string s = "";

    for(int j = 0; j < variables.variables.size(); ++j){
        i += variables.getDistance(j);
        pattern = variables.getRegex(j);
        if(pattern == "[0-9]+" || pattern == "[a-zA-Z]+"){
            s.push_back(file[i]); // char -> string
            while(regex_match(s, regex(pattern))) {
                mapping[variables.getVariable(j)] += file[i];
                i++;
                s = "";
                s.push_back(file[i]);
            }
            if(is_number(mapping[variables.getVariable(j)]){ 
                mapping[variables.getVariable(j)] = stoi(mapping[variables.getVariable(j)]);
            }
            s = "";
        } else {
            parsedRegex = variables.parseRegex(j);
            for(const auto& expr: parsedRegex){
                mapping[variables.getVariable(j)] += file[i];
                i++;
            }
            if(is_number(mapping[variables.getVariable(j)])){ 
                mapping[variables.getVariable(j)] = stoi(mapping[variables.getVariable(j)]);
            }
        }
    }
    return mapping;
}

void ExternalFilePattern::matchFilesOneDir(bool cutPath){
    Map mapping;
    vector<string> parsedRegex;
    vector<string> block;

    int i, j;
    string s;
    string filePath;
    string file;
    Tuple member;

    // Iterate over every file in directory
    regex patternRegex = regex(this->regexFilePattern);
    int number = 0;
    while(!this->stream.isEmpty()){
        block = stream.getBlock();
        for (const auto& entry : block) {
            
            // Get the current file
            filePath = entry;
            file = "";

            // cut off path to leave just the filename
            i = 0;
            if(cutPath) {
                i = filePath.size()-1;

                while(filePath[i] != '/'){
                    file.insert(0, 1, filePath[i]); 
                    --i;
                }     
            }
            // Check if filename matches filepattern
            mapping.clear();
            std::get<1>(member).clear();
            if(regex_match(file, patternRegex)) {
                //mapping["file"] = filePath;
                // loop over the variables in the file pattern, creating a mapping
                // between the variable name and value
                mapping = this->matchFilesLoop(mapping, file, patternRegex, parsedRegex);
                std::get<0>(member) = mapping;
                std::get<1>(member).push_back(filePath);
                //validFiles.push_back(mapping);

                stream.writeValidFiles(member);
                number++;
            }
        }
    }
}

void ExternalFilePattern::matchFilesMultDir(bool cutPath){
    
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
    ifstream infile;
    string str;
    this->validFilesPath = stream.getValidFilesPath();
    cout << validFilesPath << endl;
    ifstream validin(validFilesPath);
    Tuple current;
    while(!this->stream.isEmpty()){
        block = stream.getBlock();
        for (const auto& entry : block) {
            // Get the current file
            filePath = entry;
            file = "";

            // cut off path to leave just the filename
            //i = 0;
            i = filePath.size()-1;

            while(filePath[i] != '/'){
                file.insert(0, 1, filePath[i]); 
                --i;
            }     

            // Check if filename matches filepattern
            mapping.clear();
            if(regex_match(file, patternRegex)) {
                matched = false;

                infile.open(stream.getValidFilesPath());
                while(getMap(validin, current)) {
                    if(std::get<0>(current)["file"] == entry){
                        streampos ptr = infile.tellg();
                        matched = true;
                        ofstream outfile(stream.getValidFilesPath());
                        outfile.seekp(ptr, ios::beg);
                        
                        str += ", " + filePath;
                        outfile << str <<'\n';
                        outfile.close();
                        break;
                    } 
                }
                
                if(!matched){
                    //mapping["file"] = file;
                    std::get<1>(member).push_back(filePath);
                
                    i = 0; // pointer for filename string
                    // loop over the variables in the file pattern, creating a mapping
                    // between the variable name and value
                    std::get<0>(member)["file"] = file;
                    std::get<0>(member) = this->matchFilesLoop(mapping, file, patternRegex, parsedRegex);
                    this->mapSize = std::get<0>(member).size();
                    //validFiles.push_back(mapping);
                    stream.writeValidFiles(member);

                }
            }
        }
    }
    
}


void ExternalFilePattern::matchFiles(const bool& cutPath=true, const string& groupBy="") {
    
    filePatternToRegex(); // Get regex of filepattern
    this->mapSize = variables.variables.size();
    //Check if valid groupBy variable
    bool validGroup = false;
    for(int i = 0; i < variables.getNumberOfVariables(); i++){
        if(variables.getVariable(i) == groupBy) {
            validGroup = true;
        }
    }

    if(!(groupBy == "" || validGroup)) { 
        throw invalid_argument("groupBy must be a variable that appears in the file pattern");
    }

    if(recursive){
        this->matchFilesMultDir(cutPath);
    } else {
        this->matchFilesOneDir(cutPath);
    }
    
    this->validGroupedFiles.push_back(validFiles);
}

vector<Tuple> ExternalFilePattern::getMatching(string variables){

    //remove spaces if present
    //variables.erase(std::remove_if(variables.begin(), variables.end(), ::isspace), variables.end());
    
    //split on commas
    vector<string> splitVaraibles = split(variables, ",");
    vector<pair<string,string>> variableValues;

    std::pair<string, string> pair;
    size_t position;
    for(const auto& variable: splitVaraibles) {
        position = variable.find("=");
        pair.first = variable.substr(0, position);
        pair.second = variable.substr(position+1);
        //if(!validFiles[0].count(pair.first)) throw invalid_argument("Variable must be in file pattern.");
        variableValues.push_back(pair);
    }
    
    //vector<pair<string, int> variablesVec;
    vector<Tuple> matching;

    bool match;
    if(!stream.endOfValidFiles()){
        this->validFiles = stream.getValidFilesBlock();
        for(auto& file: this->validFiles){
            match = true;
            for(const auto& variable: variableValues) {
                if(!(std::get<0>(file)[variable.first] == variable.second)) match = false; 
            }
            if(match) matching.push_back(file);
        }
    }

    return matching;
}

std::vector<Tuple> ExternalFilePattern::getValidFilesBlock(){
    return stream.getValidFilesBlock();
}

void ExternalFilePattern::groupBy(const string& groupBy) {

    string path = stream.getValidFilesPath();
    ExternalMergeSort sort = ExternalMergeSort(std_map, 
                                               path, 
                                               path,
                                               stream.getBlockSizeStr(),
                                               groupBy,
                                               stream.mapSize);
    
    vector<Tuple> validFiles;

    validFiles = stream.getValidFilesBlock();

    /*
    sort(validFiles.begin(), validFiles.end(), [&groupBy = as_const(groupBy)]
     (Tuple& m1, Tuple& m2){
        return m1.first[groupBy] < m2.first[groupBy];
    });
    */
    string currentValue = std::get<0>(validFiles[0])[groupBy];
    vector<Tuple> emptyVec;
    int i = 0;
    int group_ptr = 0;

    long size = sizeof(vector<vector<Tuple>>);

    //string out = tmpdir + "validGroupedFiles";
    //ofstream outfile(validGroupedFiles);
    while(i < this->validFiles.size()){
        this->validGroupedFiles.push_back(emptyVec);
        size += sizeof(emptyVec);
        while(std::get<0>(this->validFiles[i])[groupBy] == currentValue && size < this->blockSize) {
            this->validGroupedFiles[group_ptr].push_back(this->validFiles[i]);
            ++i;
            if (i >= this->validFiles.size()){ 
                // some empty condition becomes true
                break;
            }
            size += sizeof(validFiles[i]);
            for(const auto map: std::get<0>(validFiles[i])){
                size += 2*sizeof(string) + map.first.length() + map.second.length();
            }
        }
        if (i < this->validFiles.size()) currentValue = std::get<0>(this->validFiles[i])[groupBy];
        ++group_ptr;
    }

}

bool ExternalFilePattern::getMap(ifstream& infile, Tuple& member){
    string str;
    Map map;

    string key, value;
    int valueLength;
    size_t pos;

    while(getline(infile, str)){

        if (map.size() == (this->mapSize)) {
            //size += sizeof(map) + sizeof(vector<string>);
            
            //sizeof(Tuple<map<string, BaseObject>, vector<string>>) +
            //for(const auto& item : map){
            //    size += item.first.length() + item.second.length();
            //}
            std::get<0>(member) = map;

            std::get<1>(member).push_back(str);
            return true;
        } 

        pos = str.find(":");
        key = str.substr(0, pos);
        valueLength = str.length() - pos;
        value = str.substr(pos+1, valueLength);

        map[key] = value;
        //size += valueLength + pos;
    }

    return false;
}
