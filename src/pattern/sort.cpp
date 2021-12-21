#include "sort.hpp"

using namespace std;

ExternalMergeSort::ExternalMergeSort(const Structure& structure, 
                                     const string& inputFileName, 
                                     const string& outputFileName, 
                                     const string& blockSize, 
                                     const string& variable="",
                                     const int mapSize=0) {

    this->inputFileName = inputFileName;
    this->outputFileName = outputFileName;
    this->sortVariable = variable;
    this->blockSizeStr = blockSize;
    this->blockSize = Block::parseblockSize(blockSize);
    this->mapSize = mapSize;
    tmpdir = fs::temp_directory_path();

    this->tmpdir += "/extern_sort_tmp/";
    if(fs::exists(tmpdir)){ 
        uintmax_t n = fs::remove_all(tmpdir);
    }

    bool created = fs::create_directory(tmpdir);
    if(!created) {
            throw invalid_argument("Error creating tmp directory");
    }

    if(structure == std_map){
        this->sortMapFile();
    } else {
        this->sortFile();
    }
    //uintmax_t n2 = fs::remove_all(tmpdir);
}


double ExternalMergeSort::currentSize(const vector_string& vec, const int& stringSize, const double& previousSize){
    return sizeof(string) + stringSize + previousSize;
}
    
void ExternalMergeSort::sortMapFile(){
    // Buffer size 1 Megabyte (or any number you like)
    int vectorCapacity = 0;
    int stringSize = sizeof(string);

    string str; 
    int blockNum = 0;
    string blockName;
    int count = 0;

    //FileStream stream = FileStream(inputFileName, this->blockSizeStr);
    //this->mapSize = stream.mapSize;
    ifstream infile(inputFileName);
    vector<Tuple> vec;

    while (true) {
        getMapBlock(infile, vec);
        if(vec.size() == 0){
            break;
        }
        
        sort(vec.begin(), vec.end(), 
            [&sortVariable = as_const(sortVariable)](Tuple& a, Tuple& b) {
                return get<0>(a)[sortVariable] < get<0>(b)[sortVariable];
        });

        //write to file
        blockName = tmpdir + to_string(blockNum) + ".txt";;
        ofstream out(blockName, ios_base::app);
        this->writeMapTmpFile(out, vec);

        count++;
        if(count > 100){
            throw runtime_error("Creating over 100 text files. Ending program");
        }

        filesToMerge.push_back(blockName);
        //clear vec and continue reading file   
        vec.clear();
        blockNum++;
    }
    //merge sorted files 
    
    this->mergeMaps();
}


bool ExternalMergeSort::getMapBlock(ifstream& infile, vector<Tuple>& vec){

    Tuple member;
    
    long size = sizeof(vector<Tuple>);

    Map map;
    string str;
    string key, value;
    int valueLength;
    size_t pos;
    Types result;
    map = this->tempMap;
    
    while(size < blockSize && infile >> str){
        
        if (map.size() == (this->mapSize)) {
            size += sizeof(map) + sizeof(vector<string>);
            
            //sizeof(Tuple) +
            for(const auto& item : map){
                size += item.first.length() + s::size(item.second);
            }
            get<0>(member) = map;
            str.pop_back(); // remove trailing comma
            get<1>(member).push_back(str);
            vec.push_back(member);
            map.clear();
            get<1>(member).clear();
            infile >> str;
        }

        pos = str.find(":");
        key = str.substr(0, pos);
        valueLength = str.length() - pos;
        value = str.substr(pos+1, valueLength);

        if(s::is_number(value)){
            result = stoi(value);
        } else {
            result = value;
        }

        map[key] = result;
        size += valueLength + pos;
    }
    this->tempMap = map;
    streampos ptr = infile.tellg();
    if(!(infile >> str)){
        return false;
    } else {
        infile.seekg(ptr, ios::beg);
        return true;
    }
    
}

