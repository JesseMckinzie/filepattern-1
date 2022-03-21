echo off

python -m pip install delvewheel
python setup.py bdist_wheel -d dist
pushd dist
dir
for %%j in (*.whl) do (

  delvewheel repair %%j
  pip install wheelhouse\%%j 
  ) 
popd