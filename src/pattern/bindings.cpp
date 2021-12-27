#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include "Pattern.hpp"
#include "InternalPattern.hpp"
#include "FilePattern.hpp"
#include "StringPattern.hpp"
#include "ExternalPattern.hpp"
#include "ExternalFilePattern.hpp"

namespace py = pybind11;

//PYBIND11_MAKE_OPAQUE(std::vector<std::vector<Tuple>>);

PYBIND11_MODULE(backend, m){

    py::class_<Pattern>(m, "Pattern")
        .def("filePatternToRegex", &Pattern::filePatternToRegex)
        .def("getPattern", &Pattern::getPattern)
        .def("setPattern", &Pattern::setPattern)
        .def("getRegexPattern", &Pattern::getRegexPattern)
        .def("getVariables", &Pattern::getVariables)
        .def("printVariables", &Pattern::printVariables)
        .def("__iter__", [](const Pattern &v){ 
            if(v.group != "") return py::make_iterator(v.validGroupedFiles.begin(), v.validGroupedFiles.end());
            else return py::make_iterator(v.validFiles.begin(), v.validFiles.end());}, 
            py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    py::class_<InternalPattern, Pattern>(m, "InternalPattern")
        .def("groupBy", &InternalPattern::groupBy)
        .def("getMatching", &InternalPattern::getMatching);

    py::class_<FilePattern, InternalPattern>(m, "FilePattern")
        .def(py::init<const std::string &, const std::string &, bool>())
        .def("matchFiles", &FilePattern::matchFiles)
        .def("printFiles", &FilePattern::printFiles);

    py::class_<StringPattern, InternalPattern>(m, "StringPattern")
        .def(py::init<const std::string &, const std::string &>())
        .def("matchFiles", &StringPattern::matchFiles)
        .def("printFiles", &StringPattern::readFile);

    py::class_<ExternalPattern, Pattern>(m, "ExternalPattern")
        //.def("invalidFilePath", &ExternalPattern::invalidFilePath)
        .def("setGroup", &ExternalPattern::setGroup)
        .def("getMatching", &ExternalPattern::getMatching);

    py::class_<ExternalFilePattern, ExternalPattern>(m, "ExternalFilePattern")
        .def(py::init<const std::string&, const std::string&, const std::string&, bool>())
        .def("matchFiles", &ExternalFilePattern::matchFiles)
        .def("printFiles", &ExternalFilePattern::printFiles)
        .def("next", &ExternalFilePattern::next)
        .def("getValidFilesBlock", &ExternalFilePattern::getValidFilesBlock)
        .def("groupBy", &ExternalFilePattern::groupBy)
        .def("currentBlockLength", &ExternalFilePattern::currentBlockLength)
        .def("__iter__", [](ExternalFilePattern &v){ 
            v.next();
            return py::make_iterator(v.currentBlock.begin(), v.currentBlock.end());}, 
            py::keep_alive<0, 1>());
}
