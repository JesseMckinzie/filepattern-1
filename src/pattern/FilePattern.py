from . import ExternalFilePattern, InternalFilePattern

class FilePattern:
    """
    Class to create either a `InternalFilePattern` or `ExternalFilePattern` object based on
    whether the arguement `block_size` is passed to the constrctor. If `block_size` is a nonempty 
    string, an `InternalFilePattern` object is created. Otherwise, an `ExternalFilePattern` object is
    created, which processes the input in blocks with a memory footprint less than `block_sizes`.
    
    """
    
    def __init__(self, path: str, pattern: str, block_size: str="", recursive: bool=False): 
        """Constructor of the FilePattern class. Passing in the optional arguement `block_size` will 
        create an ExternalFilePattern object, which will process the directory in blocks which comsume less
        than or equal to `block_size` of memory. 
        
        The pattern 

        Args:
            path (str): Path to directory
            pattern (str): Pattern to compare each filename to
            block_size (str, optional): Maximum amount of RAM to consume at once. Defaults to "" which processes all files at once.
            recursive (bool, optional): Iterate over subdirectories. Defaults to False.
        """
        if(block_size == ""): 
            self._file_pattern = InternalFilePattern.InternalFilePattern(path, pattern, recursive=recursive)
        else: 
            if(recursive is True):
                print("Recursive matching is not yet supoorted")
            self._file_pattern = ExternalFilePattern.ExternalFilePattern(path, pattern, block_size=block_size, recursive=False)

    def get_matching(self, **kwargs) -> list:
        
        """
        Returns variables matching a specific value

        :param str variables: variables to be matched e.g. variables="variable1=value1, variable2=value2"

        :return list: list of matching files
        """
        try:
            mapping = []
            for key, value in kwargs.items():
                mapping.append((key, value))

            return self._file_pattern.get_matching(mapping)
        except ValueError as e:
            print(e)
            
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
        self._file_pattern.group_by(group)

    def get_occurences(self, **kwargs):
        mapping = []
        for key, value in kwargs.items():
            mapping.append((key, value))

        return self._file_pattern.get_occurences(mapping)
    
    def get_unique_values(self, *args) -> list:
        vec = []
        for str in args:
            vec.append(str)

        return self._file_pattern.get_unique_values(vec)

    def output_name(self, files: list=[]) -> str:

        return self._file_pattern.output_name(files)
    

    def infer_pattern(path: str="", files: list=[], variables: str="", block_size: str=""):
        if(path == "" and files == []):
            raise ValueError('A path or list of files must be provided')
        elif(path != "" and files != []):
            raise ValueError('Pass in only a path or list of files, not both.')
        elif(path == ""):
            return InternalFilePattern.InternalFilePattern.infer_pattern(files, variables)
        else:
            if (block_size == ""):
                return InternalFilePattern.InternalFilePattern.infer_pattern(path, variables)
            else:
                return ExternalFilePattern.ExternalFilePattern.infer_pattern(path, variables, block_size)

    """
    def infer_pattern(files: list, variables: str="") -> str:
        
        return InternalFilePattern.InternalFilePattern.infer_pattern(files, variables)
        """
    
    def __call__(self, group_by=None):
        
        return self._file_pattern.__call__(group_by)

    def __iter__(self):
        """
        Returns iterator of valid files

        If fp is a filepattern object, call as "for file in fp()"
        The variables mapped to values are in file.first
        The filepath is in file.second
        """
        return self._file_pattern.__iter__()
        