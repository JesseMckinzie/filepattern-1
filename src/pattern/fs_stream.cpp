 #include "fs_stream.hpp"

using namespace std;
/**
 * @brief Construct a new Filesystem Stream object
 * 
 * Creates a data stream from a filesystem directory iterator or recursive
 *
 * @param path 
 * @param recursive 
 * @param blockSize 
 */
FilesystemStream::FilesystemStream(const string& path, bool recursive, const string& blockSize, const bool isInfer)
:Stream(blockSize, isInfer){
    this->recurisve = true;

    try {
        // create recursive iterator 
        if(s::endsWith(path, ".txt")){
            this->txtInput = true;
            this->infile.open(path);
            if(!infile.is_open()){
                throw invalid_argument("Invalid path \"" + path + "\".");
            }
        } else {
            txtInput = false;
            if(recursive){
                this->recursive_directory_iterator = fs::recursive_directory_iterator(path);
                this->rec_end = fs::end(recursive_directory_iterator);
            //} else{ // create directory iterator
                this->directory_iterator = fs::directory_iterator(path); // store iterator for target directory
                this->end = fs::end(directory_iterator);
            }
        }
    } catch (const std::runtime_error& e) {
        string error = "No directory found. Invalid path \"" + path + "\"."; 
        throw std::runtime_error(error);
    }

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

    vector<string> vec;
    long previousSize = sizeof(vector<string>);

    string current;

    if(this->recurisve){

        try {
            current = (*recursive_directory_iterator).path().string();
        } catch (exception& e){
            cout << e.what() << endl;
        }

        this->updateSize(previousSize, current);

        while(this->currentSize(current.length(), previousSize) < blockSize){
            vec.push_back(current);
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
    this->infile >> str;
    this->updateSize(size, str);
    vec.push_back(str);
    size = Stream::currentSize(str.length(), size);

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





