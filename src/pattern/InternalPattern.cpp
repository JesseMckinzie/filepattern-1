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

// "x=[1,2,3]"
/*
void InternalPattern::getMatchingHelper(string t, string... args){
    t.erase(remove(t.begin(), t.end(), ' '), t.end());
    vector<string> parsed = s::split(t, "=");
    vector<string> values;

    if(parsed.size() > 2) throw invalid_argument("Input string must be in format \"<variable>=[<value1>, <value2>, ..., <valuen>]");

    string current;
    for(const auto& c: parsed[1]){
        if(c == '[' || c == ']') continue;
        else if(c == ',') {
            values.push_back(current);
            current = "";
        }
        else current.push_back(c);
    }

    for(const auto& file: this->validfiles){
        for(const auto& value: values){  
            if(s::to_string(get<0>(file)[parsed[0]]) == value){
                this->matching.push_back(file);
            }
        }
    }

    getMatchingHelper(args...);
}

vector<Tuple> InternalPattern::getMatching(string t, string... args){

    this->matching.clear();
    this->variableMapping.clear();

    this->getMatchingHelper(t, args...);

    return matching;
    
}
*/

void InternalPattern::getMatchingLoop(vector<Tuple>& iter, 
                                      const string& variable, 
                                      const vector<Types>& values, 
                                      Types& temp){
    for(auto& file: iter){
        temp = get<0>(file)[variable];
        for(const auto& value: values){  
            if(temp == value){
                this->matching.push_back(file);
            }
        }
    }
}

// "x=[1,2,3]"
void InternalPattern::getMatchingHelper(const tuple<string, vector<Types>>& variableMap){
    string variable = get<0>(variableMap);
    vector<Types> values = get<1>(variableMap);

    if(find(begin(variables), end(variables), variable) == end(variables)) {
        throw invalid_argument("\"" + variable + "\" is not a variable. Use a variable that is contained in the pattern.");
    }

    Types temp;
    vector<Tuple> iter;
    if(this->matching.size() == 0) {    
        this->getMatchingLoop(this->validFiles, variable, values, temp);
    } else {
        iter = this->matching;
        this->matching.clear();
        this->getMatchingLoop(iter, variable, values, temp);
    }
}

/*
input is in the form "x=[0,1,2]; y=[0,1]"
*/
vector<Tuple> InternalPattern::getMatching(const vector<tuple<string, vector<Types>>>& variables){

    this->matching.clear();

    for(const auto& variableMap: variables){
        this->getMatchingHelper(variableMap);
    }

    return matching;
}

/*
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
*/
