#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include "Pattern.hpp"
#include "InternalPattern.hpp"
#include "FilePattern.hpp"
#include "StringPattern.hpp"
#include "ExternalPattern.hpp"
#include "ExternalFilePattern.hpp"
#include "ExternalStringPattern.hpp"
#include "VectorPattern.hpp"
#include "ExternalVectorPattern.hpp"

namespace py = pybind11;

//PYBIND11_MAKE_OPAQUE(std::vector<std::vector<Tuple>>);

PYBIND11_MODULE(backend, m){

    py::class_<Pattern>(m, "Pattern")
        .def("filePatternToRegex", &Pattern::filePatternToRegex)
        .def("getPattern", &Pattern::getPattern)
        .def("setPattern", &Pattern::setPattern)
        .def("setGroup", &Pattern::setGroup)
        .def("getRegexPattern", &Pattern::getRegexPattern)
        .def("getVariables", &Pattern::getVariables)
        .def("printVariables", &Pattern::printVariables)
        .def("getOccurences", &Pattern::getOccurences)
        .def("getUniqueValues", &Pattern::getUniqueValues)
        .def("inferPattern", &Pattern::inferPattern)
        .def("__iter__", [](const Pattern &v){ 
            if(v.group != "") return py::make_iterator(v.validGroupedFiles.begin(), v.validGroupedFiles.end());
            else return py::make_iterator(v.validFiles.begin(), v.validFiles.end());}, 
            py::keep_alive<0, 1>()); // Keep vector alive while iterator is used 

    py::class_<InternalPattern, Pattern>(m, "InternalPattern")
        .def("groupBy", &InternalPattern::groupBy)
        .def("getMatching", &InternalPattern::getMatching)
        .def("outputName", &InternalPattern::outputName);

    py::class_<FilePattern, InternalPattern>(m, "FilePattern")
        .def(py::init<const std::string &, const std::string &, bool>())
        .def("matchFiles", &FilePattern::matchFiles)
        .def("printFiles", &FilePattern::printFiles);

    py::class_<StringPattern, InternalPattern>(m, "StringPattern")
        .def(py::init<const std::string &, const std::string &>())
        .def("matchFiles", &StringPattern::matchFiles)
        .def("printFiles", &StringPattern::readFile);

    py::class_<ExternalPattern, Pattern>(m, "ExternalPattern")
        .def("getMatching", &ExternalPattern::getMatching)
        .def("getMatchingBlock", &ExternalPattern::getMatchingBlock)
        .def("next", &ExternalPattern::next)
        .def("getValidFilesBlock", &ExternalPattern::getValidFilesBlock)
        .def("groupBy", &ExternalPattern::groupBy)
        .def("currentBlockLength", &ExternalPattern::currentBlockLength)
        .def("outputName", &ExternalPattern::outputName);

    py::class_<ExternalFilePattern, ExternalPattern>(m, "ExternalFilePattern")
        .def(py::init<const std::string&, const std::string&, const std::string&, bool>())
        .def("__iter__", [](ExternalFilePattern &v){ 
            v.next();
            return py::make_iterator(v.currentBlock.begin(), v.currentBlock.end());}, 
            py::keep_alive<0, 1>()); // Keep vector alive while iterator is used 
    
    py::class_<ExternalStringPattern, ExternalPattern>(m, "ExternalStringPattern")
        .def(py::init<const std::string&, const std::string&, const std::string&>())
        .def("__iter__", [](ExternalStringPattern &v){ 
            v.next();
            return py::make_iterator(v.currentBlock.begin(), v.currentBlock.end());}, 
            py::keep_alive<0, 1>()); // Keep vector alive while iterator is used 

    py::class_<VectorPattern, InternalPattern>(m, "InternalVectorPattern")
        .def(py::init<const std::string&, const std::string&>());

    py::class_<ExternalVectorPattern, ExternalPattern>(m, "ExternalVectorPattern")
        .def(py::init<const std::string&, const std::string&, const std::string&>())
        .def("__iter__", [](ExternalVectorPattern &v){ 
            v.next();
            return py::make_iterator(v.currentBlock.begin(), v.currentBlock.end());}, 
            py::keep_alive<0, 1>()); // Keep vector alive while iterator is used ;

    
}
