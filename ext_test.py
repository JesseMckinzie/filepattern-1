from pattern import ExternalFilePattern as fp 

filepath = "/home/ec2-user/Dev/PolusData/polus-data/polus/images/TissueNet/standard/train/intensity"

#pattern = "img_r00{r:d}_c00{c:d}.tif"

pattern = "p{p:d}_y{y:d}_r{r:ddd}_c{c:d}.ome.tif"

files = fp.FilePattern(filepath, pattern, block_size="50 KB")
block = files.get_valid_files_block()
print(len(block))
"""
print('---------------------------------')
for file in block:
    print(file)
print('---------------------------------')
"""
while (len(block) != 0):
    
    print('---------------------------------')
    for file in block:
        print(file)
    print('---------------------------------')
    block = files.get_valid_files_block()

#print(files.get())

"""
i = 0
for file in files():
    if(file is not None):
        print(file.second)
    if(i == 5):
        break
    i += 1
"""

"""
print("\nGrouped by y: ")
for file in files(group_by='r'): 
    print(file.first)
    print(file.second)
    print()


print("File matching r=1 and c=2:")
for file in files.get_matching("r=1, c=2"):
    print(file.first)
"""