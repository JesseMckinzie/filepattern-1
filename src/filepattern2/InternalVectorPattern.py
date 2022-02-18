from . import backend

class VectorPattern:

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
            self._file_pattern = backend.InternalVectorPattern(filePath, pattern)
        except RuntimeError as e:
            #error = "No directory found. Invalid path " + filePath
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
            print(e)

    def group_by(self, group: str) -> None:
        """
        Groups valid files by given parameter

        :param str group: variable to group by
        """
        self._file_pattern.groupBy(group)

    def get_occurences(self, mapping):
    
        return self._file_pattern.get_occurences(mapping)
    
    def get_unique_values(self, vec) -> list:

        return self._file_pattern.get_unique_values(vec)

    def output_name(self, files: list=[]) -> str:
        
        return self._file_pattern.outputName(files)
    
    def infer_pattern(path: str, variables: str='') -> str:
        return backend.InternalVectorPattern.inferPattern(files, variables)
    
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
        
