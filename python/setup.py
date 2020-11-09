from setuptools import setup, dist
from setuptools.command.install import install
import os

class BinaryDistribution(dist.Distribution):
    def has_ext_modules(foo):
        return True

this_directory = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(this_directory, '../README.md')) as f:
    long_description = f.read()

setup(
    name='adcircmodules',
    package_data={'adcircmodules': ['_adcircmodules.so']},
    version='0.0.3',
    description="A package for manipulating adcirc model data in python",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url='http://github.com/zcobell/adcircmodules',
    install_requires=[ "gdal", "netCDF4" ],
    author='Zach Cobell',
    author_email='[redacted]',
    include_package_data=True,
    distclass=BinaryDistribution,
    packages=['adcircmodules'],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: C++',
    ],
    keywords='adcirc coastal-modeling',
    project_urls={
        'Github': 'https://github.com/zcobell/adcircmodules',
        'Bug Reports': 'https://github.com/zcobell/adcircmodules/issues',
        'Author': 'https://zachcobell.com/'
    },
)
