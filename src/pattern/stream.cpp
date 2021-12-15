#include "stream.hpp"

using namespace std;

Stream::Stream(const string& blockSize) {
    this->tmpdir = fs::temp_directory_path();
    this->tmpdir += "/fs_stream_tmp/";
    this->blockSizeStr = blockSize;
    this->blockSize = Block::parseblockSize(blockSize);

    
    this->empty = false;
    this->validFiles = tmpdir + "validFiles.txt";
    this->counter = 0;

    if (fs::exists(tmpdir)){
        fs::remove_all(tmpdir);
    }

    bool created = fs::create_directory(tmpdir);
    if (!created) {
        throw runtime_error("Could not create temporary file.");
    }

    this->outName = tmpdir + "/temp.txt";
    this->infile.open(validFiles);
}


void Stream::writeBlock(const vector<string>& vec){
    ofstream file(this->outName, ios_base::app);

    for(const auto& element: vec){
        file << '\n' << element;
    }
    file.close();
}

void Stream::writeValidFiles(const p::pair& mapping){
    counter++;
    ofstream file(validFiles, ios_base::app);

    for(const auto& element: mapping.first){
        file << element.first << ":" << element.second << '\n';
    }

    for(const auto& element: mapping.second){
        file << element << ",";
    } 
    file << '\n';
    file.close();
    validFilesEmpty = false;
    if(counter == 1){
        this->mapSize = mapping.first.size();
        this->infile.open(validFiles);
    }
}

vector<p::pair> Stream::getValidFilesBlock(){

    vector<p::pair> vec;
    p::pair member;
    
    long size = sizeof(vector<p::pair>);

    map<string, string> map;
    string str;
    string key, value;
    int valueLength;
    size_t pos;

    while(size < blockSize && this->infile >> str){
        
        if (map.size() == (this->mapSize)) {
            size += sizeof(map) + sizeof(vector<string>);
            
            //sizeof(p::pair) +
            for(const auto& item : map){
                size += item.first.length() + item.second.length();
            }
            member.first = map;

            member.second.push_back(str);
            vec.push_back(member);
            map.clear();
            member.second.clear();
            infile >> str;
        } 

        pos = str.find(":");
        key = str.substr(0, pos);
        valueLength = str.length() - pos;
        value = str.substr(pos+1, valueLength);

        map[key] = value;
        size += valueLength + pos;
    }
    /*
    pos = str.find(":");
    key = str.substr(0, pos);
    valueLength = str.length() - pos;
    value = str.substr(pos+1, valueLength);
    

    map[key] = value;
    vec.push_back(member);
    */
    streampos ptr = infile.tellg();
    if(!(this->infile >> str)){
        validFilesEmpty = true;
    }
    infile.seekg(ptr, ios::beg);
        
    return vec;
}

long double Stream::currentSize(const int& stringSize, const long double& previousSize){
    return sizeof(string) + stringSize + previousSize;
}

bool Stream::isEmpty() {
    return empty;
}

bool Stream::endOfValidFiles(){
    return validFilesEmpty;
}

string Stream::getFilePath(){
    return outName;
}

string Stream::getValidFilesPath(){
    return validFiles;
}

string Stream::getBlockSizeStr(){
    return this->blockSizeStr;
}

/*
string Stream::copyValidFiles(const string& newFileName){
    ifstream infile(this->validFiles);

    string filePath = tmpdir + newFileName;
    ofstream outfile(filePath);

    outfile << infile.rdbuf();

    return filePath;

}
*/