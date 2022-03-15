import pytest
import filepattern2 as fp
import filepattern
import os
from operator import itemgetter
import pprint
import test_generate_filepattern_data

class TestFilePattern():

    test_generate_filepattern_data.main()

    root_directory = os.path.dirname(os.path.realpath(__file__))

    path = root_directory + '/test_data/data100'

    old_pattern = 'img_r{rrr}_c{ccc}.tif'

    patterns = ['img_r{r:ddd}_c{c:ddd}.tif', 'img_r{r:d+}_c{c:d+}.tif', old_pattern]

    MAX_NUM = 9

    def test_file_pattern(self):

        for pattern in self.patterns:
            old_files = filepattern.FilePattern(self.path, self.old_pattern)
            files = fp.FilePattern(self.path, pattern)

            old_result = []
            result = []

            for file in old_files():
                old_result.append(file)
            for file in files():
                result.append(file)

            assert len(old_result) == len(result)
            
            for i in range(len(old_result)):
                assert old_result[i][0]["r"] == result[i][0]["r"]
                assert old_result[i][0]["c"] == result[i][0]["c"]
                assert str(os.path.basename(old_result[i][0]['file'])) == os.path.basename(result[i][1][0])


    def test_get_matching(self):
         for pattern in self.patterns:
            nums = []
            for i in range(0, self.MAX_NUM):

                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.path, pattern)

                nums.append(i)

                old_result = old_files.get_matching(R=[i])
                result = files.get_matching(r=[i])

                assert len(old_result) == len(result) 

                for i in range(len(old_result)):                
                    assert old_result[i]["r"] == result[i][0]["r"] 
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(C=[i])
                result = files.get_matching(c=[i])

                assert len(old_result) == len(result) 

                for i in range(len(old_result)):                
                    assert old_result[i]["r"] == result[i][0]["r"] 
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(R=nums)
                result = files.get_matching(r=nums)

                assert len(old_result) == len(result)

                for i in range(len(old_result)): 
                    assert old_result[i]["r"] == result[i][0]["r"]
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(C=nums)
                result = files.get_matching(c=nums)

                assert len(old_result) == len(result) 

                for i in range(len(old_result)): 
                    assert old_result[i]["r"] == result[i][0]["r"] 
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])


    def test_group_by(self):
        for pattern in self.patterns:
            old_files = filepattern.FilePattern(self.path, self.old_pattern)
            files = fp.FilePattern(self.path, pattern)

            old_result = []
            result = []

            # group by "c" instead of "r" since we changed how group by works
            for file in old_files(group_by="c"):
                old_result.append(file)
            for file in files(group_by="r"):
                result.append(file)

            assert len(old_result) == len(result)

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    assert old_result[i][j]["r"] == result[i][1][j][0]["r"] 
                    assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                    assert str(os.path.basename(old_result[i][j]['file'])) == os.path.basename(result[i][1][j][1][0])

            for file in old_files(group_by="r"):
                old_result.append(file)
            for file in files(group_by="c"):
                result.append(file)

            assert len(old_result) == len(result)

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    assert old_result[i][j]["r"] == result[i][1][j][0]["r"]
                    assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                    assert str(os.path.basename(old_result[i][j]['file'])) == os.path.basename(result[i][1][j][1][0])

    def test_recursive_filepattern(self):
        path = self.root_directory + '/test_data/recursive_data'
        old_path = self.root_directory + '/test_data/recursive_data/DAPI'

        for pattern in self.patterns:
            old_files = filepattern.FilePattern(old_path, self.old_pattern)
            files = fp.FilePattern(path, pattern, recursive=True)

            old_result = []
            result = []

            # group by "c" instead of "r" since we changed how group by works
            for file in old_files():
                old_result.append(file)
            for file in files():
                result.append(file)

            # test that same number of files are returned
            assert len(old_result) == len(result) 

            # test that each variable and path are equal for each file in list
            for i in range(len(old_result)): 
                assert old_result[i][0]["r"] == result[i][0]["r"]
                assert old_result[i][0]["c"] == result[i][0]["c"]
                assert str(os.path.basename(old_result[i][0]['file'])) == os.path.basename(result[i][1][0])

            basename = ''
            # test that all basenames in vector of paths are the same
            for mapping in files:
                basename = os.path.basename(mapping[1][0])
                for filepath in mapping[1]:
                    assert basename == os.path.basename(filepath)

