/**
 * @file ExternalFilePattern.hpp
 * @author Jesse McKinzie (Jesse.McKinzie@axleinfo.com)
 * @brief External memory version of filepattern. Matches files in a directory to a pattern using
 * a user specified amount of memory.
 * @version 0.1
 * @date 2021-12-21
 * 
 * 
 * 
 */

#ifndef ExternalFilePattern_H
#define ExternalFilePattern_H
#include <chrono>
#include <bits/stdc++.h>
#include "ExternalPattern.hpp"
#include "fs_stream.hpp"
#include "sort.hpp"

//namespace fs = std::filesystem;

class ExternalFilePattern : public ExternalPattern {
    public:
        std::vector<Tuple> currentBlock;

        /**
         * @brief Construct a new External File Pattern object 
         * 
         * Valid patterns are d, c, and +, where d is a digit, c is
         * a character, and + means an arbitrary number of the pattern it 
         * acts on. 

         * Example: The pattern files_x{row:ddd}_y{col:ddd}_{channel: c+}.ome.tif
         * would match files with 3 digits after x, 3 digits after y, and then an 
         * arbitrary number of characters. 
         *
         * @param path Path to directory
         * @param filePattern Pattern to compare filenames to 
         * @param blockSize Amount of memory to process at a time
         * @param recursive Iterate over all subdirectories if true
         */

        ExternalFilePattern(const std::string& path, const std::string& filePattern, const std::string& blockSize, bool recursive);

        /**
         * @brief Match files to pattern.
         * 
         * Iterates over directory, matching filenames to the user provided pattern.
         * If the file matches the pattern, the variables and filepath are written to 
         * a temporary .txt file. 
         */
        void matchFiles();

        /**
         * @brief Get the next block of matching files.
         * 
         * Read the next block of matching files that is of size less than or equal to 
         * the block size from the temporary .txt file. 
         */
        void next();

        /**
         * @brief Prints matched files to the console
         * 
         * Prints the maps for each matched file to the console.
         */
        void printFiles();

        /**
         * @brief Get all files that match a specified value(s) of variable(s).
         * 
         * @return std::vector<Tuple> Matched files.  
         */
        std::vector<Tuple> getMatching(std::string&);

        /**
         * @brief To be removed
         * 
         * @return int 
         */
        int getCounter();

        /**
         * @brief Returns a block of valid files that consumes at most the user specified
         * amount of memory
         *
         * Reads a block of files from the temporary .txt file into memory that is of size less 
         * than or equal to the block size.  
         *
         * @return std::vector<Tuple> The block of matching files where the first member of the tuple
         * is the map of variables to their value and the second member is a vector containing paths to 
         * matching files.
         */
        std::vector<Tuple> getValidFilesBlock();

        /**
         * @brief Group files by a variable. 
         *
         * Sorts the temporary .txt file by the provided variable uses an 
         * external merge sort algorithm to maintain memory usage of less than the block size.
         * 
         * @param groupBy A variable that is contained in the pattern.
         */
        void groupBy(const std::string& groupBy);

        /**
         * @brief Returns the number of 
         * 
         * @return int 
         */
        int currentBlockLength();

    private: 
        std::string path;
        std::filesystem::directory_iterator iterator; // File iterator for given path
        std::filesystem::recursive_directory_iterator recursiveIterator;
        FilesystemStream stream;
        bool endOfFile;
        bool recursive;
        int totalFiles;
        int mapSize;
        std::ifstream infile;
        std::string validFilesPath;
        bool firstCall;

        bool getMap(std::fstream&, Tuple&);

        Map matchFilesLoop(Map&, const std::string&, const std::regex&, std::vector<std::string>&);

        void matchFilesOneDir();

        void matchFilesMultDir();
};
#endif