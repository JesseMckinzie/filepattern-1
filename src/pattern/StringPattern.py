from . import backend
import logging

class StringPattern:

    def __init__(self, filePath: str, pattern: str, debug: bool =False):
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

        if(debug):
            logging.basicConfig(level=logging.DEBUG)
        else:
            logging.basicConfig(level=logging.ERROR)

        try:
            self.file_pattern = backend.StringPattern(filePath, pattern)
        except RuntimeError as e:
            error = "No directory found. Invalid path " + filePath
            logging.error(error)
            exit(1)

    #def file_pattern_to_regex(self) -> None:
        """
        Maps file pattern to equivalent regular expression.
        
        d -> [0-9]
        c -> [a-zA-Z]
        + -> +
        """
     #   self.file_pattern.filePatternToRegex()

    def print_valid_files(self) -> None:
        """
        Prints out files that match the file pattern to the console.
        """
        
        self.file_pattern.printValidFiles()
    
    def match_files(self, group_by: str="") -> None:
        """
        Comapres files to file pattern and stores file names that match
        the file pattern.

        :param bool cut_path: Cuts the path off of the filename, leaving only the filename (otional, defaults to true)
        e.g. /home/usr/file.txt -> file.txt when true
        """
        try: 
            self.file_pattern.matchFiles(group_by)
            #logging.debug(self.file_pattern.getRegexPattern())
            #logging.debug(self.file_pattern.getValidFiles())
        except ValueError as e: 
            logging.error(e)
            exit(1)
            

    def get_files_greater_than(self, file_name: str) -> list:
        """
        Returns list of files alphabetically greater than the provided filename.
        :return: list of files greater than input filename
        """

        return self.file_pattern.getFilesGreaterThan(file_name)

    def get_files_greater_than_sort(self, file_name: str) -> list:
        """
        Sorts files alphabetically and returns files alphabetically greater than file_name
        :return: sorted list of files greater than input filename 
        """

        return self.file_pattern.getFilesGreaterThanSort(file_name)
    
    def sort_files(self) -> None:
        """
        Sorts the valid files alphabetically.
        """
        self.file_pattern.sortFiles()
        logging.debug(self.file_pattern.getValidFiles())

    def set_pattern(self, pattern: str) -> None:
        """
        Sets the file pattern parameter.

        :param str pattern: filepattern
        """
        self.file_pattern.setPattern(pattern)

    def __iter__(self):
        for file in self.file_pattern.validFiles.__iter__():
            yield file
        