class TestStringPattern():

    root_directory = os.path.dirname(os.path.realpath(__file__))

    path = root_directory + '/test_data/data100'

    filepath = root_directory + '/test_data/data100.txt'

    old_pattern = 'img_r{rrr}_c{ccc}.tif'

    patterns = ['img_r{r:ddd}_c{c:ddd}.tif', 'img_r{r:d+}_c{c:d+}.tif', old_pattern]

    MAX_NUM = 9

    def test_file_pattern(self):
        for pattern in self.patterns:
            old_files = filepattern.FilePattern(self.path, self.old_pattern)
            files = fp.FilePattern(self.filepath, pattern)

            old_result = []
            result = []

            for file in old_files():
                old_result.append(file)
            for file in files():
                result.append(file)

            assert len(old_result) == len(result)

            for i in range(len(old_result)):
                assert old_result[i][0]["r"] == result[i][0]["r"]
                assert old_result[i][0]["c"] == result[i][0]["c"]
                assert str(os.path.basename(old_result[i][0]['file'])) == result[i][1][0]


    def test_get_matching(self):
         for pattern in self.patterns:
            nums = []
            for i in range(0, self.MAX_NUM):

                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.filepath, pattern)

                nums.append(i)

                old_result = old_files.get_matching(R=[i])
                result = files.get_matching(r=[i])

                assert len(old_result) == len(result)

                for i in range(len(old_result)):                
                    assert old_result[i]["r"] == result[i][0]["r"]
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]

                old_result = old_files.get_matching(C=[i])
                result = files.get_matching(c=[i])

                assert len(old_result) == len(result)

                for i in range(len(old_result)):                
                    assert old_result[i]["r"] == result[i][0]["r"]
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]

                old_result = old_files.get_matching(R=nums)
                result = files.get_matching(r=nums)

                assert len(old_result) == len(result)

                for i in range(len(old_result)): 
                    assert old_result[i]["r"] == result[i][0]["r"] 
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]

                old_result = old_files.get_matching(C=nums)
                result = files.get_matching(c=nums)

                assert len(old_result) == len(result)

                for i in range(len(old_result)): 
                    assert old_result[i]["r"] == result[i][0]["r"]
                    assert old_result[i]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]


    def test_group_by(self):
        for pattern in self.patterns:
            old_files = filepattern.FilePattern(self.path, self.old_pattern)
            files = fp.FilePattern(self.filepath, pattern)

            old_result = []
            result = []

            # group by "c" instead of "r" since we changed how group by works
            for file in old_files(group_by="c"):
                old_result.append(file)
            for file in files(group_by="r"):
                result.append(file)

            assert len(old_result) == len(result)

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    assert old_result[i][j]["r"] == result[i][1][j][0]["r"]
                    assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                    assert str(os.path.basename(old_result[i][j]['file'])) == result[i][1][j][1][0]

            for file in old_files(group_by="r"):
                old_result.append(file)
            for file in files(group_by="c"):
                result.append(file)

            assert len(old_result) == len(result)

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    assert old_result[i][j]["r"] == result[i][1][j][0]["r"] 
                    assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                    assert str(os.path.basename(old_result[i][j]['file'])) == result[i][1][j][1][0]


