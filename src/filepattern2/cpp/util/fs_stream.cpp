#include "fs_stream.hpp"

using namespace std;

FilesystemStream::FilesystemStream(const string& path, bool recursive, const string& blockSize, const bool isInfer) {
    this->validFilesSize = 0;
    this->isInfer = isInfer; // Is a call from inferPattern (handles memory footprint calculation different if true)
    this->tmpdir = fs::temp_directory_path().string(); // temp directory to store txt files
    if (s::endsWith(tmpdir, "\\")) this->tmpdir.pop_back();
    this->tmpdir += slash + "fs_stream_tmp_" + s::getTimeString() + slash; // path to temp directory
    this->blockSizeStr = blockSize; // string version of blockSize
    this->blockSize = Block::parseblockSize(blockSize); // parse string to long

    this->empty = false; // no more files
    this->validFiles = tmpdir + "validFiles.txt"; // path to txt file to store matched files
    this->counter = 0;

    bool created = fs::create_directory(tmpdir); // create temp directory
    //throw error if temp directory cannot be created
    if (!created) {
        throw runtime_error("Could not create temporary file.");
    }
    fs::permissions(this->tmpdir, fs::perms::all);

    this->outName = tmpdir + slash + "temp.txt";
    this->infile.open(validFiles);
    this->infile.close();

    this->recursive = false;

    try {
        // handle text file or directory based on path name
        if (s::endsWith(path, ".txt")) {
            this->txtInput = true;
            this->inputfile.open(path);
            if (!inputfile.is_open()) {
                throw invalid_argument("Invalid path \"" + path + "\".");
            }
        }
        else {
            txtInput = false;
            this->recursive_directory_iterator = fs::recursive_directory_iterator(path);
            this->rec_end = fs::end(recursive_directory_iterator);
            this->directory_iterator = fs::directory_iterator(path); // store iterator for target directory
            this->end = fs::end(directory_iterator);
        }
    }
    catch (const std::runtime_error& e) {
        string error = "No directory found. Invalid path \"" + path + "\".";
        throw std::runtime_error(error);
    }
}

FilesystemStream::~FilesystemStream() {
    this->infile.close();
    d::remove_dir(this->tmpdir);
}

vector<string> FilesystemStream::getBlock(){
    if(txtInput){
        return this->getBlockTxt();
    } else {
        return this->getBlockIterator();
    }
}

void FilesystemStream::updateSize(long& size, const string& current){
    if(isInfer){
        size +=  sizeof(vector<vector<int>>) + current.length()*sizeof(vector<int>) + 2*current.length()*sizeof(int);
    }
    if(size > this->blockSize) throw runtime_error("The block size is too small. Block size must be increased.");
}


vector<string> FilesystemStream::getBlockIterator(){

    vector<string> vec; // files to return
    long previousSize = sizeof(vector<string>); //memory used

    string current;

    if(this->recursive){

        try {
            current = (*recursive_directory_iterator).path().string();
        } catch (exception& e){
            cout << e.what() << endl;
        }

        this->updateSize(previousSize, current);

        while(this->currentSize(current.length(), previousSize) < blockSize){
            vec.push_back(current);
            ++this->validFilesSize;
            ++recursive_directory_iterator;

            if(fs::begin(recursive_directory_iterator) == rec_end){
                empty = true;
                break;
            } else {  
                previousSize = this->currentSize(current.length(), previousSize);

                try{
                    current = (*recursive_directory_iterator).path().string();
                } catch (exception& e){
                    cout << e.what() << endl;
                }
            }
        }
    } else{ 
        try {
            
            current = (*this->directory_iterator).path().string();
        } catch (exception& e){
            cout << e.what() << endl;
        }

        this->updateSize(previousSize, current);

        while(this->currentSize(current.length(), previousSize) < blockSize){
            vec.push_back(current);
            ++this->validFilesSize;
            ++directory_iterator;

            if(fs::begin(directory_iterator) == end){
                empty = true;
                break;
            } else {
                previousSize = this->currentSize(current.length(), previousSize);

                try{
                    current = (*directory_iterator).path().string();
                } catch (exception& e){
                    cout << e.what() << endl;
                }
            }
        }

    }

    return vec;
}

