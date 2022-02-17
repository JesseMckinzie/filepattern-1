#include "InternalPattern.hpp"

using namespace std;

void InternalPattern::groupBy(const string& groupBy) {
    this->setGroup(groupBy);
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

    //group files into vectors based on groupBy variable 
    while(i < this->validFiles.size()){
        //this->validGroupedFiles.push_back(emptyVec);
        this->validGroupedFiles.push_back(make_pair(make_pair(groupBy, currentValue), emptyVec));
        while(std::get<0>(this->validFiles[i])[groupBy] == currentValue) {
            this->validGroupedFiles[group_ptr].second.push_back(this->validFiles[i]);

            // sort group of variables
            sort(validGroupedFiles[group_ptr].second.begin(), validGroupedFiles[group_ptr].second.end(), [](Tuple& m1, Tuple& m2){
                return get<1>(m1)[0] < get<1>(m2)[0];
            });

            ++i;
            if (i >= this->validFiles.size()) break;
        }

        if (i < this->validFiles.size()) currentValue = get<0>(this->validFiles[i])[groupBy];
        ++group_ptr;
    }
}

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

void InternalPattern::getMatchingHelper(const tuple<string, vector<Types>>& variableMap){
    string variable = get<0>(variableMap); // get key from argument
    vector<Types> values = get<1>(variableMap); // get value from argument

    // throw error if argument variable is not in the pattern
    if(find(begin(variables), end(variables), variable) == end(variables)) {
        throw invalid_argument("\"" + variable + "\" is not a variable. Use a variable that is contained in the pattern.");
    }

    Types temp;
    vector<Tuple> iter;
    // if first or only variable to match, iterate over valid files
    if(this->matching.size() == 0) {    
        this->getMatchingLoop(this->validFiles, variable, values, temp);
    } else { // iterate files that matched previous call
        iter = this->matching;
        this->matching.clear();
        this->getMatchingLoop(iter, variable, values, temp);
    }
}

vector<Tuple> InternalPattern::getMatching(const vector<tuple<string, vector<Types>>>& variables){

    this->matching.clear();

    // match files for each argument
    for(const auto& variableMap: variables){
        this->getMatchingHelper(variableMap);
    }

    return matching;
}

string InternalPattern::outputName(vector<Tuple>& vec){
    return this->outputNameHelper(vec);
}

string InternalPattern::inferPattern(const string& path, string& variables){
    vector<string> vec;

    fs::directory_iterator iterator = fs::directory_iterator(path);
    string filePath;
    for(auto& file: iterator){
        vec.push_back(s::getBaseName(file.path()));
    }

    return inferPatternInternal(vec, variables);
}

string InternalPattern::inferPattern(vector<string>& vec, string& variables){
    return inferPatternInternal(vec, variables);
}

void InternalPattern::sortFiles(){
    sort(this->validFiles.begin(), this->validFiles.end(), [](Tuple& m1, Tuple& m2){
        return get<1>(m1)[0] < get<1>(m2)[0];
    });
}