from pattern import ExternalFilePattern as efp
import os
import pprint

directory = 'test_data/data'
root_directory = os.path.dirname(os.path.realpath(__file__))

path = os.path.join(root_directory, directory)

pattern = 'img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif'

files = efp.FilePattern(path, pattern, block_size="500 B")

while(True):

    for file in files():
        pprint.pprint(file)
    
    print('-----------------')
    
    if(len(files) == 0):
        break