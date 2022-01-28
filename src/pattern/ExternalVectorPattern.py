from . import backend

class ExternalVectorPattern:
    """
    External memory version of filepattern. 

    FilePattern iterates over a provided directory, matching filenames in the directory to a 
    user specified pattern only utilizing the amount of memory specifed in the the block_size 
    parameter of the constructor. 
    """

    def __init__(self, path: str, pattern: str, block_size: str="50 MB"):
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
            self._file_pattern = backend.ExternalVectorPattern(path, pattern, block_size)
        except RuntimeError as e:
            print(e)
            exit(1)

    def print_valid_files(self) -> None:
        """
        Prints out files that match the file pattern to the console.
        """

        self._file_pattern.printFiles()

    def get_valid_files_block(self) -> list:
        """
        Returns a list of tuples that contain the matched files that is the size of block_size.
        """
        return self._file_pattern.getValidFilesBlock()

    def match_files(self, group_by: str="") -> None:
        """
        Comapres files to file pattern and stores file names that match
        the file pattern.
        """
        try: 
            self._file_pattern.matchFiles()
        except ValueError as e: 
            print(e)

    def get_matching(self, mapping: list) -> None:
        """
        Returns variables matching a specific value

        :param str variables: variables to be matched e.g. variables="variable1=value1, variable2=value2"

        :return list: list of matching files
        """
        try:
            self._file_pattern.getMatching(mapping)

            while(True):
                matching = self._get_matching_block()
                if(len(matching) == 0):
                    break

                for match in matching:
                    yield match


        except ValueError as e:
            print(e)

    def _get_matching_block(self) -> list:
        """
        Returns block of mathing files of size less than or equal to block_size.

        Must be called after making a call to get_matching.

        :retur list: block of matching files.
        """

        try:
            return self._file_pattern.getMatchingBlock()
        except ValueError as e:
            print(e)

    def get_occurences(self, mapping: list):

        return self._file_pattern.getOccurences(mapping)

    def get_unique_values(self, vec) -> list:

        return self._file_pattern.getUniqueValues(vec)

    def output_name(self, files: list=[]) -> str:
        
        return self._file_pattern.outputName(files)


    def length(self) -> int:
        """
        Returns the length of the current file block.
        """
        return self._file_pattern.currentBlockLength()
    
    def infer_pattern(path: str, variables: str='', block_size: str ='') -> str:
        return backend.ExternalVectorPattern.inferPattern(path, variables, block_size)

    def __call__(self, group_by=None):
        if(group_by is not None):
            self._file_pattern.setGroup(group_by)

        return self

    def __len__(self):
        return self._file_pattern.currentBlockLength()

    def __iter__(self):
        """
        Returns iterator of valid files

        If fp is a filepattern object, call as "for file in fp()"
        The variables mapped to values are in file.first
        The filepath is in file.second
        """
        while(True):
            for block in self._file_pattern.__iter__():
                yield block

            if(len(self) == 0):
                break

    def next(self):
        self._file_pattern.next()
