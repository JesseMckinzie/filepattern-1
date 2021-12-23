#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <regex>
#include <map>
#include "Pattern.hpp"
#include "Variables.hpp"

class InternalPattern : public Pattern {

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
        std::vector<Tuple> getMatching(std::string& variables);

};
