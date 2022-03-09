import os
import re
import sys
import sysconfig
import platform
import subprocess

from distutils.version import LooseVersion
from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext

        
setup(
    name='filepattern2',
    version='0.1.1',
    author='Jesse McKinzie',
    description='Hybrid Python/C++ filepattern with external memory algorithms',
    long_description='',
    # tell setuptools to look for any packages under 'src'
    packages=find_packages('src'),
    # tell setuptools that all packages will be under the 'src' directory
    # and nowhere else
    package_dir={'':'src'},
    # add an extension module named 'python_cpp_example' to the package 
    # 'python_cpp_example'
    #ext_modules=[ Extension(name='backend',  # using dots!
    #              sources=['backend.pyd'])],
    # add custom build_ext command
    #cmdclass=dict(build_ext=CMakeBuild),
    include_package_data=True,
    zip_safe=False,
)

