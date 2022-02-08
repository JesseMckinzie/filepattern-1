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
    tmpdir = fs::temp_directory_path(); // Find temporary directory

    this->tmpdir += "/extern_sort_tmp_" + s::getTimeString() + "/"; // temp directory
    // remove files from directory if already exits
    if(fs::exists(tmpdir)){ 
        uintmax_t n = fs::remove_all(tmpdir); 
    }
    // Create temp directory
    bool created = fs::create_directory(tmpdir);
    if(!created) {
            throw invalid_argument("Error creating tmp directory");
    }

    // Call function based on data type
    if(structure == std_map){
        this->sortMapFile();
    } else {
        this->sortFile();
    }

    uintmax_t n2 = fs::remove_all(tmpdir); // Remove temp directory 
}


double ExternalMergeSort::currentSize(const int& stringSize, 
                                      const double& previousSize){
    return sizeof(string) + stringSize + previousSize; // update amount of memory being used
}
    
void ExternalMergeSort::sortMapFile(){

    int vectorCapacity = 0;
    int stringSize = sizeof(string);

    int blockNum = 0;
    string blockName;

    ifstream infile(inputFileName);
    vector<Tuple> vec;

    // Get blocks of files and sort while not end of file
    while (true) {
        getMapBlock(infile, vec); 
        if(vec.size() == 0){
            break;
        }
        // sort block
        sort(vec.begin(), vec.end(), 
            [&sortVariable = as_const(sortVariable)](Tuple& a, Tuple& b) {
                return get<0>(a)[sortVariable] < get<0>(b)[sortVariable];
        });

        //write to file
        blockName = tmpdir + to_string(blockNum) + ".txt";;
        ofstream out(blockName, ios_base::app);
        this->writeMapTmpFile(out, vec);

        filesToMerge.push_back(blockName);
        //clear vec and continue reading file   
        vec.clear();
        blockNum++;
    }

    //merge sorted files 
    this->mergeMaps();
}

/*This function could be made cleaner by writing each map to a single line in the text file*/
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
    // while the memory consumption is under blockSize and infile still has a line
    while(size < blockSize && infile >> str){
        
        // if map is full, add to block
        if (map.size() == (this->mapSize)) {
            size += sizeof(map) + sizeof(vector<string>);
            
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
        // split line on : (line is in the form "key:value")
        pos = str.find(":");
        key = str.substr(0, pos);
        valueLength = str.length() - pos;
        value = str.substr(pos+1, valueLength);

        // Conserve variable type (string or int)
        if(s::is_number(value)){
            result = stoi(value);
        } else {
            result = value;
        }

        map[key] = result; // add key value pair to map
        size += valueLength + pos;
    }

    this->tempMap = map;

    // check if end of file
    streampos ptr = infile.tellg();
    if(!(infile >> str)){
        return false;
    } else {
        infile.seekg(ptr, ios::beg);
        return true;
    }
    
}

void ExternalMergeSort::sortFile(){

    string str; 
    int blockNum = 0;
    string blockName;
    double previousSize = sizeof(vector<string>);

    ifstream file(this->inputFileName);
    if(!file.is_open()){ 
        throw invalid_argument("Cannot find file \"" + inputFileName + "\".");
    }

    // while the text file still has files:
    //      read in a block of files and sort and then write to output file
    while (getline(file, str)) {

        this->vec.push_back(str); // add filename to block
        previousSize = currentSize(str.length(), previousSize); // update amount of memory used

        // check if the amount of memory is below the specified amount and end of file hasn't been reached
        while(previousSize < this->blockSize && !file.eof()){
            // add more filenames to block
            getline(file,str);
            this->vec.push_back(str);
            previousSize = currentSize(str.length(), previousSize);
        } 

        // sort the block
        sort(this->vec.begin(), this->vec.end(), 
            [](const string& a, const string& b) {return a < b;});

        //write to file
        blockName = to_string(blockNum) + ".txt";;
        this->writeTmpFile(blockName);

        // add txt file name to vector to be merged 
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
    
    //loop over map, writing the key value pair in the form "key:value\n"
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
    // write string to output file
    ofstream out(output);
    for (const auto& element: this->vec) out << element << "\n";
}

void ExternalMergeSort::merge(){

    string str1, str2;
    string outFile;
    int length;
    int count;
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
    while(m::getMap(file, map1, this->mapSize)){
        m::writeMap(outfile, map1);
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
        // if first iteration, read in a map from both files
        if(count == 0) {
            m::getMap(file1, map1, this->mapSize); 
            m::getMap(file2, map2, this->mapSize);
            count++;
        }

        // if file1 empty
        if(file1.eof()){
            // if file2 is also empty
            if(file2.eof()){
                //close files and break
                file1.close();
                file2.close();
                outfile.close();
                break;
            }

            // write the rest of file2
            m::writeMap(outfile, map2);
            while(m::getMap(file2, map2, this->mapSize)) {
                m::writeMap(outfile, map2);
            }

            //close files
            file1.close();
            file2.close();
            outfile.close();
            break;

        } else if(file2.eof()){
            // write the rest of file1
            m::writeMap(outfile, map2);
            while(m::getMap(file1, map1, this->mapSize)) {
                m::writeMap(outfile, map1);
            }

            //close files
            file1.close();
            file2.close();
            outfile.close();
            break; 

        } else if(get<0>(map1)[this->sortVariable] <= get<0>(map2)[this->sortVariable]){
            //write str1 to output
            m::writeMap(outfile, map1);
            m::getMap(file1, map1, this->mapSize); 
        } else {
            //write str2 to output
            m::writeMap(outfile, map2);
            m::getMap(file2, map2, this->mapSize);
        }
    }
    outfile.close();
}