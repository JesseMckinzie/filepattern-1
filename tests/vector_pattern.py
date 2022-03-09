import filepattern2 as fp
import pprint

path = 'tests/test_data/stitching-vector-61b3a82d6fa1f65bf8cc2448-img-global-positions-1.txt'

pattern = 'x{x:dd}_y{y:dd}_wx{wx:d}_wy{wy:d}_c1.ome.tif'

files = fp.FilePattern(path, pattern, "1 GB")

for file in files():
    pprint.pprint(file)


