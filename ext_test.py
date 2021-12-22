from pattern import ExternalFilePattern as fp 

filepath = "/home/ec2-user/Dev/temp/data/Small_Fluorescent_Test_Dataset/image-tiles"

pattern = "img_r00{r:d}_c00{c:d}.tif"

files = fp.FilePattern(filepath, pattern, block_size="1000 KB")

while(len(files) != 0):
    for file in files(group_by='r'):
        print(file)
