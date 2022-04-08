from setuptools import setup, dist
from setuptools.command.install import install
import os

# force setuptools to recognize that this is
# actually a binary distribution
class BinaryDistribution(dist.Distribution):
    def has_ext_modules(foo):
        return True

# optional, use README.md as long_description
this_directory = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(this_directory, '../README.md')) as f:
    long_description = f.read()

setup(
    # this package is called mymodule
    name='pyadcircmodules',

    # this package contains one module,
    # which resides in the subdirectory mymodule
    packages=['pyadcircmodules'],

    # make sure the shared library is included
    package_data={'pyadcircmodules': ['_pyadcircmodules.so', 'proj.db', "interpolateAdcircStations"]},
    include_package_data=True,

    description="A package for manipulating ADCIRC model data inputs and outputs",
    
    # optional, the contents of README.md that were read earlier
    long_description=long_description,
    long_description_content_type="text/markdown",

    # See class BinaryDistribution that was defined earlier
    distclass=BinaryDistribution,

    version='0.5.1',
    url='http://github.com/zcobell/ADCIRCModules',
    author='Zachary Cobell',
    author_email='zcobell@thewaterinstitute.org'
)
