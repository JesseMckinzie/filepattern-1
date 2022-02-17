from . import FilePattern, StringPattern, VectorPattern 
import re

class Pattern:
    def __init__(self, path: str, pattern: str='', block_size: str ='', recursive: bool=False):
        """Constructor of the Pattern class. The path arugment can either be a directory, a text file,
        or a stitching vector. Passing in the optional arguement `block_size` will 
        create an ExternalFilePattern object, which will process the directory in blocks which comsume less
        than or equal to `block_size` of memory. 
        
        Just the path may be passed in the pattern is contained within the path. In this case,
        the names of the subdirectories are captured if they are named is the same manner as the pattern.
        For example, if just the path 'path/to/files/{channel: c+}/img_r{r:d+}_c{c:d+}.tif' is passed, 
        the names of the channel subfolders will be captured for each file. 

        Args:
            path: Path to directory or text file
            pattern: Pattern to compare each filename to
            block_size: Maximum amount of RAM to consume at once. Defaults to "".
            recursive: Iterate over subdirectories. Defaults to False.
        """
        
        if(path.endswith('.txt')):
            with open(path) as infile:
                line = infile.readline().rstrip()
            
            if(re.match(r'file\: .+?; corr\: .+?; position\: .+?; grid\: .+?;', line)):
                self._file_pattern = VectorPattern.VectorPattern(path, pattern, block_size=block_size)
            else:
                self._file_pattern = StringPattern.StringPattern(path, pattern, block_size=block_size)
        else:
            self._file_pattern = FilePattern.FilePattern(path, pattern, block_size=block_size, recursive=recursive)

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
        if(path.endswith('.txt')):
            with open(path) as infile:
                line = infile.readline().rstrip()
            
            if(re.match(r'file\: .+?; corr\: .+?; position\: .+?; grid\: .+?;', line)):
                return VectorPattern.VectorPattern.infer_pattern(path=path, variables=variables, block_size=block_size)
            else:
                return StringPattern.StringPattern.infer_pattern(path=path, files=files, variables=variables, block_size=block_size)
        else:
            return FilePattern.FilePattern.infer_pattern(path=path, files=files, variables=variables, block_size=block_size)
        

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
        