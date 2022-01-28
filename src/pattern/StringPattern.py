from . import ExternalStringPattern, InternalStringPattern

class StringPattern:
    def __init__(self, path: str, pattern: str, block_size: str=""): 
        if(block_size == ""): 
            self._file_pattern = InternalStringPattern.InternalStringPattern(path, pattern)
        else: 
            self._file_pattern = ExternalStringPattern.ExternalStringPattern(path, pattern, block_size=block_size)

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
            return InternalStringPattern.InternalStringPattern.infer_pattern(files, variables)
        else:
            if (block_size == ""):
                return InternalStringPattern.InternalStringPattern.infer_pattern(path, variables)
            else:
                return ExternalStringPattern.ExternalStringPattern.infer_pattern(path, variables, block_size)

    
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
        