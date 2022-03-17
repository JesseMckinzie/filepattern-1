# from . import FilePatternBackend, StringPattern, VectorPattern
from . import backend
#import backend
import re


class PatternObject:
    def __init__(self, file_pattern, block_size):
        self._file_pattern = file_pattern
        self._block_size = block_size

    def get_matching(self, **kwargs) -> list:
        """Get all filenames matching specific values

        Args:
            **kwargs: One of the variables contained in the pattern

        Returns:
            List of matching files
        """

        mapping = []
        for key, value in kwargs.items():
            mapping.append((key, value))

        if self._block_size == "":
            try:
                return self._file_pattern.getMatching(mapping)
            except Exception as e:
                print(e)
        else:
            return self._get_matching_out_of_core(mapping)

    def _get_matching_out_of_core(self, mapping):
        try:
            self._file_pattern.getMatching(mapping)

            while True:
                matching = self._get_matching_block()
                if len(matching) == 0:
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

    def get_occurences(self, mapping):
        """
        Returns the unique values for each variable along with the number of occurences for each value.

        Args:
            **kwargs: Each keyword arguement must be a variable. If no arguements are supplied, the occurences
            for every variable will be returned.

        Returns:
            Dictionary of variables mapped to values where each value is mapped to the number of occurences.
        """

        return self._file_pattern.getOccurences(mapping)

    def get_unique_values(self, vec) -> list:
        """Returns the unique values for each variable.

        This method returns a dictionary of provided variables to a list of all unique occurences. If no variables are provided,
        all variables will be returned.

        Args:
            **args: Variables to get the occurences of. All variables will be returned if no arguments are provided.

        Returns:
            Dictionary of variables mapped to values.
        """

        return self._file_pattern.getUniqueValues(vec)

    def output_name(self, files: list = []) -> str:
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

        return self._file_pattern.outputName(files)

    def __call__(self, group_by=""):
        """Iterate thorugh files parsed using a filepattern

        This method returns an iterable of filenames matched to the filepattern. If
        a group_by variable is provided, lists of files where the variable is held constant are
        returned on each call.

        Args:
            group_by: List of variables to group filenames by.
        """
        if self._block_size == "":
            if group_by != "":
                self._file_pattern.groupBy(group_by)
            return self

        if group_by != "":
            self._file_pattern.setGroup(group_by)

        return self

    def __len__(self):
        return self._file_pattern.currentBlockLength()

    def __iter__(self):
        """Returns an iterator of files matched to the pattern"""
        if self._block_size == "":
            for file in self._file_pattern.__iter__():
                yield file
        else:
            while True:

                for block in self._file_pattern.__iter__():

                    if len(self) == 0:
                        break

                    yield block

                if len(self) == 0:
                    break
                
    def __getitem__(self, key):
        print(key)
        if(type(key) == int): return self._file_pattern.getItem(key)
        if(type(key) == list): return self._file_pattern.getItemList(key)
        
        slc = [key.start, key.stop, key.step]
        if(slc[0] == None): slc[0] = 'None'
        if(slc[1] == None): slc[1] = 'None'
        if(slc[2] == None): slc[2] = 'None'
        
        return self._file_pattern.getSlice(slc)


class FilePattern(PatternObject):
    """
    Class to create a FilePattern object.

    This class take in in 4 arguements: path, pattern, block_size, and recurisve. For the path,
    either a path to a directory, text file, or stitching vector may be provided. ``filepattern2`` will
    then match the filenames in the directory, or each line of the text file, to the provided ``pattern``.

    The ``block_size`` parameter allows for out of core processing, which consume ``block_size`` amount of memory at most.

    The ``recursive`` parameter enables recursive iteration of subdirectories when a directory is passed as ``path``. In
    this case ``filepattern2`` will iteratre over the subdirectories, storing filenames with the same basename in the same
    group.


    Args:
            path: Path to directory or text file
            pattern: Pattern to compare each filename to
            block_size: Maximum amount of RAM to consume at once. Defaults to "".
            recursive: Iterate over subdirectories. Defaults to False.
    """

    def __init__(
        self,
        path: str,
        pattern: str = "",
        block_size: str = "",
        recursive: bool = False,
    ):
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

        if path.endswith(".txt"):

            with open(path) as infile:
                line = infile.readline().rstrip()

            if re.match(r"file\: .+?; corr\: .+?; position\: .+?; grid\: .+?;", line):
                if block_size == "":
                    self._file_pattern = backend.InternalVectorPattern(path, pattern)
                else:
                    self._file_pattern = backend.ExternalVectorPattern(
                        path, pattern, block_size
                    )
                # self._file_pattern = backend.InternalVectorPattern(path, pattern) if block_size == '' \
                #                    else backend.ExternalVectorPattern(path, pattern, block_size=block_size)
            else:
                if block_size == "":
                    self._file_pattern = backend.StringPattern(path, pattern)
                else:
                    self._file_pattern = backend.ExternalStringPattern(
                        path, pattern, block_size
                    )
                # self._file_pattern = backend.StringPattern(path, pattern) if block_size == '' \
                #                    else backend.ExternalStringPattern(path, pattern, block_size=block_size)
        else:
            if block_size == "":
                self._file_pattern = backend.FilePattern(path, pattern, recursive)
            else:
                self._file_pattern = backend.ExternalFilePattern(
                    path, pattern, block_size, recursive
                )
            # self._file_pattern = backend.FilePattern(path, pattern, recursive) if block_size == '' \
            #                    else backend.ExternalFilePattern(path, pattern, recursive = recursive, block_size=block_size)

        super().__init__(self._file_pattern, block_size)

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

        return super(FilePattern, self).get_occurences(mapping)

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

        return super().get_unique_values(vec)

    def output_name(self, files: list = []) -> str:
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

        return super().output_name(files)
