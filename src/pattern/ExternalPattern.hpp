/**
 * @file ExternalPattern.hpp
 * @author Jesse McKinzie (Jesse.McKinzie@axleinfo.com)
 * @brief Parent class of ExternalFilePattern and ExternalStringPattern
 * @version 0.1
 * @date 2021-12-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#ifndef ExternalPattern_H
#define ExternalPattern_H
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <map>
#include <tuple>
#include <chrono>
#include "Pattern.hpp"
#include "util.hpp"
#include "stream.hpp"
#include "fs_stream.hpp"
#include "sort.hpp"
//#include "sort.h"

class ExternalPattern : public Pattern {
    
    protected:
        long blockSize; // Max amount of main memory to use at a time
        std::string matching;
        std::string matchingCopy;
        std::string validFilesPath; // Path to temporary txt file containing valid files
        int mapSize;
        std::ifstream matchingStream;
        std::ifstream groupStream;
        Types currentValue; 
        std::ifstream infile; // Input stream used throughout methods
        bool firstCall; // True if first call has not been made to next()
        Tuple temp;
        FilesystemStream stream; // I/O stream from temporary file

        void getMatchingHelper(const std::tuple<std::string, std::vector<Types>>& variableMap, const std::string& matching);

        void getMatchingLoop(std::ifstream& infile, 
                             std::ofstream& outfile,
                             const std::string& variable, 
                             const std::vector<Types>& values, 
                             Types& temp,
                             Tuple& tempMap);
        
        std::string externalOutPutName();

    public: 
        std::vector<Tuple> currentBlock;
        std::vector<std::pair<std::pair<std::string, Types>, std::vector<Tuple>>> currentGroup;

        ExternalPattern(const std::string& path, const std::string& blockSize, bool recursive);

        /**
         * @brief Returns files that match the value of variable. Needs to be updated to match old version input. 
         * 
         * @param variables Variables with value to match (ex. "x=1, y=2")
         * @return std::vector<Tuple> Vector of files where the variable(s) match the value.
         */
        std::string getMatching(const std::vector<std::tuple<std::string, std::vector<Types>>>& variables);

        std::vector<Tuple> getMatchingBlock();

        /**
         * @brief Get the next block of matching files.
         * 
         * Read the next block of matching files that is of size less than or equal to 
         * the block size from the temporary .txt file. 
         */
        void next();

        void nextGroup();

        /**
         * @brief Returns the number of 
         * 
         * @return int 
         */
        int currentBlockLength();

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


        std::string outputName(std::vector<Tuple>& vec);

        static std::string inferPattern(const std::string& path, std::string& variables, const std::string& blockSize);

        int getGroupLength();
};

#endif