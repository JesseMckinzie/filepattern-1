#include "file_stream.hpp"

using namespace std;

FileStream::FileStream(const string& filename, const string& blockSize="50 MB")
: Stream(blockSize) {

    this->infile.open(filename);
    
}

vector<string> FileStream::getBlock(){

    vector<string> vec;
    long double size = sizeof(vector<string>);
    string str;

    while(size < this->blockSize && this->infile >> str){
        size = Stream::currentSize(str.length(), size);
        vec.push_back(str);
    }
    streampos ptr = infile.tellg();
    if(!(this->infile >> str)){
        this->empty = true;
    }
    infile.seekg(ptr, ios::beg);
    
    return vec;
}

