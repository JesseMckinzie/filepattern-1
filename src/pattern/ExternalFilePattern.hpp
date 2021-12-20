#ifndef ExternalFilePattern_H
#define ExternalFilePattern_H
#include <chrono>
#include "ExternalPattern.hpp"
#include "fs_stream.hpp"
#include "sort.hpp"

//namespace fs = std::filesystem;

class ExternalFilePattern : public ExternalPattern {
    public:
        ExternalFilePattern(const std::string& path, const std::string& filePattern, const std::string& blockSize, bool recursive);

        /**
         * Stores and prints the valid filenames from the target directory. 
         */
        void matchFiles();
        void next();
        void printFiles();
        std::vector<Tuple> getMatching(std::string);
        int getCounter();
        std::vector<Tuple> getValidFilesBlock();
        void groupBy(const std::string& groupBy);
        std::vector<Tuple> get();
        std::vector<Tuple> currentBlock;
        int currentBlockLength();

    private: 
        std::string path;
        std::filesystem::directory_iterator iterator; // File iterator for given path
        std::filesystem::recursive_directory_iterator recursiveIterator;
        FilesystemStream stream;
        bool getMap(std::ifstream&, Tuple&);
        bool endOfFile;

        //std::string fileFromPath(const std::string&);
        
        bool recursive;

        Map matchFilesLoop(Map&, const std::string&, const std::regex&, std::vector<std::string>&);
        void matchFilesOneDir();
        void matchFilesMultDir();
        int totalFiles;
        int mapSize;
        std::ifstream infile;
        std::string validFilesPath;
        bool firstCall;
};
#endif