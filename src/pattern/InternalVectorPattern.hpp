#include "InternalPattern.hpp"
#include "VectorPattern.hpp"

class InternalVectorPattern: public VectorPattern, public InternalPattern {
    public:
        InternalVectorPattern(const std::string& path, const std::string& pattern);
};