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
#include "stream.hpp"

class FilesystemStream : public Stream {

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
        FilesystemStream(const std::string& path, bool recursive=false, const std::string& blockSize="50 MB");

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

    private:
        fs::directory_iterator directory_iterator; // directory iterator to stream from
        fs::recursive_directory_iterator recursive_directory_iterator; 
        bool recurisve; // Iterate over subdirectories
        fs::directory_iterator end; // End of iterator
        fs::recursive_directory_iterator rec_end; // End of recursive iterator
        bool txtInput;
        std::ifstream infile;
};
