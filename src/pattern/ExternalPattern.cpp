#include "ExternalPattern.hpp"

using namespace std;

namespace fs = std::filesystem;

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
    string tmpdir = fs::temp_directory_path();
    tmpdir += "/filepattern/";

    if(fs::exists(tmpdir)){
        fs::remove_all(tmpdir);
    }

    bool created = fs::create_directory(tmpdir);

    //if (!created) {
    //    throw runtime_error("Could not create temporary file.");
    //}

    this->matching = tmpdir + "matching.txt";
    this->matchingCopy = tmpdir + "/temp.txt";
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
    //if(!matchingStream.is_open()) throw runtime_error("The get matching function must be called first.");

    long size = sizeof(vector<Tuple>);
    if(size > this->blockSize) throw runtime_error("The block size is smaller than the size of a vector. The block size must be increased");

    Tuple temp;
    vector<Tuple> vec;
    bool moreFiles;

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