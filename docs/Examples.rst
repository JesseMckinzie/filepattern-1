<h2 id="filepattern-section"> FilePattern </h2> 

FilePattern iterates over a directory, matching filenames to a suplied ```filepattern```. The syntax of the ```filepattern``` is best described by example. Consider a direcotry
containing the following files, 

.. code-block:: bash
    img_r001_c001_DAPI.tif
    img_r001_c001_TXREAD.tif
    img_r001_c001_GFP.tif


In each of these filenames, there are three descriptors of the image: the row, the column, and the channel. To match these files, the pattern ```img_r{r:ddd}_c{c:ddd}_{channel:c+}``` can be used. In this pattern, the nameed groups are contained within the curly brackets, where the variable name is before the colon and the value is after the colon. For the value, the descriptors `d` and `c` are used, which represent a digit and a character, respectively. In the example pattern, three `d`'s are used to catpure three digits. The `+` after `c` denotes that one or more characters will be captured, equivelantly to `[a-zA-z]+` in a regular expression. The `+` symbol may be used after either `d` or `c`. 

To have `filepattern` guess what the pattern is for a directory, the static method `infer_pattern` can be used:

.. code-block:: python
    from pattern import FilePattern as fp 

    path = 'path/to/directory'

    pattern = fp.FilePattern.infer_pattern(filepath)

    print(pattern)


The result is:

.. code-block:: bash
    img_r00{r:d}_c00{c:d}_{t:c+}.tif


Note that the `infer_pattern` method is also included in `VectorPattern` and `StringPattern` to guess the patterns from stitching vectors and text files, respectively. 

To retrieve the matched files, an iterator is called on the `FilePattern` object, as shown below. A user specified custom pattern, such as the one below, or the guessed pattern can be
as input in the constructor.

.. code-block:: python
    from pattern import FilePattern as fp
    import pprint

    filepath = "path/to/directory"

    pattern = "img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif"

    files = fp.FilePattern(filepath, pattern)

    for file in files(): 
        pprint.pprint(file)

The output is:
.. code-block:: bash
    ({'c': 1, 'channel': 'DAPI', 'r': 1},
    ['path/to/direcotry/img_r001_c001_DAPI.tif'])
    ({'c': 1, 'channel': 'TXREAD', 'r': 1},
    ['path/to/direcotry/img_r001_c001_TXREAD.tif'])
    ({'c': 1, 'channel': 'GFP', 'r': 1},
    ['path/to/direcotry/img_r001_c001_GFP.tif'])


As shown in this example, the output is a tuple where the first member is a map between the group name supplied in the pattern and the value of the group for each file name. The second member of the tuple is a vector containing the path to the matched file. The second member is stored in a vector for the case where a directory is supplied with multiple subdirectories. In this case, a third optional parameter can be passed to the constructor. If the third parameter is set to `True`, a recursive directory iterator will be used, which iterates over all subdirectories. If the basename of two files from two different subdirectories match, ```filepattern``` will add the path of the file to the vector in the existing tuple rather than creating a new tuple. For example, consider the directory with the structure 

.. code-block:: bash
    /root_directory
        /DAPI
            img_r001_c001.tif
        /GFP
            img_r001_c001.tif
        /TXREAD
            img_r001_c001.tif


In this case, the subdirectories are split by the channel. Recursive matching can be used as shown below.
.. code-block:: python
    from pattern import FilePattern as fp
    import pprint

    filepath = "path/to/root/directory"

    pattern = "img_r{r:ddd}_c{c:ddd}.tif"

    files = fp.FilePattern(filepath, pattern, recursive=True)

    for file in files(): 
        pprint.pprint(file)


The output of this case is:

.. code-block:: bash
    ({'c': 1, 'r': 1},
    ['path/to/root/direcotry/DAPI/img_r001_c001.tif',
    'path/to/root/direcotry/GFP/img_r001_c001.tif',
    'path/to/root/direcotry/TXREAD/img_r001_c001.tif'])


