#pragma once
#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 

#include "../internal/filepattern.hpp"
#include "../internal/stringpattern.hpp"
#include "../internal/vectorpattern.hpp"
#include "../external/external_filepattern.hpp"
#include "../external/external_stringpattern.hpp"
#include "../external/external_vectorpattern.hpp"
#include "../util/vector_parser.hpp"

namespace py = pybind11;

class FilePatternFactory {

    public:

        FilePatternFactory() {}

        std::unique_ptr<PatternObject> getObject(
            const py::array_t<std::string, py::array::c_style | py::array::forcecast>& file_array,
            const std::string& path, const std::string& file_pattern, 
            const std::string& block_size, 
            bool recursive, 
            bool suppressWarnings) {
            
            // check if array of strings was passed
            if (file_array.request().shape[0]) {

                if (file_array.ndim() != 1) {
                    throw std::runtime_error("Input array must be 1-dimensional");
                }

                std::vector<std::string> file_vector;
                // need to copy array since strings are handled differently in python
                auto file_array = strings.unchecked<1>(); // 1D array
                for (py::ssize_t i = 0; i < file_array.shape(0); ++i) {
                    file_vector.emplace_back(file_array(i));
                }

                return std::make_unique<ArrayPattern>(file_vector, file_pattern, suppressWarnings)
            }

            if (block_size == "") {
                if(fs::is_regular_file(path)) {
                    std::ifstream infile(path);
                    std::string str;

                    std::getline(infile, str);

                    if(VectorParser::isStitchingVector(str)) {
                        return std::make_unique<VectorPattern>(path, file_pattern, suppressWarnings); // need to add builder to FPOjbect
                    }

                    return std::make_unique<StringPattern>(path, file_pattern, suppressWarnings); // need to add builder to FPOjbect
                }

                return std::make_unique<FilePatternObject>(path, file_pattern, recursive, suppressWarnings); // need to add builder to FPOjbect
            }

            if(fs::is_regular_file(path)) {
                std::ifstream infile(path);
                    std::string str;

                    std::getline(infile, str);

                if(VectorParser::isStitchingVector(str)) {
                    return std::make_unique<ExternalVectorPattern>(path, file_pattern, block_size, suppressWarnings); // need to add builder to FPOjbect
                }

                return std::make_unique<ExternalStringPattern>(path, file_pattern, block_size, suppressWarnings); // need to add builder to FPOjbect
            }

            return std::make_unique<ExternalFilePattern>(path, file_pattern, block_size, recursive, suppressWarnings); // need to add builder to FPOjbect
        }
};
