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
#include "Variables.hpp"
//#include "sort.h"

class ExternalPattern {
    
    protected:
        std::regex regexExpression; // Regex expression
        std::string filePattern; // Pattern to match files to
        std::string regexFilePattern; // Regex version of the pattern
        Variables variables; // structure to store the vairables
        bool filesSorted; // to be removed
        long blockSize; // Max amount of main memory to use at a time
        std::string group; // Variable to group the valid files by

    public:
        std::vector<Tuple> validFiles; // Store files that match given regex
        std::vector<std::vector<Tuple>> validGroupedFiles; // to be removed 

        /**
         * @brief Will be removed
         * 
         * @return true 
         * @return false 
         */
        bool invalidFilePath(const std::string&);

        /**
         * @brief Convert to pattern to regex.
         * 
         * Creates a version of the pattern with regex to match files to. For example, 
         * if the pattern contains {variable:d}, this is changed to [0-9] in the regex pattern.
         */
        void filePatternToRegex();


        /**
         * @brief Split a string on a character
         *
         * Splits a string into a vector of strings along a character. 
         * From https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
         *
         * @param s String to be split
         * @param delimiter Character to split string on
         * @return std::vector<std::string> Vector of split strings
         */
        std::vector<std::string> split (std::string& s, const std::string& delimiter);
        
        /**
         * @brief Print the valid files.
         * 
         */
        void printValidFiles();

        /**
         * @brief Returns the pattern that files are matched to.
         * 
         * @return std::string The pattern
         */
        std::string getPattern();

        /**
         * @brief Change the pattern to match files to.
         * 
         */
        void setPattern(const std::string&);

        /**
         * @brief Get the regex version of the pattern.
         * 
         * @return std::string The pattern with regex instead of variables.
         */
        std::string getRegexPattern();

        /**
         * @brief To be removed
         * 
         * @param group 
         */
        void setGroup(const std::string& group);
};

#endif