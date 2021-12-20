from . import backend

class FilePattern:
    """
    Filepattern iterates over a provided directory, matching filenames in the directory to a 
    user specified pattern. 
    """

    def __init__(self, path: str, pattern: str, block_size: str="50 MB", recursive: bool=False):
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
            self._file_pattern = backend.ExternalFilePattern(path, pattern, block_size, recursive)
        except RuntimeError as e:
            print(e)

    def print_valid_files(self) -> None:
        """
        Prints out files that match the file pattern to the console.
        """

        self._file_pattern.printFiles()
    
    def get_valid_files_block(self) -> list:
        return self._file_pattern.getValidFilesBlock()

    def match_files(self, group_by: str="") -> None:
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

    def get(self):
       return self._file_pattern.get()

    def length(self):
        return self._file_pattern.currentBlockLength()
            
    def __call__(self, group_by=None):
        if(group_by is not None):
            self._file_pattern.groupBy(group_by)

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
            
        return self._file_pattern.__iter__()
 