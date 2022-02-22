import filepattern2 as fp
import os

try:
    directory = 'test_data'
    root_directory = os.path.dirname(os.path.realpath(__file__))
    path = os.path.join(root_directory, directory)    
    os.mkdir(path)
    
except FileExistsError as e:
    pass

name = ""
channels = ['DAPI', 'TXREAD', 'GFP']

MAX = 1

for channel in channels:
    for i in range(0, MAX):

        str_i = str(i).zfill(3)

        for j in range(0, MAX):

            str_j = str(j).zfill(3)

            data_name = 'img_r{}_c{}_{}.tif'.format(str_i, str_j, channel)
            f = open(path + '/' + data_name, 'w+')
            f.close()

pattern = 'img_r{r:ddd}_c{c:ddd}_{channel:c+}.tif'

data_path = path

files = fp.FilePattern2(data_path, pattern)

results = []
for file in files():
    results.append(file)
    
truth = [({'c': 0, 'channel': 'DAPI', 'r': 0}, ['/home/ec2-user/Dev/FilePattern/test_data/img_r000_c000_DAPI.tif']),
         ({'c': 0, 'channel': 'GFP', 'r': 0}, ['/home/ec2-user/Dev/FilePattern/test_data/img_r000_c000_GFP.tif']),
         ({'c': 0, 'channel': 'TXREAD', 'r': 0}, ['/home/ec2-user/Dev/FilePattern/test_data/img_r000_c000_TXREAD.tif'])]
    
assert results == truth, "Error, results are not correct."

print("Success")
    
