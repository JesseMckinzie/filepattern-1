#include "InternalPattern.hpp"

using namespace std;

void InternalPattern::groupBy(const string& groupBy) {

    validGroupedFiles.clear();
    Tuple member;
    
    // Sort the matched files by the groupBy parameter 
    sort(validFiles.begin(), validFiles.end(), [&groupBy = as_const(groupBy)](Tuple& p1, Tuple& p2){
        return get<0>(p1)[groupBy] < get<0>(p2)[groupBy];
    });

    Types currentValue = get<0>(validFiles[0])[groupBy]; // get the value of variable
    vector<Tuple> emptyVec;
    int i = 0;
    int group_ptr = 0;

    //group files into vectors based on groupBy variable (to be removed)
    while(i < this->validFiles.size()){
        this->validGroupedFiles.push_back(emptyVec);
        while(std::get<0>(this->validFiles[i])[groupBy] == currentValue) {
            this->validGroupedFiles[group_ptr].push_back(this->validFiles[i]);

            sort(validGroupedFiles[group_ptr].begin(), validGroupedFiles[group_ptr].end(), [](Tuple& m1, Tuple& m2){
                return get<0>(m1)["file"] < get<0>(m2)["file"];
            });

            ++i;
            if (i >= this->validFiles.size()) break;
        }

        if (i < this->validFiles.size()) currentValue = get<0>(this->validFiles[i])[groupBy];
        ++group_ptr;
    }
}


vector<Tuple> InternalPattern::getMatching(string& variables){

    //remove spaces if present
    variables.erase(std::remove_if(variables.begin(), variables.end(), ::isspace), variables.end());
    
    //split on commas
    vector<string> splitVaraibles = s::split(variables, ",");
    vector<std::pair<string,string>> variableValues;

    std::pair<string, string> pair;
    size_t position;
    // Parse input into variables and values (to be updated)
    for(const auto& variable: splitVaraibles) {
        position = variable.find("=");
        pair.first = variable.substr(0, position);
        pair.second = variable.substr(position+1);
        if(!get<0>(validFiles[0]).count(pair.first)) throw invalid_argument("Variable must be in file pattern.");
        variableValues.push_back(pair);
    }

    vector<Tuple> matching;

    bool match;
    Types temp;
    //Iterate over files, pushing to return vector if the variable matches the input
    for(auto& file: this->validFiles){
        match = true;
        for(const auto& variable: variableValues) {
            temp = get<0>(file)[variable.first];
            if(!(s::to_string(temp) == variable.second)) match = false; 
        }
        if(match) matching.push_back(file);
    }

    return matching;
    
}