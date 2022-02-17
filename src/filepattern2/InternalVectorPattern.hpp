#include "InternalPattern.hpp"
#include "VectorPattern.hpp"

class InternalVectorPattern: public VectorPattern, public InternalPattern {
    public:
    /**
     * @brief Constructor of InternalVectorPattern.
     * 
     * @param path Path to stitching vector
     * @param pattern Pattern to match filename to
     */
        InternalVectorPattern(const std::string& path, const std::string& pattern);
};