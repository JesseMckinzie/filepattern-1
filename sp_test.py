from pattern import StringPattern as fp

filepath = "/home/ec2-user/Dev/FilePattern/test.txt"

pattern = "img_r00{r:d}_c00{c:d}.tif"

files = fp.StringPattern(filepath, pattern)

for file in files(): 
    print(file.first)
    print(file.second)
    print()

print("\nGrouped by y: ")
for file in files(group_by='r'): 
    print(file.first)
    print(file.second)
    print()

print("File matching r=1 and c=2:")
for file in files.get_matching("r=1, c=2"):
    print(file.first)