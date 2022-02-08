import unittest, json
from pathlib import Path
from pattern import Pattern

class InferenceTest(unittest.TestCase):
    """Verify VERSION is correct """

    json_path = Path(__file__).parent.parent.joinpath("plugin.json")
    
    def setUp(self):
        
        with open(Path(__file__).with_name('inference_test.json'),'r') as fr:
            
            self.data = json.load(fr)
            
    def test_numeric_fixed_width(self):
        
        pattern = Pattern.Pattern.infer_pattern(files=self.data['robot'])

        self.assertEqual(pattern,'00{r:d}0{t:dd}-{c:d}-00100100{z:d}.tif')

    def test_alphanumeric_fixed_width(self):
        
        pattern = Pattern.Pattern.infer_pattern(files=self.data['brain'])

        self.assertEqual(pattern,'S1_R{r:d}_C1-C11_A1_y0{t:dd}_x0{c:dd}_c0{z:dd}.ome.tif')
        
    def test_alphanumeric_variable_width(self):
        
        pattern = Pattern.Pattern.infer_pattern(files=self.data['variable'])

        self.assertEqual(pattern,'S1_R{r:d}_C1-C11_A1_y{t:d+}_x{c:d+}_c{z:d+}.ome.tif')

    def test_alphanumeric_channel_variable_width(self):
        
        pattern = Pattern.Pattern.infer_pattern(files=self.data['channel'])

        self.assertEqual(pattern,'img_r00{r:d}_c00{t:d}_{c:c+}.tif')
    
    def test_variable_naming(self):
        pattern = Pattern.Pattern.infer_pattern(files=self.data['channel'], variables='xyz')
        
        self.assertEqual(pattern,'img_r00{x:d}_c00{y:d}_{z:c+}.tif')

    def test_alphanumeric_both_variable_width(self):

        pattern = Pattern.Pattern.infer_pattern(files=self.data['both'])

        self.assertEqual(pattern, '{r:cccccc}_{t:d+}_{c:c+}.ome.tif')
    
    def test_invalid_input(self):
        with self.assertRaises(RuntimeError):
            pattern = Pattern.Pattern.infer_pattern(files=self.data['invalid'])

class ExternalInferenceTest(unittest.TestCase):

    path = 'FilePattern/tests/test_data/data'
    def test_fp_single_block(self):
        pattern = Pattern.Pattern.infer_pattern(path=self.path, block_size='1 GB')
        
        self.assertEqual(pattern, 'img_r00{r:d}_c00{t:d}_{c:c+}.tif')
        
    def test_fp_multiple_block(self):
        pattern = Pattern.Pattern.infer_pattern(path=self.path, block_size='1 MB')
        
        self.assertEqual(pattern, 'img_r00{r:d}_c00{t:d}_{c:c+}.tif')
        
    def test_sp_single_block(self):
        path = '/home/ec2-user/Dev/FilePattern/examples/test_data/sp_data.txt'
        pattern = Pattern.Pattern.infer_pattern(path=path, block_size='1 GB')
        
        self.assertEqual(pattern, 'img_r00{r:d}_c00{t:d}_{c:c+}.tif')
        
    def test_sp_multi_block(self):
        path = '/home/ec2-user/Dev/FilePattern/examples/test_data/sp_data.txt'
        pattern = Pattern.Pattern.infer_pattern(path=path, block_size='900 B')
        
        self.assertEqual(pattern, 'img_r00{r:d}_c00{t:d}_{c:c+}.tif')
    
    def test_vp_single_block(self):
        path = '/home/ec2-user/Dev/FilePattern/stitching-vector-61b3a82d6fa1f65bf8cc2448-img-global-positions-1.txt'
        pattern = Pattern.Pattern.infer_pattern(path=path, block_size='1 GB')
        
        self.assertEqual(pattern, 'x{r:dd}_y{t:dd}_wx{c:d}_wy{z:d}_c1.ome.tif')
                
    def test_vp_multi_block(self):
        path = '/home/ec2-user/Dev/FilePattern/stitching-vector-61b3a82d6fa1f65bf8cc2448-img-global-positions-1.txt'
        pattern = Pattern.Pattern.infer_pattern(path=path, block_size='250 B')
        
        self.assertEqual(pattern, 'x{r:dd}_y{t:dd}_wx{c:d}_wy{z:d}_c1.ome.tif')
        

if __name__=="__main__":

    unittest.main()