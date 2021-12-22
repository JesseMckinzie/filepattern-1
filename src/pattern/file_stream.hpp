/**
 * @file file_stream.hpp
 * @author Jesse McKinzie (Jesse.McKinzie@axleinfo.com)
 * @brief Creates a data stream from a .txt file. Child class of stream.hpp.
 * @version 0.1
 * @date 2021-12-22
 * 
 * 
 */

#pragma once
#include "stream.hpp"

class FileStream : public Stream {
    
    public:
        /**
         * @brief Construct a new File Stream object.
         * 
         * Constructs a data stream from a .txt to return strings in less than or 
         * equal to the size of blockSize.
         * 
         * @param filename Path to .txt file
         * @param blockSize Size of memory block.
         */
        FileStream(const std::string& filename, const std::string& blockSize);

        /**
         * @brief Returns a block of strings from .txt file.
         * 
         * @return std::vector<std::string> Block of strings of size less than blockSize. 
         */
        std::vector<std::string> getBlock();
};