class TestVectorPattern():
    
    root_directory = os.path.dirname(os.path.realpath(__file__))

    path = root_directory + '/test_vectorpattern_data.txt'
    
    old_pattern = 'x{xx}_y{yy}_wx{z}_wy{r}_c{c}.ome.tif'
    
    patterns = ['x{x:dd}_y{y:dd}_wx{z:d}_wy{r:d}_c{c:d}.ome.tif', old_pattern]
    
    MAX_NUM = 9

    def test_file_pattern(self):

        for pattern in self.patterns:
            old_files = filepattern.VectorPattern(self.path, self.old_pattern)
            files = fp.FilePattern(self.path, pattern)

            old_result = []
            result = []

            for file in old_files():
                old_result.append(file)
            for file in files():
                result.append(file)

            assert len(old_result) == len(result) 
            
            old_result = sorted(old_result, key=lambda k: k[0]['file'])
            
            for i in range(len(old_result)):
                for key in result[i][0]:
                    assert int(old_result[i][0][key]) == result[i][0][key]
                assert str(os.path.basename(old_result[i][0]['file'])) == os.path.basename(result[i][1][0])

    def test_get_matching(self):
         for pattern in self.patterns:
            nums = []
            for i in range(0, self.MAX_NUM):

                old_files = filepattern.VectorPattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.path, pattern)

                nums.append(i)

                old_result = old_files.get_matching(R=[i])
                result = files.get_matching(r=[i])

                assert len(old_result) == len(result)

                #pprint.pprint(result)
                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results
                
                
                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert str(old_result[i][key]) == str(result[i][0][key]) # Old version stores value as string 
                    assert os.path.basename(old_result[i]['file']) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(C=[i])
                result = files.get_matching(c=[i])

                assert len(old_result) == len(result) 
                
                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results

                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert old_result[i][0][key] == result[i][0][key]
                    assert str(os.path.basename(old_result[i][0]['file'])) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(R=nums)
                result = files.get_matching(r=nums)

                assert len(old_result) == len(result)

                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results

                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert str(old_result[i][key]) == str(result[i][0][key]) # Old version stores value as string 
                    assert os.path.basename(old_result[i]['file']) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(C=nums)
                result = files.get_matching(c=nums)

                assert len(old_result) == len(result)
                
                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results

                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert str(old_result[i][key]) == str(result[i][0][key]) # Old version stores value as string 
                    assert os.path.basename(old_result[i]['file']) == os.path.basename(result[i][1][0])


class TestExternalFilePattern():
    root_directory = os.path.dirname(os.path.realpath(__file__))

    path = root_directory + '/test_data/data100'

    old_pattern = 'img_r{rrr}_c{ccc}.tif'

    patterns = ['img_r{r:ddd}_c{c:ddd}.tif', 'img_r{r:d+}_c{c:d+}.tif']

    block_sizes = ['300 KB']

    MAX_NUM = 1

    def test_file_pattern(self):
        for pattern in self.patterns:
            for block_size in self.block_sizes:
                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.path, pattern, block_size=block_size)
                
                old_result = []
                result = []
                
                for file in old_files():
                    old_result.append(file)

                for file in files():
                    result.append(file)

                assert len(old_result) == len(result)

                for i in range(len(old_result)):
                    assert old_result[i][0]["r"] == result[i][0]["r"] 
                    assert old_result[i][0]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i][0]['file'])) == os.path.basename(result[i][1][0])
                    
                    

    def test_get_matching(self):
        for pattern in self.patterns:
            nums = []
            for block_size in self.block_sizes:
                for i in range(0, self.MAX_NUM):

                    old_files = filepattern.FilePattern(self.path, self.old_pattern)
                    files = fp.FilePattern(self.path, pattern, block_size=block_size)

                    nums.append(i)

                    old_result = old_files.get_matching(R=[i])

                    result = []
                    for match in files.get_matching(r=[i]):
                        result.append(match)
                    
                    assert len(old_result) == len(result)

                    for i in range(len(old_result)):                
                        assert old_result[i]["r"] == result[i][0]["r"]
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])

                    old_result = old_files.get_matching(C=[i])
                    result = []
                    for match in files.get_matching(c=[i]):
                        result.append(match)

                    assert len(old_result) == len(result)

                    for i in range(len(old_result)):                
                        assert old_result[i]["r"] == result[i][0]["r"]
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])

                    old_result = old_files.get_matching(R=nums)
                    #files.get_matching(r=nums)
                    
                    result = []
                    for match in files.get_matching(r=nums):
                        result.append(match)

                    assert len(old_result) == len(result)

                    for i in range(len(old_result)): 
                        assert old_result[i]["r"] == result[i][0]["r"]
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])

                    old_result = old_files.get_matching(C=nums)
                    #files.get_matching(c=nums)
                    
                    result = []
                    for match in files.get_matching(c=nums):
                        result.append(match)

                    assert len(old_result) == len(result)

                    for i in range(len(old_result)): 
                        assert old_result[i]["r"] == result[i][0]["r"]
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == os.path.basename(result[i][1][0])


    def test_group_by(self):
        for pattern in self.patterns:
            for block_size in self.block_sizes:
                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.path, pattern, block_size=block_size)

                old_result = []
                result = []
                
                # group by "c" instead of "r" since we changed how group by works
                for file in old_files(group_by="c"):
                    old_result.append(file)

                for file in files(group_by="r"):
                    result.append(file)
                  
                assert len(old_result) == len(result) 
                for i in range(len(old_result)):
                   assert len(old_result[i]) == len(result[i][1])
                
                for i in range(len(old_result)):
                    for j in range(len(old_result[i])):
                        assert old_result[i][j]["r"] == result[i][1][j][0]["r"] 
                        assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                        assert str(os.path.basename(old_result[i][j]['file'])) == os.path.basename(result[i][1][j][1][0])

                result = []
                old_result = []
                
                for file in old_files(group_by="r"):
                    old_result.append(file)


                for file in files(group_by="c"):
                    result.append(file)         
                

                assert len(old_result) == len(result)

                for i in range(len(old_result)):
                    for j in range(len(old_result[i])):
                        assert old_result[i][j]["r"] == result[i][1][j][0]["r"]
                        assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                        assert str(os.path.basename(old_result[i][j]['file'])) == os.path.basename(result[i][1][j][1][0])


