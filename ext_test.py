from pattern import ExternalFilePattern as fp 

filepath = "data/Small_Fluorescent_Test_Dataset/image-tiles"

pattern = "img_r00{r:d}_c00{c:d}.tif"

files = fp.FilePattern(filepath, pattern, block_size="1 KB")
block = files.get_valid_files_block()

size = len(block)
while (len(block) != 0):
    print('---------------------------------')
    for file in block:
        print(file)
    print('---------------------------------')

    block = files.get_valid_files_block()
