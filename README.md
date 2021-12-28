# FilePattern
The ``filepattern`` utility is used to store files that follow a pattern, where the pattern is analogous to a simplified regular expression. The need for 
``filepattern`` arises in situations where large amounts of data with a systematic naming convention needs to be filtered by patterns in the naming. For example, one may have
a directory containing segmented images where the name contains information such as the channel, the column value, and the row value. ``filepattern`` provides the ability to 
extract all images containing such a naming pattern, filter by the row or column value, or group files by one of the affermentioned variables. 

## Install
To install filepattern:

1. Clone repository with ```--recursive-submodules```
2. cd to the folder and then run ```pip install .```
  
After installation, use "import pattern" to import the module into Python. The pattern module contains the following classes: 
* [FilePattern](#FilePattern)
* [StringPattern](#StringPattern)
* [ExternalFilePattern](#ExternalFilePattern)

## FilePattern
FilePattern iterates over a directory, matching filenames to a suplied ```filepattern```. The syntax of the ```filepattern``` is best described by example. Consider a direcotry
containing the following files, 

```
img_r001_c001_DAPI.tif
img_r001_c001_TXREAD.tif
img_r001_c001_GFP.tif
```

In each of these filenames, there are three descriptors of the image: the row, the column, and the channel. To match these files, the pattern ```img_r{r:ddd}_c{c:ddd}_{channel:c+}``` can be used. In this pattern, the nameed groups are contained within the curly brackets, where the variable name is before the colon and the value is after the colon. For the value, the descriptors `d` and `c` are used, which represent a digit and a character, respectively. In the example pattern, three `d`'s are used to catpure three digits. The `+` after `c` denotes that one or more characters will be captured, equivelantly to `[a-zA-z]+` in a regular expression. The `+` symbol may be used after either `d` or `c`. 

To retrieve the matched files, an iterator is called on the `FilePattern` object, as shown below.

```python
from pattern import FilePattern as fp
import pprint

filepath = "path/to/direcotry"

pattern = "img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif"

files = fp.FilePattern(filepath, pattern)

for file in files(): 
    pprint.pprint(file)
```
The output is,
```
({'c': 1, 'channel': 'DAPI', 'r': 1},
 ['path/to/direcotry/img_r001_c001_DAPI.tif'])
({'c': 1, 'channel': 'TXREAD', 'r': 1},
 ['path/to/direcotry/img_r001_c001_TXREAD.tif'])
({'c': 1, 'channel': 'GFP', 'r': 1},
 ['path/to/direcotry/img_r001_c001_GFP.tif'])
```

As shown in this example, the output is a tuple where the first member is a map between the group name supplied in the pattern and the value of the group for each file name. The second member of the tuple is a vector containing the path to the matched file. The second member is stored in a vector for the case where a directory is supplied with multiple subdirectories. In this case, a third optional parameter can be passed to the constructor. If the third parameter is set to `True`, a recursive directory iterator will be used, which iterates over all subdirectories. If the basename of two files from two different subdirectories match, ```filepattern``` will add the path of the file to the vector in the existing tuple rather than creating a new tuple. For example, consider the directory with the structure 

```
/root_directory
    /DAPI
        img_r001_c001.tif
    /TXREAD
        img_r001_c001.tif
    /GFP
        img_r001_c001.tif
```

In this case, the subdirectories are split by the channel. Recursive matching can be used as shown below.
```python
from pattern import FilePattern as fp
import pprint

filepath = "path/to/root/direcotry"

pattern = "img_r{r:ddd}_c{c:ddd}.tif"

files = fp.FilePattern(filepath, pattern, True)

for file in files(): 
    pprint.pprint(file)
```

The output of this case is:
```

```


## StringPattern
StringPattern contains all the functionalility of FilePattern, except it takes in a txt file as an input rather than a directory.
  
## ExternalFilePattern
ExternalFilePattern has the same functionality as FilePattern, however it processes the input in a specified block size for when the input is too large to store in main memory, or for when the memory use of FilePattern must be restricted.