class TestExternalStringPattern():
    root_directory = os.path.dirname(os.path.realpath(__file__))

    path = root_directory + '/test_data/data100'
    string_path = root_directory + '/test_data/data100.txt'

    old_pattern = 'img_r{rrr}_c{ccc}.tif'

    patterns = ['img_r{r:ddd}_c{c:ddd}.tif', 'img_r{r:d+}_c{c:d+}.tif']

    block_sizes = ['300 KB']

    MAX_NUM = 1

    def test_file_pattern(self):
        for pattern in self.patterns:
            for block_size in self.block_sizes:
                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.string_path, pattern, block_size=block_size)

                old_result = []
                result = []

                for file in old_files():
                    old_result.append(file)

                for file in files():
                    result.append(file)

                assert len(old_result) == len(result)

                for i in range(len(old_result)):
                    assert old_result[i][0]["r"] == result[i][0]["r"]
                    assert old_result[i][0]["c"] == result[i][0]["c"]
                    assert str(os.path.basename(old_result[i][0]['file'])) == result[i][1][0]
   
    def test_get_matching(self):
        for pattern in self.patterns:
            nums = []
            for block_size in self.block_sizes:
                for i in range(0, self.MAX_NUM):

                    old_files = filepattern.FilePattern(self.path, self.old_pattern)
                    files = fp.FilePattern(self.string_path, pattern, block_size=block_size)

                    nums.append(i)

                    old_result = old_files.get_matching(R=[i])

                    result = []
                    for match in files.get_matching(r=[i]):
                        result.append(match)
                    
                    assert len(old_result) == len(result)

                    for i in range(len(old_result)):                
                        assert old_result[i]["r"] == result[i][0]["r"]
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]

                    old_result = old_files.get_matching(C=[i])
                    result = []
                    for match in files.get_matching(c=[i]):
                        result.append(match)

                    assert len(old_result) == len(result)

                    for i in range(len(old_result)):                
                        assert old_result[i]["r"] == result[i][0]["r"] 
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]

                    old_result = old_files.get_matching(R=nums)
                    
                    result = []
                    for match in files.get_matching(r=nums):
                        result.append(match)

                    assert len(old_result) == len(result)

                    for i in range(len(old_result)): 
                        assert old_result[i]["r"] == result[i][0]["r"]
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]

                    old_result = old_files.get_matching(C=nums)
                    
                    result = []
                    for match in files.get_matching(c=nums):
                        result.append(match)

                    assert len(old_result) == len(result)

                    for i in range(len(old_result)): 
                        assert old_result[i]["r"] == result[i][0]["r"] 
                        assert old_result[i]["c"] == result[i][0]["c"]
                        assert str(os.path.basename(old_result[i]['file'])) == result[i][1][0]


    def test_group_by(self):
        for pattern in self.patterns:
            for block_size in self.block_sizes:
                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.string_path, pattern, block_size=block_size)

                old_result = []
                result = []

                # group by "c" instead of "r" since we changed how group by works
                for file in old_files(group_by="c"):
                    
                    old_result.append(file)

                for file in files(group_by="r"):
                    result.append(file)


                assert len(old_result) == len(result)
                for i in range(len(old_result)):
                   assert len(old_result[i]) == len(result[i][1])
                
                for i in range(len(old_result)):
                    for j in range(len(old_result[i])):
                        assert old_result[i][j]["r"] == result[i][1][j][0]["r"]
                        assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                        assert str(os.path.basename(old_result[i][j]['file'])) == result[i][1][j][1][0]

                result = []
                old_result = []

                for file in old_files(group_by="r"):
                    old_result.append(file)


                for file in files(group_by="c"):
                    result.append(file)     
                                
                assert len(old_result) == len(result)

                for i in range(len(old_result)):
                    for j in range(len(old_result[i])):
                        assert old_result[i][j]["r"] == result[i][1][j][0]["r"] 
                        assert old_result[i][j]["c"] == result[i][1][j][0]["c"]
                        assert str(os.path.basename(old_result[i][j]['file'])) == result[i][1][j][1][0]


