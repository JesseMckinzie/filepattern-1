/**
 * @file Pattern.hpp
 * @author Jesse McKinzie (Jesse.McKinzie@axleinfo.com)
 * @brief Parent class of FilePattern and ExternalFilePattern
 * @version 0.1
 * @date 2021-12-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include "fs_stream.hpp"
#include "util.hpp"

class Pattern {
    
    protected:
        std::regex regexExpression; // Regex expression
        std::string filePattern; // Pattern to match files to
        std::string regexFilePattern; // Pattern with capture groups
        std::vector<std::string> variables; // Store the names of variables from the pattern
        std::map<std::string, std::map<Types, int>> variableOccurences;
        std::map<std::string, std::set<Types>> uniqueValues;
        std::vector<std::string> namedGroups;

        std::string VARIABLES;

        static std::string inferPatternInternal(std::vector<std::string>& files, std::string& variables, const std::string& startingPattern="");

    public:
        std::vector<Tuple> validFiles; // Store files that match given regex
        std::vector<std::vector<Tuple>> validGroupedFiles;
        std::string group;

        /**
         * @brief Convert to pattern to regex.
         * 
         * Creates a version of the pattern with regex to match files to. For example, 
         * if the pattern contains {variable:d}, this is changed to [0-9] in the regex pattern.
         */
        void filePatternToRegex();

        static std::tuple<std::string, std::vector<std::string>, std::vector<std::string>> getRegex(std::string& pattern);

        /**
         * @brief Get the mapping of variables to values for a matching file. Used with a recursive directory iterator. 
         * 
         * Uses the regex version of the pattern from filePatternToRegex() to extract capture groups from
         * a basename. Returns a tuple of variable matched to capture group and the file path if no file with 
         * the same basename has already been matched and an empty tuple otherwise. In the later case, the 
         * basename is appending to the second member of the existing tuple.
         * 
         * @param filePattern filePath that matches the pattern
         * @param sm captured groups
         * @return Tuple A tuple with the mapping in first and the file path in second. An empty tuple is returned
         * if the basename of filePath has already been matched
         */
        Tuple getVariableMapMultDir(const std::string& filePath, const std::smatch& sm);


         /**
         * @brief Get the mapping of variables to values for a matching file. Used with a directory iterator.
         * 
         * Uses the regex version of the pattern from filePatternToRegex() to extract capture groups from
         * a basename. Returns a tuple of variable matched to capture group.
         * 
         * @param filePattern filePath that matches the pattern
         * @param sm captured groups
         * @return Tuple A tuple with the mapping in first and the file path in second 
         */
        Tuple getVariableMap(const std::string& filePath, const std::smatch& sm);

        /**
         * @brief Get the pattern to match files to.
         * 
         * @return std::string The pattern that files are matched to
         */
        std::string getPattern();

        /**
         * @brief Set the pattern.
         * 
         * @param pattern New pattern
         */
        void setPattern(const std::string& pattern);

        /**
         * @brief Get the pattern with regex capture groups
         * 
         * @return std::string The pattern with regex capture groups
         */
        std::string getRegexPattern();
        
        /**
         * @brief Get the variable names
         * 
         * @return std::vector<std::string> Vector of variable names
         */
        std::vector<std::string> getVariables();

        /**
         * @brief Prints the variables to the console.
         * 
         */
        void printVariables();

        /**
         * @brief Sets the variable to be grouped in groupBy()
         * 
         * @param group Variable to group the matched files by 
         */
        void setGroup(const std::string& group);

        std::map<std::string, std::map<Types, int>> getOccurences(const std::vector<std::tuple<std::string, std::vector<Types>>>& mapping);
        //std::map<std::string, std::map<Types, int>> getOccurences();

        std::map<std::string, std::set<Types>> getUniqueValues(const std::vector<std::string>& mapping);

        static void getNewNaming(std::string& pattern);

        std::string outputNameHelper(std::vector<Tuple>& vec);

        void replaceOutputName(Tuple& min, 
                               Tuple& max, 
                               const std::string& var, 
                               std::string& outputName, 
                               const int idx, 
                               std::string& temp, 
                               const std::regex& patternRegex);

        //static std::string inferPattern(const std::string& path, std::string& variables, const std::string& blockSize="");
        //static std::string inferPattern(std::vector<std::string>& vec, std::string& variables);
        static std::string swSearch(std::string& pattern, std::string& filename, const std::string& variables);
};
