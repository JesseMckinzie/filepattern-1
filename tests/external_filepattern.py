from pattern import ExternalFilePattern as efp
import os
import pprint

directory = 'test_data/data'
root_directory = os.path.dirname(os.path.realpath(__file__))

path = os.path.join(root_directory, directory)

pattern = 'img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif'

files = efp.FilePattern(path, pattern, block_size="125 KB")

length = 0
while(True):

    for file in files(group_by="r"):
        pprint.pprint(file)

    print('-----------------')
    
    if(len(files) == 0):
        break

files.get_matching(r=[1])

while(True):
    matching = files.get_matching_block()

    if(len(matching) == 0):
        break

    pprint.pprint(matching)
    pprint.pprint('-------------------')
