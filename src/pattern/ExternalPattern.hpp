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
        std::string group; // Variable to group the valid files by

    public: 

        /**
         * @brief Print the valid files.
         * 
         */
        void printValidFiles();

        /**
         * @brief Sets the variable to be grouped in groupBy()
         * 
         * @param group Variable to group the matched files by 
         */
        void setGroup(const std::string& group);
};

#endif