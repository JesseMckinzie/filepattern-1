#include "InternalVectorPattern.hpp"

using namespace std;

InternalVectorPattern::InternalVectorPattern(const string& path, const string& pattern){
    Pattern::path = path; // store path to target directory

    this->infile.open(Pattern::path);
    if(!infile.is_open()) throw invalid_argument("Invalid path \"" + Pattern::path + "\".");

    Pattern::filePattern = pattern; // cast input string to regex
    Pattern::regexFilePattern = ""; // Regex version of pattern

    VectorPattern::matchFiles();
}