void ExternalMergeSort::sortFile(){
    // Buffer size 1 Megabyte (or any number you like)
    int vectorCapacity = 0;
    int stringSize = sizeof(string);

    string str; 
    int blockNum = 0;
    string blockName;
    double previousSize = sizeof(vector<string>);
    int count = 0;

    ifstream file(this->inputFileName);
    if(!file.is_open()){ 
        throw invalid_argument("Cannot find file \"" + inputFileName + "\".");
    }

    while (getline(file, str)) {

        this->vec.push_back(str);
        previousSize = currentSize(vec, str.length(), previousSize);

        while(previousSize < this->blockSize && !file.eof()){
            getline(file,str);
            this->vec.push_back(str);
            previousSize = currentSize(vec, str.length(), previousSize);
        } 

        //sort vec
        auto start = chrono::high_resolution_clock::now();

        sort(this->vec.begin(), this->vec.end(), 
            [](const string& a, const string& b) {return a < b;});

        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
        cout << "Sort time: " << duration.count() << endl;

        //write to file
        blockName = to_string(blockNum) + ".txt";;
        this->writeTmpFile(blockName);

        count++;
        if(count > 100){
            throw runtime_error("Creating over 100 text files. Ending program");
        }

        filesToMerge.push_back(blockName);
        //clear vec and continue reading file   
        vec.clear();
        previousSize = sizeof(vector<string>);
        blockNum++;
    }

    //merge sorted files 
    this->merge();
}

void ExternalMergeSort::writeTmpFile(string& output){
    output = tmpdir + output;
    ofstream out(output);
    for (const auto& element: this->vec) out << element << "\n";
}

void ExternalMergeSort::writeMapTmpFile(ofstream& file, 
                                        const vector<Tuple>& vec){

    for(const auto& mapping: vec){
        for(const auto& element: get<0>(mapping)){
            file << element.first << ":" << s::to_string(element.second) << '\n';
        }

        for(const auto& element: get<1>(mapping)){
            file << element << "," << '\n';
        } 
    }
}

void ExternalMergeSort::writeFile(const string& output){
    ofstream out(output);
    for (const auto& element: this->vec) out << element << "\n";
}

void ExternalMergeSort::merge(){

    // case of 1 file
    string str1, str2;
    string outFile;
    int length;
    int count;
    int filesWritten = 0;
    int iter = 0;

    while(filesToMerge.size() > 1){

        length = filesToMerge.size();
        for(int i = 0; i < (length - (length%2)); i+=2){

            ifstream file1(filesToMerge[i]);
            ifstream file2(filesToMerge[i+1]);

            if(!file1.is_open()) {
                string error = "Cannot open file " + filesToMerge[i];
                throw runtime_error(error);
            }

            if(!file2.is_open()) {
                string error = "Cannot open file " + filesToMerge[i+1];
                throw runtime_error(error);
            }

            outFile = this->tmpdir + to_string(iter) + "_" + to_string(i) + "_" + to_string(i+1) + ".txt"; 
            newFiles.push_back(outFile);
            ofstream outfile(outFile, ios_base::app);

            if(filesWritten > 100){
                throw runtime_error("Creating over 100 text files. Ending program");
            }

            count = 0;
            while(true){
                if(count == 0) {
                    getline(file1, str1); 
                    getline(file2, str2);
                    count++;
                }

                // if file1 empty
                if(file1.eof()){
                    if(file2.eof()){
                        file2.close();
                        outfile.close();
                        break;
                    }
                    // write the rest of file2
                    outfile << str2 << '\n';
                    while(getline(file2, str2)) {
                        if (str2 != " " && str2 != ""){ 
                            outfile << str2 << '\n';
                        };
                    }

                    file1.close();
                    file2.close();
                    outfile.close();
                    break;

                } else if(file2.eof()){

                    // write the rest of file1
                    outfile << str1 << '\n';
                    while(getline(file1, str1)) {
                        if (str1 != " " && str1 != ""){ 
                            outfile << str1 << '\n';
                        }
                    }

                    file1.close();
                    file2.close();
                    outfile.close();
                    break; 

                } else if(str1 < str2){
                    //write str1 to output
                    if (str1 != " " && str1 != "") {
                        outfile << str1 << '\n';
                    }
                    getline(file1, str1); 
                } else {
                    if (str2 != " " && str2 != ""){
                        outfile << str2 << '\n';
                    }
                    getline(file2, str2);
                    //write str2 to output
                }
            }
            outfile.close();
            
        }
        iter++;
        filesToMerge = newFiles;
        newFiles.clear();
    }

    ifstream file(filesToMerge[0]);
    if(!file.is_open()){
        string error = "Cannot open file " + filesToMerge[0];
        throw runtime_error(error);
    }

    vec.clear();

    while (getline(file, str1)) {
        vec.push_back(str1);
    }
    writeFile(this->outputFileName);

}   

