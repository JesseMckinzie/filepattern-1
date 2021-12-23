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

        void groupBy(const std::string&);

        std::vector<Tuple> getMatching(std::string&);

};
