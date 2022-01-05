import unittest
from pattern import FilePattern as fp
from pattern import StringPattern as sp
from pattern import ExternalFilePattern as efp
import filepattern
import os
import pprint

class TestFilePattern(unittest.TestCase):

    path = '/home/ec2-user/Dev/FilePattern/benchmarks/test_data/data100'

    old_pattern = 'img_r{rrr}_c{ccc}.tif'

    patterns = ['img_r{r:ddd}_c{c:ddd}.tif', 'img_r{r:d+}_c{c:d+}.tif']

    MAX_NUM = 1

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

            self.assertEqual(len(old_result), len(result)) 

            for i in range(len(old_result)):
                self.assertEqual(old_result[i][0]["r"], result[i][0]["r"]) 
                self.assertEqual(old_result[i][0]["c"], result[i][0]["c"])
                self.assertEqual(str(old_result[i][0]['file']), result[i][1][0])


    def test_get_matching(self):
         for pattern in self.patterns:
            nums = []
            for i in range(0, self.MAX_NUM):

                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.path, pattern)

                nums.append(i)

                old_result = old_files.get_matching(R=[i])
                result = files.get_matching(r=[i])

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)):                
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])

                old_result = old_files.get_matching(C=[i])
                result = files.get_matching(c=[i])

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)):                
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])

                old_result = old_files.get_matching(R=nums)
                result = files.get_matching(r=nums)

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)): 
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])

                old_result = old_files.get_matching(C=nums)
                result = files.get_matching(c=nums)

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)): 
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])


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

            self.assertEqual(len(old_result), len(result)) 

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    self.assertEqual(old_result[i][j]["r"], result[i][j][0]["r"]) 
                    self.assertEqual(old_result[i][j]["c"], result[i][j][0]["c"])
                    self.assertEqual(str(old_result[i][j]['file']), result[i][j][1][0])

            for file in old_files(group_by="r"):
                old_result.append(file)
            for file in files(group_by="c"):
                result.append(file)

            self.assertEqual(len(old_result), len(result)) 

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    self.assertEqual(old_result[i][j]["r"], result[i][j][0]["r"]) 
                    self.assertEqual(old_result[i][j]["c"], result[i][j][0]["c"])
                    self.assertEqual(str(old_result[i][j]['file']), result[i][j][1][0])

    def test_recursive_filepattern(self):
        path = '/home/ec2-user/Dev/FilePattern/tests/test_data/recursive_data'
        old_path = '/home/ec2-user/Dev/FilePattern/tests/test_data/recursive_data/DAPI'

        for pattern in self.patterns:
            old_files = filepattern.FilePattern(old_path, self.old_pattern)
            files = fp.FilePattern(path, pattern, True)

            old_result = []
            result = []

            # group by "c" instead of "r" since we changed how group by works
            for file in old_files():
                old_result.append(file)
            for file in files():
                result.append(file)

            # test that same number of files are returned
            self.assertEqual(len(old_result), len(result)) 

            # test that each variable and path are equal for each file in list
            for i in range(len(old_result)): 
                self.assertEqual(old_result[i][0]["r"], result[i][0]["r"]) 
                self.assertEqual(old_result[i][0]["c"], result[i][0]["c"])
                self.assertEqual(str(old_result[i][0]['file']), result[i][1][0])

            basename = ''
            # test that all basenames in vector of paths are the same
            for mapping in files:
                basename = os.path.basename(mapping[1][0])
                for filepath in mapping[1]:
                    self.assertEqual(basename, os.path.basename(filepath))
    

class TestStringPattern(unittest.TestCase):

    path = '/home/ec2-user/Dev/FilePattern/benchmarks/test_data/data100'

    filepath = '/home/ec2-user/Dev/FilePattern/benchmarks/test_data/data100.txt'

    old_pattern = 'img_r{rrr}_c{ccc}.tif'

    patterns = ['img_r{r:ddd}_c{c:ddd}.tif', 'img_r{r:d+}_c{c:d+}.tif']

    MAX_NUM = 1

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

            self.assertEqual(len(old_result), len(result)) 

            for i in range(len(old_result)):
                self.assertEqual(old_result[i][0]["r"], result[i][0]["r"]) 
                self.assertEqual(old_result[i][0]["c"], result[i][0]["c"])
                self.assertEqual(str(old_result[i][0]['file']), result[i][1][0])


    def test_get_matching(self):
         for pattern in self.patterns:
            nums = []
            for i in range(0, self.MAX_NUM):

                old_files = filepattern.FilePattern(self.path, self.old_pattern)
                files = fp.FilePattern(self.path, pattern)

                nums.append(i)

                old_result = old_files.get_matching(R=[i])
                result = files.get_matching(r=[i])

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)):                
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])

                old_result = old_files.get_matching(C=[i])
                result = files.get_matching(c=[i])

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)):                
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])

                old_result = old_files.get_matching(R=nums)
                result = files.get_matching(r=nums)

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)): 
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])

                old_result = old_files.get_matching(C=nums)
                result = files.get_matching(c=nums)

                self.assertEqual(len(old_result), len(result)) 

                for i in range(len(old_result)): 
                    self.assertEqual(old_result[i]["r"], result[i][0]["r"]) 
                    self.assertEqual(old_result[i]["c"], result[i][0]["c"])
                    self.assertEqual(str(old_result[i]['file']), result[i][1][0])


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

            self.assertEqual(len(old_result), len(result)) 

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    self.assertEqual(old_result[i][j]["r"], result[i][j][0]["r"]) 
                    self.assertEqual(old_result[i][j]["c"], result[i][j][0]["c"])
                    self.assertEqual(str(old_result[i][j]['file']), result[i][j][1][0])

            for file in old_files(group_by="r"):
                old_result.append(file)
            for file in files(group_by="c"):
                result.append(file)

            self.assertEqual(len(old_result), len(result)) 

            for i in range(len(old_result)):
                for j in range(len(old_result[i])):
                    self.assertEqual(old_result[i][j]["r"], result[i][j][0]["r"]) 
                    self.assertEqual(old_result[i][j]["c"], result[i][j][0]["c"])
                    self.assertEqual(str(old_result[i][j]['file']), result[i][j][1][0])

    def test_recursive_filepattern(self):
        path = '/home/ec2-user/Dev/FilePattern/tests/test_data/recursive_data'
        old_path = '/home/ec2-user/Dev/FilePattern/tests/test_data/recursive_data/DAPI'

        for pattern in self.patterns:
            old_files = filepattern.FilePattern(old_path, self.old_pattern)
            files = fp.FilePattern(path, pattern, True)

            old_result = []
            result = []

            # group by "c" instead of "r" since we changed how group by works
            for file in old_files():
                old_result.append(file)
            for file in files():
                result.append(file)

            # test that same number of files are returned
            self.assertEqual(len(old_result), len(result)) 

            # test that each variable and path are equal for each file in list
            for i in range(len(old_result)): 
                self.assertEqual(old_result[i][0]["r"], result[i][0]["r"]) 
                self.assertEqual(old_result[i][0]["c"], result[i][0]["c"])
                self.assertEqual(str(old_result[i][0]['file']), result[i][1][0])

            basename = ''
            # test that all basenames in vector of paths are the same
            for mapping in files:
                basename = os.path.basename(mapping[1][0])
                for filepath in mapping[1]:
                    self.assertEqual(basename, os.path.basename(filepath))
    



        
if __name__ == '__main__':
    unittest.main()