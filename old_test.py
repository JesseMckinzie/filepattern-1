#from pattern import ExternalFilePattern as efp
from pattern import FilePattern as fp
#import filepattern as fp
import time

#path = '/home/ec2-user/Dev/PolusData/polus-data/polus/images/TissueNet/standard/train/intensity'
#pattern = 'p{p:d}_y{y:d}_r{r:ddd}_c{c:d}.ome.tif'

#file = efp.FilePattern(path, pattern)c

#file.print_valid_files()


path = "/home/ec2-user/Dev/data/Small_Fluorescent_Test_Dataset/"
pattern = "img_r00{r:d}_c00{c:d}.tif"

files = fp.FilePattern(path, pattern, recursive=True)

for file in files(): 
    print(file.first)
    print(file.second)
    print()

for file in files.get_matching("r=1, c=2"):
    print(file.first)
