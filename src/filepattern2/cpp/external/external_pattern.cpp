#include "external_pattern.hpp"

using namespace std;

namespace fs = std::filesystem;


ExternalPattern::ExternalPattern(const string& path, const string& blockSize, bool recursive):
stream(FilesystemStream(path, recursive, blockSize)){
    this->validFilesPath = stream.getValidFilesPath(); // Store path to valid files txt file
    this->tmpDirectories.push_back(validFilesPath);
    this->infile.open(validFilesPath); // open temp file for the valid files
    this->groupStream.open(stream.getValidFilesPath());
}

ExternalPattern::~ExternalPattern() {
    this->infile.close();
    this->groupStream.close();
}


void ExternalPattern::getMatchingLoop(ifstream& infile, 
                                      ofstream& outfile,
                                      const string& variable, 
                                      const vector<Types>& values, 
                                      Types& temp,
                                      Tuple& tempMap){
    // while infile still has a map: 
    //   read map and add to outfile if value matches
    while(m::getMap(infile, tempMap, this->mapSize)){
        temp = get<0>(tempMap)[variable];
        for(const auto& value: values){  

            if(s::to_string(temp) == s::to_string(value)){
                m::writeMap(outfile, tempMap);
            }
        }
    }

}

void ExternalPattern::getMatchingHelper(const tuple<string, vector<Types>>& variableMap, const string& matching){
    string variable = get<0>(variableMap); // get key from argument
    vector<Types> values = get<1>(variableMap); // get value from argument

    // throw error if argument variable is not in the pattern
    if(find(begin(variables), end(variables), variable) == end(variables)) {
        throw invalid_argument("\"" + variable + "\" is not a variable. Use a variable that is contained in the pattern.");
    }

    Types temp;
    vector<Tuple> iter;
    Tuple tempMap;
    // if first or only variable to match, iterate over valid files
    if(!(fs::exists(matching))) {

        ifstream validFiles(this->validFilesPath);    
        ofstream outfile(matching);

        this->getMatchingLoop(validFiles, outfile, variable, values, temp, tempMap);
    
        validFiles.close();
        outfile.close();
        
    } else { // iterate files that matched previous call
    
        // copy contents
        std::ifstream infile(matching);
        std::ofstream outfile(matchingCopy);
        outfile << infile.rdbuf();
        infile.close();
        outfile.close();


        // clear contents of matching.txt
        std::ofstream ofs;
        ofs.open(matching, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
       
        
        ifstream in(matchingCopy); 
        ofstream out(matching);
        this->getMatchingLoop(in, out, variable, values, temp, tempMap);
    }
}

string ExternalPattern::getMatching(const vector<tuple<string, vector<Types>>>& variables){
    // construct temporary directory path
    this->fp_tmpdir = fs::temp_directory_path().string();
    if (s::endsWith(fp_tmpdir, "\\")) this->fp_tmpdir.pop_back();
    this->fp_tmpdir += slash + "filepattern_" + s::getTimeString() + slash;

    this->tmpDirectories.push_back(this->fp_tmpdir);

    // remove old contents if it already exists
    if(fs::exists(fp_tmpdir)){
        fs::remove_all(fp_tmpdir);
    }

    bool created = fs::create_directory(fp_tmpdir);

    fs::permissions(this->fp_tmpdir, fs::perms::all);

    // create a path to store matching files
    this->matching = fp_tmpdir + "matching.txt";
    this->matchingCopy = fp_tmpdir + slash + "temp.txt";
    if(fs::exists(matching)) {

        fs::remove(matching);
    }

    // Iterate over each variable passed from front end
    for(const auto& variableMap: variables){
        this->getMatchingHelper(variableMap, matching);
    }

    this->matchingStream.open(this->matching);

    return matching;
}

vector<Tuple> ExternalPattern::getMatchingBlock(){

    long size = sizeof(vector<Tuple>); // store amount of memory begin used by this method
    // throw error if blockSize is too small to fit the empty vector
    if(size > this->blockSize) throw runtime_error("The block size is smaller than the size of a vector. The block size must be increased");

    Tuple temp; 
    vector<Tuple> vec;
    bool moreFiles;

    // If there are no more files to return, return an empty vector
    if(!matchingStream.is_open()) return vec;

    // while memory is still available:
    //      get file from matching.txt in tmpdir and add to return vec
    while(size < this->blockSize){
        moreFiles = m::getMap(this->matchingStream, temp, this->mapSize);

        m::preserveType(temp);

        if(!moreFiles) {
            this->matchingStream.close();
            break;
        }
        vec.push_back(temp);

        size += m::getSize(temp);
    }

    return vec;
}

void ExternalPattern::nextGroup(){
    if(firstCall) this->groupBy(this->group);
    
    // add mapping from previous call to return block
    if(!firstCall){
        this->currentGroup[0].second.clear();
        if(get<0>(this->temp).size() != 0) this->currentGroup[0].second.push_back(this->temp);
    }
    
    // check if end of file
    streampos ptr = groupStream.tellg();
    string str; 
    vector<Tuple> empty;
    if(!(this->groupStream >> str)){
        // reset variables incase of another call
        this->currentGroup[0].second.clear();
        this->groupStream.close();
        this->groupStream.open(this->validFilesPath);
        this->firstCall = true;
        return;
    }
    groupStream.seekg(ptr, ios::beg);
    // iterate over vaild files temp file while the group variable is constant
    while(m::getMap(groupStream, this->temp, this->mapSize)){
        m::preserveType(temp);

        // if method has not been called, initialize data structures
        if(firstCall) {
            this->currentValue = get<0>(temp)[this->group];
            this->currentGroup.resize(1);
            this->currentGroup[0] = make_pair(make_pair(this->group, currentValue), empty);
            this->currentGroup[0].second.push_back(temp);
            this->firstCall = false;
        } else {
            // add to block if value matches current value
            this->currentGroup[0].first = make_pair(this->group, currentValue);
    
            if(get<0>(this->temp)[this->group] == this->currentValue) {
                    this->currentGroup[0].second.push_back(this->temp);
            } else { 
                // update variable value and end loop on variable value change
                        // sort block by basename
                sort(this->currentGroup[0].second.begin(), this->currentGroup[0].second.end(), [](Tuple& m1, Tuple& m2){
                    return get<1>(m1)[0] < get<1>(m2)[0];
                });
                this->currentValue = get<0>(this->temp)[this->group];
                //cout << "else return" << endl;
                //cout << "result: " << currentGroup[0].first.first << ", " << s::to_string(currentGroup[0].first.second) << endl << endl;
                return;
            };
        }
    }
    sort(this->currentGroup[0].second.begin(), this->currentGroup[0].second.end(), [](Tuple& m1, Tuple& m2){
        return get<1>(m1)[0] < get<1>(m2)[0];
    });
}

void ExternalPattern::next(){
    this->currentBlock = this->getValidFilesBlock(); // get block of valid files 
}

int ExternalPattern::currentBlockLength(){
    if(this->group == "") return this->currentBlock.size();// + this->currentGroup.size();
    else return this->currentGroup[0].second.size();
}

std::vector<Tuple> ExternalPattern::getValidFilesBlock(){
    // return an empty vector if no more files
    if(stream.endOfValidFiles()){
        std::vector<Tuple> empty;
        return empty;
    }

    // return a vector with a memory footprint of at most blockSize
    return stream.getValidFilesBlock();

}

void ExternalPattern::groupBy(const string& groupBy) {
    this->setGroup(groupBy);
    // sort valid files externally 
    string path = stream.getValidFilesPath();
    this->tmpDirectories.push_back(path);
    ExternalMergeSort sort = ExternalMergeSort(std_map, 
                                               path, 
                                               path,
                                               stream.getBlockSizeStr(),
                                               groupBy,
                                               stream.mapSize);
}

string ExternalPattern::externalOutPutName(){
    string outputName = this->filePattern; // store a copy of the filePattern to modify
    regex patternRegex(this->regexFilePattern); // regex version of filePattern

    std::ifstream infile;
    
    Tuple temp, min, max;
    int idx = 0;
    string tempStr;
    // iterate over every variable in mapping
    for(const auto& var: variables){
        infile.open(this->validFilesPath); // open matched files

        m::getMap(infile, temp, this->mapSize);
        min = temp; 
        max = temp;

        // find min and max values 
        while(m::getMap(infile, temp, this->mapSize)){
            if(get<0>(temp)[var] < get<0>(min)[var]) min = temp;
            if(get<0>(temp)[var] > get<0>(max)[var]) max = temp;
        }

        // update output namme
        this->replaceOutputName(min, max, var, outputName, idx, tempStr, patternRegex);
        ++idx;

        infile.close();
    }

    return outputName;
}

string ExternalPattern::outputName(vector<Tuple>& vec){
    // if vector is passed, process using in memory version
    if(vec.size() != 0) return this->outputNameHelper(vec); 
    else return this->externalOutPutName(); // if no vector provided, iterate over all matched files

}

string ExternalPattern::inferPattern(const string& path, string& variables, const string& blockSize){
    FilesystemStream stream = FilesystemStream(path, true, blockSize, true); // create a stream from directory 

    vector<string> vec = stream.getBlock();
    for(auto& str: vec) str = s::getBaseName(str); // Get basename of each file
    string pattern = inferPatternInternal(vec, variables); // Get pattern from first block

    // while the stream is nonempty:
    //      process each block using the in memory version with the current pattern
    while(!stream.isEmpty()){
        vec = stream.getBlock();
        for(auto& str: vec) {
            replace(str.begin(), str.end(), '\\', '/');
        }
        for(auto& str: vec) cout << str << endl;
        //for(int i = 0; i < vec.size(); ++i) vec[i] = s::getBaseName

        pattern = inferPatternInternal(vec, variables, pattern);

    }

    fs::remove_all(stream.getTmpPath());
    return pattern;
}

int ExternalPattern::getGroupLength(){
    return this->currentGroup.size();
}

void ExternalPattern::sortFiles(){

}

Tuple ExternalPattern::getItem(int key){
    if(key >= this->this->steam.getValidFilesSize()) throw out_of_range("Index " + std::to_string(key) + is " out of range.");
    if(key < 0) return this->stream.getFileByIndex(this->steam.getValidFilesSize()+key);
    return this->stream.getFileByIndex(key);
}

vector<Tuple> ExternalPattern::getItemList(vector<int>& key){

    vector<Tuple> vec;

    int validFilesSize = this->stream.getValidFilesSize();

    for(const auto& index: key){
        if(index > validFilesSize) throw out_of_range("Index " + std::to_string(index) + " is out of range.");

        vec.push_back(this->stream.getFileByIndex(index));
    }

    return vec;
}

vector<Tuple> ExternalPattern::getSlice(vector<Types>& key){
    
    string key0 = s::to_string(key[0]);
    string key1 = s::to_string(key[1]);
    string key2 = s::to_string(key[2]);

    cout << "valid files length: " << this->stream.getValidFilesSize() << endl;

    if(s::is_number(key0) && key1 == "None"  && key2 == "None"){
        int i = stoi(key0);

        if(i >= this->stream.getValidFilesSize()) throw out_of_range("Index out of range.");
        int j = this->stream.getValidFilesSize();
        int step =  1;
        return this->stream.getValidFilesSlice(i, j, step);
    }

    // A start and stop index is provided with no step size, i.e. validFiles[i:j]
    if(s::is_number(key0) && s::is_number(key1)  && key2 == "None"){
        int i =  stoi(key0);
        int j = stoi(key1);

        if(i > this->stream.getValidFilesSize() || j > this->stream.getValidFilesSize()) throw out_of_range("Index out of range.");
        if(j >= 0 && i > j) throw out_of_range("Invalid range.");

        if(j < 0) j += this->stream.getValidFilesSize() + 1;

        return this->stream.getValidFilesSlice(i, j, 1);
    }

    // A start, stop, and step is provided
    if(s::is_number(key0) && s::is_number(key1)  && s::is_number(key2)){
        int i = stoi(key0);
        int j = stoi(key1);
        int step =  stoi(key2);
        return this->stream.getValidFilesSlice(i, j, step);
    }

    if(s::is_number(key0) && key1 == "None" && s::is_number(key2)){
        int i = stoi(key0);
        int j = this->stream.getValidFilesSize();
        int step =  stoi(key2);
        return this->stream.getValidFilesSlice(i, j, step);
    }
    
    cout << "Case not handled" << endl;
    vector<Tuple> empty;
    return empty;

}