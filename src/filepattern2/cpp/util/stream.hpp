#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstddef>
#include <map>
#include "block.hpp"
#include "util.hpp"

namespace fs = std::filesystem;

class Stream {
    
    public:
        int counter; // for debugging (to be removed) 
        int mapSize; // size of maps in stream

        /**
         * @brief Write a block of strings to the output file.
         * 
         * @param vec Vector to write to output file.
         */
        void writeBlock(const std::vector<std::string>& vec);

        /**
         * @brief Write valid files maps to output file. 
         * 
         * @param mapping Mapping to write to the output file.
         */
        void writeValidFiles(const Tuple& mapping);

        /**
         * @brief Curent size of 
         * 
         * @param double 
         * @return long double 
         */
        long double currentSize(const int, const long double&);

        bool isEmpty();

        std::string getFilePath();

        std::vector<Tuple> getValidFilesBlock();

        bool endOfValidFiles();

        std::string getValidFilesPath();
        
        std::string getBlockSizeStr();

    protected:
        
        std::string tmpdir;
        long double blockSize;
        std::string outName;
        bool empty;
        bool validFilesEmpty;
        std::string validFiles;
        std::ifstream infile;
        std::string blockSizeStr;
        Map tempMap;
        bool isInfer;

        Stream(const std::string&, const bool isInfer=false);

};
