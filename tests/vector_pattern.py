import filepattern2 as fp
import pprint

path = '/home/ec2-user/Dev/FilePattern/tests/test_data/stitching-vector-61b3a82d6fa1f65bf8cc2448-img-global-positions-1.txt'

pattern = 'x{x:dd}_y{y:dd}_wx{wx:d}_wy{wy:d}_c1.ome.tif'

files = fp.FilePattern(path, pattern)

matching = files.get_matching(y=[1])

pprint.pprint(matching)
