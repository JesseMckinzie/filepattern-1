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

        Args:
            path: Path to directory
            pattern: Pattern to compare each filename to
            block_size: Maximum amount of RAM to consume at once. Defaults to "".
            recursive: Iterate over subdirectories. Defaults to False.
        """
        if(block_size == ""): 
            self._file_pattern = InternalFilePattern.InternalFilePattern(path, pattern, recursive=recursive)
        else: 
            if(recursive is True):
                print("Recursive matching is not yet supoorted")
            self._file_pattern = ExternalFilePattern.ExternalFilePattern(path, pattern, block_size=block_size, recursive=False)

    def get_matching(self, **kwargs) -> list:
        """Get all filenames matching specific values
        

        Args:
            **kwargs: One of the variables contained in the pattern 

        Returns:
            List of matching files
        """
        try:
            mapping = []
            for key, value in kwargs.items():
                mapping.append((key, value))

            return self._file_pattern.get_matching(mapping)
        except ValueError as e:
            print(e)

    def get_occurences(self, **kwargs):
        """
        Returns the unique values for each variable along with the number of occurences for each value.
        
        Args: 
            **kwargs: Each keyword arguement must be a variable. If no arguements are supplied, the occurences
            for every variable will be returned. 
            
        Returns:
            Dictionary of variables mapped to values where each value is mapped to the number of occurences.
        """
    
        mapping = []
        for key, value in kwargs.items():
            mapping.append((key, value))

        return self._file_pattern.get_occurences(mapping)
    
    def get_unique_values(self, *args) -> list:
        """Returns the unique values for each variable.
        
        This method returns a dictionary of provided variables to a list of all unique occurences. If no variables are provided,
        all variables will be returned.   
        
        Args: 
            **args: Variables to get the occurences of. All variables will be returned if no arguments are provided.
            
        Returns:
            Dictionary of variables mapped to values.
        """
        vec = []
        for str in args:
            vec.append(str)

        return self._file_pattern.get_unique_values(vec)

    def output_name(self, files: list=[]) -> str:
        """Returns a single filename that captures variables from a list of files.
        
        Given a list of files, this method will return a single filename that captures the variables from each
        file in the list. If a variable is constant through the list, the variable value will be in the returned 
        name. If a variable is not constant, the minimum and maximum values will appear in the returned name in
        the form "(min-max)". 
        
        Args:
            files: List of files to get a single filename of.
        
        Returns:
            A string that captures the variable values from each file in files.

        """

        return self._file_pattern.output_name(files)
    

    def infer_pattern(path: str="", files: list=[], variables: str="", block_size: str=""):
        """Returns a guess of a pattern given path to a directory of files or a list of files.
        

    
        Args:
            path: The path to a directory of files. Defualts to "".
            files: A list of files. Defualts to [].
            variables: A string of variables. If an empty string, variable names will be provided. Defaults to "".
            block_size: An string that specifies a maximum amount of RAM to consume. If "", no limit will be imposed. Defaults to "". 
        
        Returns:
            A string that is a guess of the pattern for the supplied filenames.
        """
        
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

    
    def __call__(self, group_by=None):
        """Iterate thorugh files parsed using a filepattern 
        
        This method returns an iterable of filenames matched to the filepattern. If 
        a group_by variable is provided, lists of files where the variable is held constant are 
        returned on each call. 
        
        Args: 
            group_by: List of variables to group filenames by.
        """
        
        return self._file_pattern.__call__(group_by)

    def __iter__(self):
        """Returns an iterator of files matched to the pattern
        """
        return self._file_pattern.__iter__()
        