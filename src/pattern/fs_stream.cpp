#include "fs_stream.hpp"

using namespace std;

FilesystemStream::FilesystemStream(const string& path, bool recursive=false, const string& blockSize="50 MB")
:Stream(blockSize){
    this->recurisve = true;

    try {
        if(recursive){
            this->recursive_directory_iterator = fs::recursive_directory_iterator(path);
            this->rec_end = fs::end(recursive_directory_iterator);
        } else{ 
            this->directory_iterator = fs::directory_iterator(path); // store iterator for target directory
            this->end = fs::end(directory_iterator);
        }
    } catch (const std::runtime_error& e) {
        string error = "No directory found. Invalid path \"" + path + "\"."; 
        throw std::runtime_error(error);
    }

}

vector<string> FilesystemStream::getBlock(){
    
    vector<string> vec;
    long double previousSize = sizeof(vector<string>);

    string current;
    if(this->recurisve){
        try {
            current = (*recursive_directory_iterator).path().string();
            //cout << "current: " << current << endl;
        } catch (exception& e){
            cout << "Error on line 52" << endl;
            cout << e.what() << endl;
        }
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
                    //cout << current << endl;
                } catch (exception& e){
                    cout << e.what() << endl;
                }
            }
        }
    } else{ 
        //cout << "here" << endl;
        try {
            current = (*directory_iterator).path().string();
        } catch (exception& e){
            cout << "Error on line 52" << endl;
            cout << e.what() << endl;
        }
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




