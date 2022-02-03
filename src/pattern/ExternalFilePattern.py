from . import backend

class ExternalFilePattern:
    """
    External memory version of filepattern. 

    FilePattern iterates over a provided directory, matching filenames in the directory to a 
    user specified pattern only utilizing the amount of memory specifed in the the block_size 
    parameter of the constructor. 
    """

    def __init__(self, path: str, pattern: str, block_size: str="50 MB", recursive: bool=False):
        """
        Constructor of the FilePattern class.

        Creates a new file pattern object given a path to a directory of files and a pattern.
        Valid patterns are d, c, and +, where d is a digit, c isa character, and + means an 
        arbitrary number of the pattern it acts on. 

        Example: The pattern files_x{row:ddd}_y{col:ddd}_{channel: c+}.ome.tif would match files 
        with 3 digits after x, 3 digits after y, and then an 
        arbitrary number of characters. 

        @param str path: path to directory 
        @param str pattern: file pattern
        @param bool debug: debug mode on or off. When true, debug statements are printed to the console

        """
        try:
            self._file_pattern = backend.ExternalFilePattern(path, pattern, block_size, recursive)
        except RuntimeError as e:
            print(e)


    def get_matching(self, mapping: list) -> None:
        """
        Returns variables matching a specific value

        @param str variables Variables to be matched e.g. variables="variable1=value1, variable2=value2"

        @return list List of matching files
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

        @return list Block of matching files.
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
    
    def infer_pattern(path: str, variables: str, block_size: str):
        
        return backend.ExternalPattern.inferPattern(path, variables, block_size)

    def __call__(self, group_by=None):
        if(group_by is not None):
            self._file_pattern.setGroup(group_by)

        return self

    def __len__(self):
        return self._file_pattern.currentBlockLength()
    
    def group_length(self):
        return self._file_pattern.getGroupLength()

    def __iter__(self):
        """
        Returns iterator of valid files

        If fp is a filepattern object, call as "for file in fp()"
        The variables mapped to values are in file.first
        The filepath is in file.second
        """
        while(True):
            
            """
            while(True):
            
                block = [];
                for file in self._file_pattern.__iter__():
                    block.append(file)
                
                if(len(block) == 0):
                    break   
                
                yield ((self._file_pattern.group, block[0][0][self._file_pattern.group]), block)
            
            """
            for block in self._file_pattern.__iter__():
                
                if(len(self) == 0):
                    break
                
                yield block
                
            if(len(self) == 0):
                break
            

            
            
