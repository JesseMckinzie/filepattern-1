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
        int counter; // for debugging (to be removed) 
        int mapSize; // size of maps in stream
    
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

        ~FilesystemStream();

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

        /**
         * @brief True if no more files, otherwise false
         * 
         * @return true No more files remaining
         * @return false More files remain
         */
        bool isEmpty();

        /**
         * @brief Returns path to files
         * 
         * @return std::string Path to files
         */
        std::string getFilePath();

        /**
         * @brief Get a block of matched files that uses at most blockSize of memory.
         * 
         * @return std::vector<Tuple> Block of matched files
         */
        std::vector<Tuple> getValidFilesBlock();

        /**
         * @brief End of vaildFiles.txt has been reached
         * 
         * @return true No more matched files remain
         * @return false There are still matched files
         */
        bool endOfValidFiles();

        /**
         * @brief Returns path of validFiles.txt, which stores the matched files
         * 
         * @return std::string Path to validFiles.txt
         */
        std::string getValidFilesPath();
        
        /**
         * @brief Returns the string version of the blockSize
         * 
         * @return std::string String containing the max amount of memory used
         */
        std::string getBlockSizeStr();

        /**
         * @brief Returns path of the temporary directory used by the FilesystemStream object
         * 
         * @return std::string Path to the temporary directory
         */
        std::string getTmpPath();

    private:
        fs::directory_iterator directory_iterator; // directory iterator to stream from
        fs::recursive_directory_iterator recursive_directory_iterator; 
        fs::directory_iterator end; // End of iterator
        fs::recursive_directory_iterator rec_end; // End of recursive iterator

        bool recurisve; // Iterate over subdirectories
        bool txtInput; // The path is to a text file

        std::ifstream infile;
        std::ifstream inputfile;

        std::string tmpdir; // path to temporary directory
        std::string outName; 
        std::string validFiles; // store matched files block 
        std::string blockSizeStr; // string version of max amount of memory
        Map tempMap; 
        bool isInfer; // Called from inferPattern method. Calculates memory used from matrix in inferPattern
        bool empty; // no more files remaining
        bool validFilesEmpty; // no more matched files
        long double blockSize; // max amount of memory to use

        /**
         * @brief Updates the amount of memory being used
         * 
         * @param size Current amount of memory
         * @param current String to add memory use of
         */
        void updateSize(long& size, const std::string& current);
};
