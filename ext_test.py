from pattern import ExternalFilePattern as fp 

filepath = "data/Small_Fluorescent_Test_Dataset/image-tiles"

pattern = "img_r00{r:d}_c00{c:d}.tif"

files = fp.FilePattern(filepath, pattern, block_size="1 KB")

while(len(files) != 0):
    for file in files():
        print(file)
