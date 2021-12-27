#include "ExternalPattern.hpp"

using namespace std;

namespace fs = std::filesystem;

void ExternalPattern::setGroup(const string& group){
    this->group = group;
}

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
    if(fs::file_size(matching) == 0) {
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

    bool created = fs::create_directory(tmpdir);

    //if (!created) {
    //    throw runtime_error("Could not create temporary file.");
    //}

    this->matching = tmpdir + "/matching.txt";
    this->matchingCopy = tmpdir + "/temp.txt";

    for(const auto& variableMap: variables){
        this->getMatchingHelper(variableMap, matching);
    }

    return matching;
}
