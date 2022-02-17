import filepattern
from filepattern2 import ExternalFilePattern as fp 
import time

path = 'FilePattern/benchmarks/test_data/data1000000'
old_pattern = 'img_r{rrr}_c{ccc}.tif'

pattern = 'img_r{r:ddd}_c{c:ddd}.tif'

start = time.time()
files = fp.FilePattern(path, pattern, "20 GB")
end = time.time()

print(end-start)
