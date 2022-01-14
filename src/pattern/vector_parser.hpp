#pragma once
#include "util.hpp"

class VectorParser {
    public:
        static std::string getFileName(std::string& line);

        static void parseVectorLine(Tuple& tup,
                                    std::string& stitchingVectorLine, 
                                    const std::vector<std::string>& STITCH_VALRIABLES, 
                                    const std::regex& STITCH_REGEX,
                                    std::vector<std::string> variables);

};