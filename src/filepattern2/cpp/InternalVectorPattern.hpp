#include "InternalPattern.hpp"
#include "VectorPattern.hpp"

class InternalVectorPattern: public InternalPattern {
    private:
        std::string path; // path to stitching vector
        std::regex STITCH_REGEX; // regex to match stitching vector line
        std::vector<std::string> STITCH_VARIABLES; // variable names for stitching vector 
        std::ifstream infile; // stream to read in stitching vector

    public:
    /**
     * @brief Constructor of InternalVectorPattern.
     * 
     * @param path Path to stitching vector
     * @param pattern Pattern to match filename to
     */
        InternalVectorPattern(const std::string& path, const std::string& pattern);
};