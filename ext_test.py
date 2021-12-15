from pattern import ExternalFilePattern as fp 

filepath = "data/Small_Fluorescent_Test_Dataset/image-tiles"

pattern = "img_r00{r:d}_c00{c:d}.tif"

files = fp.FilePattern(filepath, pattern)

#print(files.get())


for file in files.get():
    print(file.first)


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