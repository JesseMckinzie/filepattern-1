import math
import os

DATA_SIZES = [100, 1000, 10000, 100000, 1000000]

directory = 'test_data'
root_directory = os.path.dirname(os.path.realpath(__file__))
path = os.path.join(root_directory, directory)

try:
    os.mkdir(path)
    print('Data directory created at ' + path)
except FileExistsError as e:
    print("Data directory already exists")

name = ""

for MAX in DATA_SIZES:
    data_path = path + '/data' + str(MAX)

    try:
        os.mkdir(data_path)
        print('Data directory created at ' + data_path)
    except FileExistsError as e:
        print("Data directory already exists")

    limit = math.ceil(math.sqrt(MAX)) 

    length = 0

    for i in range(0, limit):

        str_i = str(i).zfill(3)

        for j in range(0, limit):

            str_j = str(j).zfill(3)
            data_name = 'img_r{}_c{}.tif'.format(str_i, str_j)
            f = open(data_path + '/' + data_name, 'w+')
            f.close()
            length += 1
    
    print(str(length) + " files generated in directory " + data_path + ".")
