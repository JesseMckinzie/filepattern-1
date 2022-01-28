from . import backend

class InternalFilePattern:
    """
    In memory version of filepattern.

    FilePattern iterates over a directory, matching filenames to a pattern. 
    """


    def __init__(self, path: str, pattern: str, recursive: bool=False):
        """
        Constructor of the FilePattern class.

        Creates a new file pattern object given a path to a 
        directory of files and a pattern.

        :param str path: path to directory 
        :param str pattern: file pattern
        :param bool debug: debug mode on or off. When true, debug statements are printed to the console

        Valid patterns are d, c, and +, where d is a digit, c is
        a character, and + means an arbitrary number of the pattern it 
        acts on. 

        Example: The pattern files_x{row:ddd}_y{col:ddd}_{channel: c+}.ome.tif
        would match files with 3 digits after x, 3 digits after y, and then an 
        arbitrary number of characters. 
        """
        try:
            self._file_pattern = backend.FilePattern(path, pattern, recursive)
            print("creating object")
        except Exception as e:
            print(e)

    def print_valid_files(self) -> None:
        """
        Prints out files that match the file pattern to the console.
        """
        
        self._file_pattern.printValidFiles()
    
    def match_files(self) -> list:
        """
        Comapres files to file pattern and stores file names that match
        the file pattern.

        :param bool cut_path: Cuts the path off of the filename, leaving only the filename (otional, defaults to true)
        e.g. /home/usr/file.txt -> file.txt when true
        """
        try: 
            self._file_pattern.matchFiles(True)
        except ValueError as e: 
            print(e)

    def get_matching(self, mapping) -> list:
        """
        Returns variables matching a specific value

        :param str variables: variables to be matched e.g. variables="variable1=value1, variable2=value2"

        :return list: list of matching files
        """
        try:

            return self._file_pattern.getMatching(mapping)
        except ValueError as e:
            print(e)
            

    def get_files_greater_than(self, file_name: str) -> list:
        """
        Returns list of files alphabetically greater than the provided filename.
        :return: list of files greater than input filename
        """

        return self._file_pattern.getFilesGreaterThan(file_name)

    def get_files_greater_than_sort(self, file_name: str) -> list:
        """
        Sorts files alphabetically and returns files alphabetically greater than file_name
        :return: sorted list of files greater than input filename 
        """

        return self._file_pattern.getFilesGreaterThanSort(file_name)
    
    def sort_files(self) -> None:
        """
        Sorts the valid files alphabetically.
        """
        self._file_pattern.sortFiles()

    def set_pattern(self, pattern: str) -> None:
        """
        Sets the file pattern parameter.

        :param str pattern: filepattern
        """
        self._file_pattern.setPattern(pattern)

    def group_by(self, group: str) -> None:
        """
        Groups valid files by given parameter

        :param str group: variable to group by
        """
        self._file_pattern.groupBy(group)

    def get_occurences(self, mapping: list):

        return self._file_pattern.getOccurences(mapping)
    
    def get_unique_values(self, vec) -> list:

        return self._file_pattern.getUniqueValues(vec)

    def output_name(self, files):
        return self._file_pattern.outputName(files)

    def infer_pattern(path: str, variables: str='') -> str:
        return backend.InternalPattern.inferPattern(files, variables)

    def infer_pattern(files: list, variables: str='') -> str:
        return backend.InternalPattern.inferPattern(files, variables)

    def __call__(self, group_by=None):
        if(group_by is not None):
            self._file_pattern.groupBy(group_by)
        
        return self

    def __iter__(self):
        """
        Returns iterator of valid files

        If fp is a filepattern object, call as "for file in fp()"
        The variables mapped to values are in file.first
        The filepath is in file.second
        """
        return self._file_pattern.__iter__()
        