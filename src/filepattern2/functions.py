from . import backend
import re

def get_regex(filepattern: str) -> tuple:
    result  = backend.Pattern.getRegex(filepattern)
    return result[0:2]

def infer_pattern(
    path: str = "", files: list = [], variables: str = "", block_size: str = ""
):
    """Returns a guess of a pattern given path to a directory of files or a list of files.

    Args:
        path: The path to a directory of files. Defualts to "".
        files: A list of files. Defualts to [].
        variables: A string of variables. If an empty string, variable names will be provided. Defaults to "".
        block_size: An string that specifies a maximum amount of RAM to consume. If "", no limit will be imposed. Defaults to "".

    Returns:
        A string that is a guess of the pattern for the supplied filenames.
    """
    if path == "" and files == []:
        raise ValueError("A path or list of files must be provided")
    elif path != "" and files != []:
        raise ValueError("Pass in only a path or list of files, not both.")

    if path.endswith(".txt"):
        with open(path) as infile:
            line = infile.readline().rstrip()

        # check if the file is a stitching vector
        if re.match(r"file\: .+?; corr\: .+?; position\: .+?; grid\: .+?;", line):
            if block_size == "":
                if files == []:
                    return backend.VectorPattern.inferPattern(path, variables)
                else:
                    return backend.VectorPattern.inferPattern(files, variables)
            else:
                return backend.ExternalVectorPattern.inferPattern(
                    path, variables, block_size
                )
    if block_size == "":
        if files == []:
            return backend.InternalPattern.inferPattern(path, variables)

        return backend.InternalPattern.inferPattern(files, variables)
    else:
        return backend.ExternalPattern.inferPattern(path, variables, block_size)
