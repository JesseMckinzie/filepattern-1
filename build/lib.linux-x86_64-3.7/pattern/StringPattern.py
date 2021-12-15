from numpy import string_
from . import backend
import logging

class StringPattern:

    def __init__(self, filePath: str, pattern: str):
        """
        Constructor of the FilePattern class.

        Creates a new file pattern object given a path to a 
        directory of files and a pattern.

        :param str path: path to directory 
        :param str pattern: file pattern
        :param bool debug: debug mode on or off. When true, debug statements are printed to the consol

        Valid patterns are d, c, and +, where d is a digit, c is
        a character, and + means an arbitrary number of the pattern it 
        acts on. 

        Example: The pattern files_x{row:ddd}_y{col:ddd}_{channel: c+}.ome.tif
        would match files with 3 digits after x, 3 digits after y, and then an 
        arbitrary number of characters. 
        """

        try:
            self._file_pattern = backend.StringPattern(filePath, pattern)
        except RuntimeError as e:
            #error = "No directory found. Invalid path " + filePath
            print(e.what())


    def print_valid_files(self) -> None:
        """
        Prints out files that match the file pattern to the console.
        """
        
        self._file_pattern.printValidFiles()
    
    def match_files(self) -> None:
        """
        Comapres files to file pattern and stores file names that match
        the file pattern.

        :param bool cut_path: Cuts the path off of the filename, leaving only the filename (otional, defaults to true)
        e.g. /home/usr/file.txt -> file.txt when true
        """
        try: 
            self._file_pattern.matchFiles()
        except ValueError as e: 
            print(e.what())

    def __iter__(self):
        for file in self.file_pattern.validFiles.__iter__():
            yield file
        
