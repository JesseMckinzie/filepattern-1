#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/stl/filesystem.h>
#include <iostream>
#include <vector>
#include "pattern.hpp"
#include "internal/internal_pattern.hpp"
#include "internal/filepattern.hpp"
#include "internal/stringpattern.hpp"
#include "internal/vectorpattern.hpp"
#include "external/external_pattern.hpp"
#include "external/external_filepattern.hpp"
#include "external/external_stringpattern.hpp"
#include "external/external_vectorpattern.hpp"
#include "util/util.hpp"


namespace py = pybind11;

//PYBIND11_MAKE_OPAQUE(std::vector<std::vector<Tuple>>);

PYBIND11_MODULE(backend, m){

    py::class_<Pattern>(m, "Pattern")
        .def("filePatternToRegex", &Pattern::filePatternToRegex)
        .def("getPattern", &Pattern::getPattern)
        .def("setPattern", &Pattern::setPattern)
        .def("getPath", &Pattern::getPath)
        .def("getRegexPattern", &Pattern::getRegexPattern)
        .def("getVariables", &Pattern::getVariables)
        .def("printVariables", &Pattern::printVariables)
        .def("getOccurrences", &Pattern::getOccurrences)
        .def("getUniqueValues", &Pattern::getUniqueValues)
        .def("getTmpDirs", &Pattern::getTmpDirs)
        .def("setGroup", &Pattern::setGroup)
        .def("setMatchingVariables", &Pattern::setMatchingVariables)
        .def_readwrite("matchingVariables", &Pattern::matching_variables_)
        .def_static("getRegex", &Pattern::getRegex)
        .def_readonly("group", &Pattern::group_)
        .def_readonly("validFiles", &Pattern::valid_files_);

    py::class_<InternalPattern, Pattern>(m, "InternalPattern")
        .def("groupBy", &InternalPattern::groupBy)
        .def("getMatching", &InternalPattern::getMatching)
        .def("outputName", &InternalPattern::outputName)
        .def("getItem", &InternalPattern::getItem)
        .def("getSlice", &InternalPattern::getSlice)
        .def("getItemList", &InternalPattern::getItemList)
        .def_static("inferPattern", py::overload_cast<std::vector<std::string>&, std::string&>(&InternalPattern::inferPattern))
        .def_static("inferPattern", py::overload_cast<const std::string&, std::string&>(&InternalPattern::inferPattern))
        .def("__iter__", [](InternalPattern &v){
            if (v.group_.size() == 0 || v.group_[0] != "") {

                if (v.matching_variables_.size() == 0 && v.group_.size() == 0) {
                    return py::make_iterator(v.valid_files_.begin(), v.valid_files_.end());
                }

                if (v.matching_variables_.size() == 0 && v.group_.size() != 0) {
                    return py::make_iterator(v.valid_grouped_files_.begin(), v.valid_grouped_files_.end());
                }

                if (v.matching_variables_.size() != 0 && v.group_.size() == 0) {
                    v.getMatching(v.matching_variables_, true);
                    return py::make_iterator(v.matching_.begin(), v.matching_.end());
                }

                if (v.matching_variables_.size() != 0) {
                    v.getMatchingGrouped();
                }

                if(v.group_.size() != 0)
                    return py::make_iterator(v.valid_grouped_files_.begin(), v.valid_grouped_files_.end());

                return py::make_iterator(v.valid_files_.begin(), v.valid_files_.end());
            } else if (v.matching_variables_.size() != 0) {

                //std::vector<Tuple> matching = v.getMatching(v.matching_variables_);
                return py::make_iterator(v.matching_grouped_.begin(), v.matching_grouped_.end());
            } else { 

                return py::make_iterator(v.valid_files_.begin(), v.valid_files_.end());
            }
            }, 
            py::keep_alive<0, 1>()); // Keep vector alive while iterator is used 

    py::class_<FilePattern, InternalPattern>(m, "FilePattern")
        .def(py::init<const std::string &, const std::string &, bool, bool>())
        .def("matchFiles", &FilePattern::matchFiles)
        .def("printFiles", &FilePattern::printFiles);

    py::class_<StringPattern, InternalPattern>(m, "StringPattern")
        .def(py::init<const std::string &, const std::string &, bool>())
        .def("matchFiles", &StringPattern::matchFiles)
        .def("printFiles", &StringPattern::readFile);

    py::class_<ExternalPattern, Pattern>(m, "ExternalPattern")
        .def("getMatching", &ExternalPattern::getMatching)
        .def("getMatchingBlock", &ExternalPattern::getMatchingBlock)
        .def("next", &ExternalPattern::next)
        .def("getValidFilesBlock", &ExternalPattern::getValidFilesBlock)
        .def("groupBy", &ExternalPattern::groupBy)
        .def("currentBlockLength", &ExternalPattern::currentBlockLength)
        .def("outputName", &ExternalPattern::outputName)
        .def("getGroupLength", &ExternalPattern::getGroupLength)
        .def("inferPattern", &ExternalPattern::inferPattern)
        .def("getItem", &ExternalPattern::getItem)
        .def("getSlice", &ExternalPattern::getSlice)
        .def("getItemList", &ExternalPattern::getItemList)
        .def("__iter__", [](ExternalPattern &v){ 
            if(v.group_.size() == 0 || v.group_[0] == "") {
                v.next();
                return py::make_iterator(v.current_block_.begin(), v.current_block_.end());
            } else {
                v.nextGroup();
                return py::make_iterator(v.current_group_.begin(), v.current_group_.end());
            }}, 
            py::keep_alive<0, 1>()); // Keep vector alive while iterator is used ;
        

    py::class_<ExternalFilePattern, ExternalPattern>(m, "ExternalFilePattern")
        .def(py::init<const std::string&, const std::string&, const std::string&, bool, bool>())
        .def("__iter__", [](ExternalFilePattern &v){ 
            if(v.group_.size() == 0 || v.group_[0] == "") {
                v.next();
                return py::make_iterator(v.current_block_.begin(), v.current_block_.end());
            } else {
                v.nextGroup();
                return py::make_iterator(v.current_group_.begin(), v.current_group_.end());
            }}, 
            py::keep_alive<0, 1>()); // Keep vector alive while iterator is used ;
    
    py::class_<ExternalStringPattern, ExternalPattern>(m, "ExternalStringPattern")
        .def(py::init<const std::string&, const std::string&, const std::string&, bool>());

    py::class_<VectorPattern, InternalPattern>(m, "InternalVectorPattern")
        .def(py::init<const std::string&, const std::string&, bool>())
        .def_static("inferPattern", &VectorPattern::inferPattern);

    py::class_<ExternalVectorPattern, ExternalPattern>(m, "ExternalVectorPattern")
        .def(py::init<const std::string&, const std::string&, const std::string&, bool>())
        .def_static("inferPattern", &ExternalVectorPattern::inferPattern);

    
}
