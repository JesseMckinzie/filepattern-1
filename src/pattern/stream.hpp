#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstddef>
#include <map>
#include "block.hpp"
#include "Variables.hpp"

namespace fs = std::filesystem;

class Stream {
    
    public:
        void writeBlock(const std::vector<std::string>&);
        void writeValidFiles(const p::pair& mapping);
        long double currentSize(const int&, const long double&);
        bool isEmpty();
        std::string getFilePath();
        std::vector<p::pair> getValidFilesBlock();
        bool endOfValidFiles();
        int counter;
        std::string getValidFilesPath();
        int mapSize;
        std::string getBlockSizeStr();
        //void sort(const std::string&);

    protected:
        Stream(const std::string&);
        std::string tmpdir;
        long double blockSize;
        std::string outName;
        bool empty;
        bool validFilesEmpty;
        std::string validFiles;
        std::ifstream infile;
        std::string blockSizeStr;

};
