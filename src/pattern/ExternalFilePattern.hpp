/**
 * @file ExternalFilePattern.hpp
 * @author Jesse McKinzie (Jesse.McKinzie@axleinfo.com)
 * @brief External memory version of filepattern. Matches files in a directory to a pattern using
 * a user specified amount of memory.
 * @version 0.1
 * @date 2021-12-21
 * 
 * 
 * 
 */

#ifndef ExternalFilePattern_H
#define ExternalFilePattern_H
#include <chrono>
#include "ExternalPattern.hpp"
#include "fs_stream.hpp"
#include "sort.hpp"

//namespace fs = std::filesystem;

class ExternalFilePattern : public ExternalPattern {
    public:
        std::vector<Tuple> currentBlock;

        /**
         * @brief Construct a new External File Pattern object 
         * 
         * Valid patterns are d, c, and +, where d is a digit, c is
         * a character, and + means an arbitrary number of the pattern it 
         * acts on. 

         * Example: The pattern files_x{row:ddd}_y{col:ddd}_{channel: c+}.ome.tif
         * would match files with 3 digits after x, 3 digits after y, and then an 
         * arbitrary number of characters. 
         *
         * @param path Path to directory
         * @param filePattern Pattern to compare filenames to 
         * @param blockSize Amount of memory to process at a time
         * @param recursive Iterate over all subdirectories if true
         */

        ExternalFilePattern(const std::string& path, const std::string& filePattern, const std::string& blockSize, bool recursive);

        ~ExternalFilePattern();

        /**
         * @brief Match files to pattern.
         * 
         * Iterates over directory, matching filenames to the user provided pattern.
         * If the file matches the pattern, the variables and filepath are written to 
         * a temporary .txt file. 
         */
        void matchFiles();

        /**
         * @brief Get the next block of matching files.
         * 
         * Read the next block of matching files that is of size less than or equal to 
         * the block size from the temporary .txt file. 
         */
        void next();

        /**
         * @brief Prints matched files to the console
         * 
         * Prints the maps for each matched file to the console.
         */
        void printFiles();

        /**
         * @brief Get all files that match a specified value(s) of variable(s).
         * 
         * @return std::vector<Tuple> Matched files.  
         */
        //std::vector<Tuple> getMatching(std::string&);


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

        /**
         * @brief Returns the number of 
         * 
         * @return int 
         */
        int currentBlockLength();

    private: 
        std::string path; // path to directory
        std::filesystem::directory_iterator iterator; // File iterator for given path
        std::filesystem::recursive_directory_iterator recursiveIterator; // Recursive iterator
        FilesystemStream stream; // I/O stream from temporary file
        bool endOfFile; // True if end of temp file is reached
        bool recursive; // True if recursive iterator through subdirectories
        int totalFiles; // Total number of matched files (will be deleted)
        std::ifstream infile; // Input stream used throughout methods
        bool firstCall; // True if first call has not been made to next() 

        /**
         * @brief Get a map from a .txt file 
         * 
         * Gets a map from a txt file that contains variables mapped to values.
         *
         * @param infile Input stream
         * @param mapping Map to be modified
         * @return true The end of the file has not been reached and the map is modified
         * @return false The end of the file has been reached and the mao is not modified
         */
        //bool getMap(std::fstream& infile, Tuple& mapping);

        /**
         * @brief Finds values of variables given a file that matches the pattern.
         *
         * Finds the value of variables given a matching filepath and returns a map of the 
         * variables to the respective value. The type of the variable is preserved.
         * 
         * @return A map of variables to values from the basename of the filepath.  
         */
        //Map matchFilesLoop(Map&, const std::string&, const std::regex&, std::vector<std::string>&);


        /**
         * @brief Matches files from a directory iterator. 
         * 
         * Iterates over directory using a filesystem directory iterator and finds 
         * all files in the directory that match the filepattern. matchFilesLoop is
         * called to generate a mapping between the variables and values. The mapping is
         * stored in the first member of the resulting tuple. The filepath is stored in the second 
         * member of the tuple.
         */
        void matchFilesOneDir();

        /**
         * @brief Iterates over a directory and its subdirectories, matching files 
         * to the pattern. If two files in different subdirectories have the same name, 
         * the filepath to both files is stored in the array in the second member of the tuple.
         * 
         */
        void matchFilesMultDir();

};
#endif