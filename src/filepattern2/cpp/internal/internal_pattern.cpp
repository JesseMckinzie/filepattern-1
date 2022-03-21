#include "internal_pattern.hpp"

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

    if(s::endsWith(path, ".txt")){

        ifstream infile(path);

        string str;
        while(getline(infile, str)) vec.push_back(str);

    } else {

        fs::directory_iterator iterator = fs::directory_iterator(path);

        for(auto& file: iterator){
            vec.push_back(s::getBaseName(file.path().string()));
        }
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

Tuple InternalPattern::getItem(int key){
    if(key < 0) return this->validFiles[validFiles.size()+key];
    return this->validFiles[key];
}

vector<Tuple> InternalPattern::getItemList(vector<int> key){

    vector<Tuple> vec;

    int validFilesSize = validFiles.size();

    for(const auto& index: key){
        if(index > validFilesSize) throw invalid_argument("Index " + std::to_string(index) + " is out of range.");

        vec.push_back(validFiles[index]);
    }

    return vec;
}

vector<Tuple> InternalPattern::getSlice(vector<Types>& key){
    
    string key0 = s::to_string(key[0]);
    string key1 = s::to_string(key[1]);
    string key2 = s::to_string(key[2]);

    // A start and stop index is provided with no step size, i.e. validFiles[i:j]
    if(s::is_number(key0) && s::is_number(key1)  && key2 == "None"){
        int i =  stoi(key0);
        int j = stoi(key1);

        cout << "size: " << validFiles.size() << endl;

        cout << "i: " << i << endl;

        cout << "j: " << j << endl;

        if(i > validFiles.size() || (j > validFiles.size() && j >=0)) throw invalid_argument("Index out of range.");
        if(i > j && j >= 0) throw invalid_argument("Invalid range.");

        if(j < 0) j += validFiles.size() + 1;

        return v::sliceVector(this->validFiles, i, j, 1);
    }

    // A start, stop, and step is provided
    if(s::is_number(key0) && s::is_number(key1)  && s::is_number(key2)){
        int i = stoi(key0);
        int j = stoi(key1);
        int step =  stoi(key2);
        return v::sliceVector(this->validFiles, i, j, step);
    }

    if(s::is_number(key0) && key1 == "None" && s::is_number(key2)){
        int i = stoi(key0);
        int j = validFiles.size();
        int step =  stoi(key2);
        return v::sliceVector(this->validFiles, i, j, step);
    }
    
    cout << "Error in InternalPattern::getSlize" << endl;
    vector<Tuple> empty;
    return empty;

}



/*
vector<Tuple> InternalPattern::getFilesFromOutputName(const string& outputName, const vector<string>& vars) {

    regex outputGroup("\([0-9a-zA-Z]+)-[0-9a-zA-Z]+\)");

    smatch sm;
    if(regex_search(outputName, sm, outputGroup)){
        // find any named groups with regex style naming
        cout << "sm: " << endl;

        for(const auto& match: sm){
            cout << match << endl;
        }
        cout << endl;
    }
}
*/