import filepattern2 as fp 
import filepattern as fpo
import pprint

path = '/home/ec2-user/Dev/data/mini/int'

pattern = 'p{p:d}_y{y:d}_r{r:d}_c{c:d}.ome.tif'


f = fp.FilePattern(path, pattern)

for file in f():
    pprint.pprint(file)
    