#include "internal_pattern.hpp"

using namespace std;

void InternalPattern::groupByHelper(const vector<string>& groups){

    std::vector<std::pair<std::vector<std::pair<std::string, Types>> , std::vector<Tuple>>> temp;
    int groupIdx;
    vector<Tuple> tempVec;
    vector<std::pair<std::string, Types>> grouped_variables;
    for(const auto& groupBy: groups){

        groupIdx = 0;
        
        for(auto& vec: validGroupedFiles){
            grouped_variables.clear();
            for(auto& g: vec.first) grouped_variables.push_back(g);
            // Sort the matched files by the groupBy parameter 
            sort(vec.second.begin(), vec.second.end(), [&groupBy = as_const(groupBy)](Tuple& p1, Tuple& p2){
                return get<0>(p1)[groupBy] < get<0>(p2)[groupBy];
            });

            Types currentValue = get<0>(vec.second[0])[groupBy]; // get the value of variable
            vector<Tuple> emptyVec;
            int i = 0;
            int group_ptr = 0;

            //group files into vectors based on groupBy variable 
            while(i < vec.second.size()){

                //this->validGroupedFiles.push_back(emptyVec);
                //temp.push_back(make_pair(make_pair(groupBy, currentValue), emptyVec));
                while(std::get<0>(vec.second[i])[groupBy] == currentValue) {
                    //temp[group_ptr].second.push_back(vec.second[i]);
                    tempVec.push_back(vec.second[i]);

                    // sort group of variables
                    //sort(temp[group_ptr].second.begin(), temp[group_ptr].second.end(), [](Tuple& m1, Tuple& m2){
                    //    return get<1>(m1)[0] < get<1>(m2)[0];
                    //});

                    ++i;
                    if (i >= vec.second.size()) break;
                }

                // sort group of variables
                //sort(temp[group_ptr].second.begin(), temp[group_ptr].second.end(), [](Tuple& m1, Tuple& m2){
                //   return get<1>(m1)[0] < get<1>(m2)[0];
                //});
                /*
                cout << "tempVec: ";
                for(const auto& v: tempVec){
                    for(auto& p: get<0>(v)){
                        cout << p.first << ", " << s::to_string(p.second) << ", ";
                    }
                    for(auto& p: get<1>(v)){
                        cout << p << endl;
                    }
                } 
                */
                grouped_variables.push_back(make_pair(groupBy, currentValue));
                temp.push_back(make_pair(grouped_variables, tempVec)); 
                sort(temp[group_ptr].second.begin(), temp[group_ptr].second.end(), [](Tuple& m1, Tuple& m2){
                    return get<1>(m1)[0] < get<1>(m2)[0];
                });
                tempVec.clear(); 

                if (i < vec.second.size()){
                     currentValue = get<0>(vec.second[i])[groupBy];
                     grouped_variables.pop_back();
                }
                ++group_ptr;
            }

            // insert into group  
            /*          
            cout << "temp: " << endl;
            for(const auto& v: temp){
                for(const auto& p: get<0>(v)){
                    cout << p.first << s::to_string(p.second) << ", ";
                }
                for(const auto& p: get<1>(v)){
                    cout << p << " " << endl;
                }
            }
            cout << endl;
            */
            
        }
        validGroupedFiles = temp;
        /*
        cout << "temp: ";
        for(const auto& vecc: temp){
            cout << endl;
            for(const auto& v: vecc.second){
                for(const auto& p: get<0>(v)){
                    cout << p.first << s::to_string(p.second) << ", ";
                }
                for(const auto& p: get<1>(v)){
                    cout << p << " " << endl;
                }
            }
            cout << endl;
        }
        */

        temp.clear();
    }
}


void InternalPattern::groupBy(vector<string>& groups) {
    vector<std::pair<std::string, Types>> grouped_variables;
    this->setGroup(groups[0]);
    validGroupedFiles.clear();
    Tuple member;
    
    string groupBy = groups[0];
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
        grouped_variables.clear();
        grouped_variables.push_back(make_pair(groupBy, currentValue));
        this->validGroupedFiles.push_back(make_pair(grouped_variables, emptyVec));
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

    groups.erase(groups.begin());
    this->groupByHelper(groups);
    
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
    if(key < 0) {
        if(this->validFiles.size() + key < 0) throw out_of_range("Index " + std::to_string(key) + " is out of range.");
        return this->validFiles[validFiles.size()+key];
    }

    if(key >= this->validFiles.size()) throw out_of_range("Index " + std::to_string(key) + " is out of range.");

    return this->validFiles[key];
}

vector<Tuple> InternalPattern::getItemList(vector<int> key){

    vector<Tuple> vec;

    int validFilesSize = validFiles.size();

    for(const auto& index: key){
        if(index < 0) {
            if(validFilesSize + index < 0) throw out_of_range("Index " + std::to_string(index) + " is out of range.");
            vec.push_back(validFiles[validFilesSize+index]);
        } else {
            if(index > validFilesSize) throw invalid_argument("Index " + std::to_string(index) + " is out of range.");
            vec.push_back(validFiles[index]);
        }
    }

    return vec;
}

vector<Tuple> InternalPattern::getSlice(vector<Types>& key){
    
    string key0 = s::to_string(key[0]);
    string key1 = s::to_string(key[1]);
    string key2 = s::to_string(key[2]);

    int validFilesSize = validFiles.size();

    if(s::is_number(key0) && key1 == "None"  && key2 == "None"){
        int i = stoi(key0);
        if(i >= validFilesSize) throw out_of_range("Index " + std::to_string(i) + " is out of range.");
        
        int j = validFilesSize;
        int step =  1;

        return v::sliceVector(this->validFiles, i, j, step);
    }

    // A start and stop index is provided with no step size, i.e. validFiles[i:j]
    if(s::is_number(key0) && s::is_number(key1)  && key2 == "None"){
        int i =  stoi(key0);
        int j = stoi(key1);

        if(i > validFilesSize) throw out_of_range("Index " + std::to_string(i) + " is out of range.");
        if(j > validFilesSize) throw out_of_range("Index " + std::to_string(j) + " is out of range.");
        if(j >= 0 && i > j) throw out_of_range("Invalid range.");

        if(j < 0) j += validFilesSize + 1;

        return v::sliceVector(this->validFiles, i, j, 1);
    }

    // A start, stop, and step is provided
    if(s::is_number(key0) && s::is_number(key1)  && s::is_number(key2)){
        int i = stoi(key0);
        int j = stoi(key1);

        if(i > validFilesSize) throw out_of_range("Index " + std::to_string(i) + " is out of range.");
        if(j > validFilesSize) throw out_of_range("Index " + std::to_string(j) + " is out of range.");

        int step =  stoi(key2);
        return v::sliceVector(this->validFiles, i, j, step);
    }

    if(s::is_number(key0) && key1 == "None" && s::is_number(key2)){
        int i = stoi(key0);
        if(i > validFilesSize) throw out_of_range("Index " + std::to_string(i) + " is out of range.");

        int j = validFilesSize;
        int step =  stoi(key2);

        return v::sliceVector(this->validFiles, i, j, step);
    }
  
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