vector<string> FilesystemStream::getBlockTxt(){

    vector<string> vec;
    long size = sizeof(vector<string>);
    string str;
    // get string while less than block size
    this->inputfile >> str;
    this->updateSize(size, str);
    vec.push_back(str);
    ++this->validFilesSize;
    size = this->currentSize(str.length(), size);

    while(size < this->blockSize && this->inputfile >> str){
        size = this->currentSize(str.length(), size);
        ++this->validFilesSize;
        vec.push_back(str);
    }
    
    //check if end of file
    streampos ptr = inputfile.tellg();
    if(!(this->inputfile >> str)){
        this->empty = true;
    }
    inputfile.seekg(ptr, ios::beg);
    
    return vec;
}

string FilesystemStream::getTmpPath(){
    return this->tmpdir;
}


void FilesystemStream::writeBlock(const vector<string>& vec){
    ofstream file(this->outName, ios_base::app);

    for(const auto& element: vec){
        file << '\n' << element;
    }
    file.close();
}

void FilesystemStream::writeValidFiles(const Tuple& mapping){
    counter++;
    ofstream file(validFiles, ios_base::app);
  
    for(const auto& element: get<0>(mapping)){
        file << element.first << ":" << s::to_string(element.second) << '\n';
    }

    for(const auto& element: get<1>(mapping)){
        file << element << ",";
    } 

    file << '\n';
    file.close();
    validFilesEmpty = false;
    if(counter == 1){
        this->mapSize = get<0>(mapping).size();
        this->infile.open(validFiles);
    }
    
}

vector<Tuple> FilesystemStream::getValidFilesBlock(){

    if(this->validFilesEmpty){
        vector<Tuple> empty;
        return empty; 
    }

    vector<Tuple> vec;
    Tuple member;
    
    long size = sizeof(vector<Tuple>);

    Map map;
    string str;
    string key, value;
    int valueLength; 
    size_t pos;
    Types result;
    map = this->tempMap;
    
    while(size < blockSize && this->infile >> str){
        
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
    
    streampos ptr = infile.tellg();
    if(!(this->infile >> str)){
        validFilesEmpty = true;
        this->infile.close();
    }
    //ptr +=1;
    infile.seekg(ptr, ios::beg);
    this->tempMap = map;
    return vec;
}

long double FilesystemStream::currentSize(const int& stringSize, const long double& previousSize){
    return sizeof(string) + stringSize + previousSize;
}

bool FilesystemStream::isEmpty() {
    return empty;
}

bool FilesystemStream::endOfValidFiles(){
    return validFilesEmpty;
}

string FilesystemStream::getFilePath(){
    return outName;
}

string FilesystemStream::getValidFilesPath(){
    return validFiles;
}

string FilesystemStream::getBlockSizeStr(){
    return this->blockSizeStr;
}


Tuple FilesystemStream::getFileByIndex(int i) {
    int fileIndex = (this->mapSize + 1)*i + 1;
    cout << "file: " << fileIndex << endl;
    ifstream in = ifstream(validFiles);

    string str;

    Map map;
    string key, value;
    int valueLength; 
    size_t pos;
    Types result;

    f::goToLine(in, fileIndex);
    for(int j = 0; j < this->mapSize; ++j){

        getline(in, str);

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
    }

    in >> str;
    Tuple temp;

    get<0>(temp) = map;
    str.pop_back(); // remove trailing comma
    get<1>(temp).push_back(str);

    return temp;
}

int FilesystemStream::getValidFilesSize(){
    return this->validFilesSize;
}

vector<Tuple> FilesystemStream::getValidFilesSlice(int i, int j, int step){
    vector<Tuple> vec;
    
    for(int index = i; index < j; index += step){
        vec.push_back(getFileByIndex(index));
    }

    return vec;
}