from pattern import FilePattern as fp
import os
import pprint

directory = 'test_data/recursive_data'
root_directory = os.path.dirname(os.path.realpath(__file__))

path = os.path.join(root_directory, directory)

pattern = 'img_r{r:ddd}_c{c:ddd}.tif'

files = fp.FilePattern(path, pattern, recursive=True)


for file in files(): 
    pprint.pprint(file)
print()


print('Files grouped by \'r\'')
for file in files(group_by='r'): 
    pprint.pprint(file)
print()


print('Files matching c=1')
matching = files.get_matching(c=[1])
pprint.pprint(matching)