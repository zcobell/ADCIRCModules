import os
from . import _pyadcircmodules
os.environ['PROJ_LIB']=os.path.dirname(_pyadcircmodules.__file__)
