/**
 * @file fs_stream.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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

class FilesystemStream {

    public:

        /**
         * @brief Construct a new Filesystem Stream object.
         * 
         * Creates a data stream of filenames from a filesystem directory iterator.
         *
         * @param path Path to directory to stream filenames from
         * @param recursive True will create a recursive directory iterator that will iterate through subdirectories
         * @param blockSize Maximum size of memory the stream will consume at a time
         */
        FilesystemStream(const std::string& path, bool recursive=false, const std::string& blockSize="50 MB", const bool isInfer=false);
        int counter; // for debugging (to be removed) 
        int mapSize; // size of maps in stream

        //~FilesystemStream();
        /**
         * @brief Get a block of filenames that consumes at most blockSize of main memory.
         * 
         * @return std::vector<std::string> Vector of filenames
         */
        std::vector<std::string> getBlock();

        /**
         * @brief Get a block of filenames that consumes at most blockSize of main memory.
         * 
         * @return std::vector<std::string> Vector of filenames
         */
        std::vector<std::string> getBlockIterator();

        /**
         * @brief Get a block of filenames that consumes at most blockSize of main memory.
         * 
         * @return std::vector<std::string> Vector of filenames
         */
        std::vector<std::string> getBlockTxt();

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
        long double currentSize(const int&, const long double&);

        bool isEmpty();

        std::string getFilePath();

        std::vector<Tuple> getValidFilesBlock();

        bool endOfValidFiles();

        std::string getValidFilesPath();
        
        std::string getBlockSizeStr();

        std::string getTmpPath();

    private:
        fs::directory_iterator directory_iterator; // directory iterator to stream from
        fs::recursive_directory_iterator recursive_directory_iterator; 
        bool recurisve; // Iterate over subdirectories
        fs::directory_iterator end; // End of iterator
        fs::recursive_directory_iterator rec_end; // End of recursive iterator
        bool txtInput;
        std::ifstream infile;
        std::ifstream inputfile;

        std::string tmpdir;
        long double blockSize;
        std::string outName;
        bool empty;
        bool validFilesEmpty;
        std::string validFiles;
        std::string blockSizeStr;
        Map tempMap;
        bool isInfer;

        void updateSize(long& size, const std::string& current);
};
