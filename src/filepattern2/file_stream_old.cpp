#include "file_stream.hpp"

using namespace std;

FileStream::FileStream(const string& filename, const string& blockSize="50 MB")
: Stream(blockSize) {

    this->infile.open(filename);
    
    this->tmpdir = fs::temp_directory_path();
    this->tmpdir += "/fs_stream_tmp_" + s::getTimeString() + "/";

    if (fs::exists(tmpdir)){
        fs::remove_all(tmpdir);
    }

    bool created = fs::create_directory(tmpdir);
    if (!created) {
        throw runtime_error("Could not create temporary file.");
    }
}



vector<string> FileStream::getBlock(){

    vector<string> vec;
    long double size = sizeof(vector<string>);
    string str;
    // get string while less than block size
    while(size < this->blockSize && this->infile >> str){
        size = Stream::currentSize(str.length(), size);
        vec.push_back(str);
    }
    
    //check if end of file
    streampos ptr = infile.tellg();
    if(!(this->infile >> str)){
        this->empty = true;
    }
    infile.seekg(ptr, ios::beg);
    
    return vec;
}

