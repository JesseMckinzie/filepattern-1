#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <map>
#include "Pattern.hpp"
#include "util.hpp"

class InternalPattern : public Pattern {

    private:
        std::vector<Tuple> matching;
        std::vector<std::pair<std::string, std::vector<Types>>> variableMapping;

        void getMatchingHelper(const std::tuple<std::string, std::vector<Types>>& variableMap);

        void getMatchingLoop(std::vector<Tuple>& iter, 
                             const std::string& variable, 
                             const std::vector<Types>& values, 
                             Types& temp);
    
    public:
        /**
         * @brief Group matched files by a variable.
         * 
         * Sorts files by provided variable to return matching files in groups where the
         * parameter variable is constant.
         *
         * @param group Variable to group files by
         */
        void groupBy(const std::string& groupBy);

        /**
         * @brief Returns files that match the value of variable. Needs to be updated to match old version input. 
         * 
         * @param variables Variables with value to match (ex. "x=1, y=2")
         * @return std::vector<Tuple> Vector of files where the variable(s) match the value.
         */
        std::vector<Tuple> getMatching(const std::vector<std::tuple<std::string, std::vector<Types>>>& variables);

};