class TestExternalVectorPattern():
    
    root_directory = os.path.dirname(os.path.realpath(__file__))

    path = root_directory + '/test_vectorpattern_data.txt'
    
    old_pattern = 'x{xx}_y{yy}_wx{z}_wy{r}_c{c}.ome.tif'
    
    patterns = ['x{x:dd}_y{y:dd}_wx{z:d}_wy{r:d}_c{c:d}.ome.tif', old_pattern]
    
    MAX_NUM = 9

    def test_file_pattern(self):

        for pattern in self.patterns:
            old_files = filepattern.VectorPattern(self.path, self.old_pattern)
            files = fp.FilePattern(self.path, pattern)

            old_result = []
            result = []

            for file in old_files():
                old_result.append(file)
            for file in files():
                result.append(file)

            assert len(old_result) == len(result)
            
            old_result = sorted(old_result, key=lambda k: k[0]['file'])
            
            for i in range(len(old_result)):
                for key in result[i][0]:
                    assert int(old_result[i][0][key]) == result[i][0][key]
                assert str(os.path.basename(old_result[i][0]['file'])) == os.path.basename(result[i][1][0])

    def test_get_matching(self):
         for pattern in self.patterns:
            nums = []
            for i in range(0, self.MAX_NUM):

                old_files = filepattern.VectorPattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.path, pattern)

                nums.append(i)

                old_result = old_files.get_matching(R=[i])
                result = files.get_matching(r=[i])

                assert len(old_result) == len(result)

                #pprint.pprint(result)
                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results
                
                
                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert str(old_result[i][key]) == str(result[i][0][key]) # Old version stores value as string 
                    assert os.path.basename(old_result[i]['file']) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(C=[i])
                result = files.get_matching(c=[i])

                assert len(old_result) == len(result) 
                
                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results

                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert old_result[i][0][key] == result[i][0][key] 
                    assert str(os.path.basename(old_result[i][0]['file'])) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(R=nums)
                result = files.get_matching(r=nums)

                assert len(old_result) == len(result)

                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results

                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert str(old_result[i][key]) == str(result[i][0][key]) # Old version stores value as string 
                    assert os.path.basename(old_result[i]['file']) == os.path.basename(result[i][1][0])

                old_result = old_files.get_matching(C=nums)
                result = files.get_matching(c=nums)

                assert len(old_result) == len(result) 
                
                old_result = sorted(old_result, key=lambda k: k['file']) # Old version does not sort results

                for i in range(len(old_result)):
                    for key in result[i][0]:
                        assert str(old_result[i][key]) == str(result[i][0][key]) # Old version stores value as string 
                    assert os.path.basename(old_result[i]['file']) == os.path.basename(result[i][1][0])            
                    