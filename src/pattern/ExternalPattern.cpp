#include "ExternalPattern.hpp"

using namespace std;

namespace fs = std::filesystem;

ExternalPattern::ExternalPattern(const string& path, const string& blockSize, bool recursive):
stream(FilesystemStream(path, recursive, blockSize)){}

void ExternalPattern::getMatchingLoop(ifstream& infile, 
                                      ofstream& outfile,
                                      const string& variable, 
                                      const vector<Types>& values, 
                                      Types& temp,
                                      Tuple& tempMap){

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
        fclose(fopen(matching.c_str(), "w"));

        ifstream in(matchingCopy); 
        ofstream out(matching);
        this->getMatchingLoop(in, out, variable, values, temp, tempMap);
    }
}

string ExternalPattern::getMatching(const vector<tuple<string, vector<Types>>>& variables){
    this->fp_tmpdir = fs::temp_directory_path();
    this->fp_tmpdir += "/filepattern_" + s::getTimeString() + "/";

    if(fs::exists(fp_tmpdir)){
        fs::remove_all(fp_tmpdir);
    }

    bool created = fs::create_directory(fp_tmpdir);

    //if (!created) {
    //    throw runtime_error("Could not create temporary file.");
    //}

    this->matching = fp_tmpdir + "matching.txt";
    this->matchingCopy = fp_tmpdir + "/temp.txt";
    if(fs::exists(matching)) {

        fs::remove(matching);
    }

    for(const auto& variableMap: variables){
        this->getMatchingHelper(variableMap, matching);
    }

    this->matchingStream.open(this->matching);

    return matching;
}

vector<Tuple> ExternalPattern::getMatchingBlock(){

    long size = sizeof(vector<Tuple>);
    if(size > this->blockSize) throw runtime_error("The block size is smaller than the size of a vector. The block size must be increased");

    Tuple temp;
    vector<Tuple> vec;
    bool moreFiles;

    if(!matchingStream.is_open()) return vec;

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

    if(stream.endOfValidFiles()){
        std::vector<Tuple> empty;
        return empty;
    }

    return stream.getValidFilesBlock();

}

void ExternalPattern::groupBy(const string& groupBy) {
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

string ExternalPattern::externalOutPutName(){
    string outputName = this->filePattern;
    regex patternRegex(this->regexFilePattern);

    std::ifstream infile;
    
    Tuple temp, min, max;
    int idx = 0;
    string tempStr;
    for(const auto& var: variables){
        infile.open(this->validFilesPath);

        m::getMap(infile, temp, this->mapSize);
        min = temp; 
        max = temp;

        while(m::getMap(infile, temp, this->mapSize)){
            if(get<0>(temp)[var] < get<0>(min)[var]) min = temp;
            if(get<0>(temp)[var] > get<0>(max)[var]) max = temp;
        }

        this->replaceOutputName(min, max, var, outputName, idx, tempStr, patternRegex);
        ++idx;

        infile.close();
    }

    return outputName;
}

string ExternalPattern::outputName(vector<Tuple>& vec){
    if(vec.size() != 0) return this->outputNameHelper(vec);
    else return this->externalOutPutName();

}

string ExternalPattern::inferPattern(const string& path, string& variables, const string& blockSize){
    FilesystemStream stream = FilesystemStream(path, true, blockSize, true);

    vector<string> vec = stream.getBlock();
    for(auto& str: vec) str = s::getBaseName(str);
    string pattern = inferPatternInternal(vec, variables);

    while(!stream.isEmpty()){
        vec = stream.getBlock();
        for(auto& str: vec) str = s::getBaseName(str);

        pattern = inferPatternInternal(vec, variables, pattern);

    }
    
    return pattern;
}

int ExternalPattern::getGroupLength(){
    return this->currentGroup.size();
}