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
#include "Pattern.hpp"
#include "Variables.hpp"
//#include "sort.h"

class ExternalPattern : public Pattern {
    
    protected:
        long blockSize; // Max amount of main memory to use at a time
        std::string matching;
        std::string matchingCopy;
        std::string validFilesPath; // Path to temporary txt file containing valid files
        int mapSize;

        void getMatchingHelper(const std::tuple<std::string, std::vector<Types>>& variableMap, const std::string& matching);

        void getMatchingLoop(std::ifstream& infile, 
                             std::ofstream& outfile,
                             const std::string& variable, 
                             const std::vector<Types>& values, 
                             Types& temp,
                             Tuple& tempMap);

    public: 

        /**
         * @brief Sets the variable to be grouped in groupBy()
         * 
         * @param group Variable to group the matched files by 
         */
        void setGroup(const std::string& group);

        /**
         * @brief Returns files that match the value of variable. Needs to be updated to match old version input. 
         * 
         * @param variables Variables with value to match (ex. "x=1, y=2")
         * @return std::vector<Tuple> Vector of files where the variable(s) match the value.
         */
        std::string getMatching(const std::vector<std::tuple<std::string, std::vector<Types>>>& variables);
};

#endif