void ExternalMergeSort::mergeMaps(){

    // case of 1 file
    string outFile;
    int length;
    int iter = 0;
   
    while(filesToMerge.size() > 1){

        length = filesToMerge.size();
        for(int i = 0; i < (length - (length%2)); i+=2){
            outFile = this->tmpdir + to_string(iter) + "_" + to_string(i) + "_" + to_string(i+1) + ".txt";
            this->twoWayMergeMaps(filesToMerge[i], filesToMerge[i+1], outFile);
            newFiles.push_back(outFile);
        }

        if(length%2 == 1){
            outFile = outFile = this->tmpdir + to_string(iter) + "_" + to_string(length-1) + "_" + to_string(length-1) + ".txt";
            this->twoWayMergeMaps(filesToMerge[filesToMerge.size()-1], newFiles[0], outFile);
            newFiles[0] = outFile;
        }
        for(const auto& f: filesToMerge){
            cout << f <<", " ;
        }
        cout << endl;

        iter++;
        filesToMerge = newFiles;
        newFiles.clear();
    }
    
    ifstream file(filesToMerge[0]);
    if(!file.is_open()){
        string error = "Cannot open file " + filesToMerge[0];
        throw runtime_error(error);
    }

    Tuple map1, map2;
    ofstream outfile(this->outputFileName);
    while(getMap(file, map1)){
        writeMap(outfile, map1);
        get<1>(map1).clear();
    }
}  

void ExternalMergeSort::twoWayMergeMaps(const string& fileName1, const string& fileName2, const string& outFile){

    ifstream file1(fileName1);
    ifstream file2(fileName2);

    if(!file1.is_open()) {
        string error = "Cannot open file " + fileName1;
        throw runtime_error(error);
    }

    if(!file2.is_open()) {
        string error = "Cannot open file " + fileName2;
        throw runtime_error(error);
    }
 
    ofstream outfile(outFile);

    int count = 0;
    Tuple map1, map2;
    while(true){
        if(count == 0) {
            getMap(file1, map1); 
            getMap(file2, map2);
            count++;
        }

        // if file1 empty
        if(file1.eof()){
            cout << "eof1" << endl;
            //writeMap(outfile, map1); // write remaining map
            if(file2.eof()){
                cout << "both" << endl;
                file1.close();
                file2.close();
                outfile.close();
                break;
            }

            // write the rest of file2
            //getMap(file2, map2);
            writeMap(outfile, map2);
            while(getMap(file2, map2)) {
                for(const auto& e: get<0>(map2)){
                    cout << e.first << ":" << s::to_string(e.second) << endl;
                }
                cout << endl;
                writeMap(outfile, map2);
            }

            file1.close();
            file2.close();
            outfile.close();
            break;

        } else if(file2.eof()){
            cout << "eof2" << endl;
            // write the rest of file1
            //outfile << str1 << '\n';
            writeMap(outfile, map2);
            while(getMap(file1, map1)) {
                writeMap(outfile, map1);
            }

            file1.close();
            file2.close();
            outfile.close();
            break; 

        } else if(get<0>(map1)[this->sortVariable] <= get<0>(map2)[this->sortVariable]){
            //write str1 to output
            writeMap(outfile, map1);
            getMap(file1, map1); 
        } else {
            //write str2 to output
            writeMap(outfile, map2);
            getMap(file2, map2);
        }
    }
    outfile.close();
}

/**
* Modified method from Stream class
*/
bool ExternalMergeSort::getMap(ifstream& infile, Tuple& member){
    string str;
    Map map;

    string key, value;
    int valueLength;
    size_t pos;

    get<1>(member).clear();
    while(getline(infile, str)){

        if (map.size() == (this->mapSize)) {
            //size += sizeof(map) + sizeof(vector<string>);
            
            //sizeof(Tuple) +
            //for(const auto& item : map){
            //    size += item.first.length() + item.second.length();
            //}
            get<0>(member) = map;
            str.pop_back();
            get<1>(member).push_back(str);
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

/**
* Modified method from Stream class
*/
void ExternalMergeSort::writeMap(ofstream& file, Tuple& mapping){
    //counter++;
    //ofstream file(filename, ios_base::app);

    for(const auto& element: get<0>(mapping)){
        file << element.first << ":" << s::to_string(element.second) << '\n';
    }

    for(const auto& element: get<1>(mapping)){
        file << element << "," << '\n';
    } 
    //file << '\n';

    //file.close();
}


/*
int main(int argc, char** argv){

    string::size_type sz;
    int num = stoi(argv[1], &sz);
    string input = "test";
    input += argv[1];
    input += ".txt";

    string output = "output";
    output += argv[1];
    output += ".txt";

    auto start = chrono::high_resolution_clock::now();
    ExternalMergeSort ems = ExternalMergeSort(input, output);
    
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "The Total time: " << duration.count() << endl;
    exit(0);
    }
*/