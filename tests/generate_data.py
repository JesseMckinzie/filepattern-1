import os

MAX = 3

directory = 'test_data'
root_directory = os.path.dirname(os.path.realpath(__file__))
path = os.path.join(root_directory, directory)
data_path = path + '/data'
recursive_path = path + '/recursive_data'

sp_data = path + "/sp_data.txt"

try:
    os.mkdir(path)
    os.mkdir(data_path)
    os.mkdir(recursive_path)
    print('Data directory created at ' + path)
except FileExistsError as e:
    print("Data directory already exists")

name = ""
channels = ['DAPI', 'TXREAD', 'GFP']

infile = open(sp_data, 'w')

for channel in channels:
    current_path = recursive_path + '/' + channel
    try:
        os.mkdir(current_path)
    except FileExistsError as e:
        print("Data directory already exists")

    for i in range(0, MAX):

        str_i = str(i).zfill(3)

        for j in range(0, MAX):

            str_j = str(j).zfill(3)

            data_name = 'img_r{}_c{}_{}.tif'.format(str_i, str_j, channel)
            infile.write(data_name + '\n')
            f = open(data_path + '/' + data_name, 'w+')
            f.close()

            recursive_name = 'img_r{}_c{}.tif'.format(str_i, str_j)
            f = open(current_path + '/' + recursive_name, 'w+')
            f.close()
         
infile.close()
print("Files generated.")
    