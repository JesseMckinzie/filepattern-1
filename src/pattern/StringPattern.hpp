#pragma once
#include "InternalPattern.hpp"

class StringPattern : public InternalPattern {
    public:
        
        /**
         * @brief Constructor of StringPattern. 
         * 
         * @param fileName Path to text file
         * @param filePattern Pattern to match line of text file to
         */
        StringPattern(const std::string& fileName, const std::string& filePattern);

        /**
         * @brief Match lines of text file to the pattern.
         * 
         */
        void matchFiles();

        /**
         * @brief Read file into main memory.
         * 
         */
        void readFile();

    private:
        std::vector<std::string> files; // Vector of filenames
        std::string fileName; // path to txt file
};