<h3 id="group-by"> Group By </h3>

Say the images need to be processed in a specific order, for example by the row number. With the directory 

.. code-block:: bash
    img_r001_c001_DAPI.tif
    img_r002_c001_DAPI.tif
    img_r001_c001_TXREAD.tif
    img_r002_c001_TXREAD.tif
    img_r001_c001_GFP.tif
    img_r002_c001_GFP.tif


the images can be returned in groups where `r` is held constant by passing the parameter ```group_by='r'``` to the object iterator.

.. code-block:: python
    from pattern import FilePattern as fp
    import pprint

    filepath = "path/to/directory"

    pattern = "img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif"

    files = fp.FilePattern(filepath, pattern)

    for file in files(group_by='r'): 
        pprint.pprint(file)


The output is:
.. code-block:: bash
    [({'c': 1, 'channel': 'DAPI', 'file': 0, 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_DAPI.tif']),
    ({'c': 1, 'channel': 'TXREAD', 'file': 0, 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_TXREAD.tif']),
    ({'c': 1, 'channel': 'GFP', 'file': 0, 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_GFP.tif'])]
    [({'c': 1, 'channel': 'DAPI', 'file': 0, 'r': 2},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r002_c001_DAPI.tif']),
    ({'c': 1, 'channel': 'GFP', 'file': 0, 'r': 2},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r002_c001_GFP.tif']),
    ({'c': 1, 'channel': 'TXREAD', 'file': 0, 'r': 2},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r002_c001_TXREAD.tif'])]


<h3 id="get-matching"> Get Matching </h3>

To get files where the variable matches a value, the ```get_matching``` method is used. For example, if only files from the TXREAD channel are needed, ```get_matching(channel=['TXREAD']``` is called. 

.. code-block:: python
    filepath = "/home/ec2-user/Dev/FilePattern/data/example"

    pattern = "img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif"

    files = fp.FilePattern(filepath, pattern)

    matching = files.get_matching(channel=['TXREAD'])

    pprint.pprint(matching)


The output is:
.. code-block:: bash
    [({'c': 1, 'channel': 'TXREAD', 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_TXREAD.tif']),
    ({'c': 1, 'channel': 'TXREAD', 'r': 2},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r002_c001_TXREAD.tif'])]


## StringPattern
StringPattern contains all the functionalility of FilePattern, except it takes in a text file as an input rather than a directory and matches each line to the pattern. For example, a text file containing
.. code-block:: bash
    img_r001_c001_DAPI.tif
    img_r001_c001_TXREAD.tif
    img_r001_c001_GFP.tif


can be matched to the pattern ```img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif``` with:

.. code-block:: python
    from pattern import StringPattern as sp
    import pprint

    filepath = "path/to/file.txt"

    pattern = "img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif"

    files = sp.StringPattern(filepath, pattern)

    for file in files(): 
        pprint.pprint(file)



The ouput is:

.. code-block:: bash
    ({'c': 1, 'channel': 'DAPI', 'r': 1}, 
    ['img_r001_c001_DAPI.tif'])
    ({'c': 1, 'channel': 'TXREAD', 'r': 1}, 
    ['img_r001_c001_TXREAD.tif'])
    ({'c': 1, 'channel': 'GFP', 'r': 1}, 
    ['img_r001_c001_GFP.tif']`)


`StringPattern` also contains the [group_by](#group-by) and [get_matching](#get-matching) functionality as outlined in the [FilePattern](#filepattern-section) section. 

## VectorPattern

`VectorPattern` is a class in `filepattern` which takes in a stitching vector as input rather than a directory. A stitching vector, contained within a text file, has the following form,

.. code-block:: bash
    file: x01_y01_wx0_wy0_c1.ome.tif; corr: 0; position: (0, 0); grid: (0, 0);
    file: x02_y01_wx0_wy0_c1.ome.tif; corr: 0; position: (3496, 0); grid: (3, 0);
    file: x03_y01_wx0_wy0_c1.ome.tif; corr: 0; position: (6992, 0); grid: (6, 0);
    file: x04_y01_wx0_wy0_c1.ome.tif; corr: 0; position: (10488, 0); grid: (9, 0);


`VectorPattern` is called from `filepattern` with 

.. code-block:: python
    from pattern import VectorPattern as vp 

    filepath = 'path/to/stitching/vector.txt'

    pattern = 'x0{x:d}_y01_wx0_wy0_c1.ome.tif'

    files = vp.VectorPattern(filepath, pattern)

    for file in files():
        pprint.pprint(files)


The output is:
.. code-block:: bash
    ({'correlation': 0, 'gridX': 0, 'gridY': 0, 'posX': 0, 'posY': 0, 'x': 1},
    ['x01_y01_wx0_wy0_c1.ome.tif'])
    ({'correlation': 0, 'gridX': 3, 'gridY': 0, 'posX': 3496, 'posY': 0, 'x': 2},
    ['x02_y01_wx0_wy0_c1.ome.tif'])
    ({'correlation': 0, 'gridX': 6, 'gridY': 0, 'posX': 6992, 'posY': 0, 'x': 3},
    ['x03_y01_wx0_wy0_c1.ome.tif'])
    ({'correlation': 0, 'gridX': 9, 'gridY': 0, 'posX': 10488, 'posY': 0, 'x': 4},
    ['x04_y01_wx0_wy0_c1.ome.tif'])

As shown in the output, `VectorPattern` not only captures the specified variables from the pattern, but also captures the variables supplied in the stitching vector. 

## ExternalFilePattern

`ExternalFilePattern` is an external memory version of `filepattern`, i.e. it utilizes disk memory along with main memory. It has the same functionality as FilePattern, however it takes in an addition parameter called `block_size`, which limits the amount of main memory used by `filepattern`. Consider a directory containing the files:

.. code-block:: bash
    img_r001_c001_DAPI.tif
    img_r001_c001_TXREAD.tif
    img_r001_c001_GFP.tif


`ExternalFilePattern` can be used to processes this directory with only one file in memory as:

.. code-block:: python
    from pattern import FilePattern as fp
    import pprint

    filepath = "path/to/directory"

    pattern = "img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif"

    files = fp.FilePattern(filepath, pattern, block_size="125 B")


    for file in files():
        pprint.pprint(file)
    

The output from this example is:

.. code-block:: bash
    ({'c': 1, 'channel': 'DAPI', 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_DAPI.tif'])
    ({'c': 1, 'channel': 'TXREAD', 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_TXREAD.tif'])
    ({'c': 1, 'channel': 'GFP', 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_GFP.tif'])

Note that the ```block_size``` argument is provided in bytes (B) in this example, but also has the options for kilobytes (KB), megabytes (MB), and gigabytes (GB).

<h3 id="group-by-external"> Group by </h3>

`ExternalFilePattern`contains the [group_by](#group-by) functionalility as described in [FilePattern](#filepattern). The output of `group_by` is the same as `FilePatten`.

.. code-block:: python
    for file in files(group_by="r"):
        pprint.pprint(file)


The output remains identical to `FilePattern`.

<h3 id="get-matching-external"> Get Matching </h3>

`ExternalFilePattern` also contains the `get_matching` functionality. To call `get_matching`, the following is used:

..code-block:: python
    files.get_matching(channel=['TXREAD'])

    for matching in files.get_matching_block()
        pprint.pprint(matching)

where the output is returned in blocks of `block_size`. The output is:

.. code-block:: bash
    ({'c': 1, 'channel': 'TXREAD', 'r': 1},
    ['/home/ec2-user/Dev/FilePattern/data/example/img_r001_c001_TXREAD.tif'])


## ExternalStringPattern and ExternalVectorPattern

`StringPattern` and `VectorPattern` also contain external memory versiosn, which can be called the with the same method as `ExternalFilePattern`, with the exception of calling the `StringPattern` or  `VectorPattern` constructors.
