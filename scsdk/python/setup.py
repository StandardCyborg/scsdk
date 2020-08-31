# Copyright 2020 Standard Cyborg
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import re
import sys
import platform
import plistlib
import subprocess
import sysconfig

from setuptools import setup, Extension, Distribution
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

SC_FUSION_INFO_PLIST_PATH = '../../StandardCyborgFusion/Info.plist'
if(not os.path.isfile(SC_FUSION_INFO_PLIST_PATH)):
  SC_FUSION_INFO_PLIST_PATH = "../ios/StandardCyborgFusion/Info.plist"
sc_fusion_plist = plistlib.load(open(SC_FUSION_INFO_PLIST_PATH, 'rb'))
SCSDK_VERSION = sc_fusion_plist['CFBundleShortVersionString']

## Based upon https://github.com/pybind/cmake_example/blob/11a644072b12ad78352b6e6649db9dfe7f406676/setup.py#L1

with open('requirements.txt') as f:
  INSTALL_REQUIRES = f.read().splitlines()


SETUP_REQUIRES = ['pytest-runner']
TESTS_REQUIRE = ['pytest']

def run_cmd(cmd):
  cmd = cmd.replace('\n', '').strip()
  subprocess.check_call(cmd, shell=True)

SCSDK_NATIVE_CXX_SRC_ROOT = os.environ.get(
  'SCSDK_NATIVE_CXX_SRC_ROOT',
  os.path.join(os.path.abspath('.'), '../c++'))

assert os.path.exists(SCSDK_NATIVE_CXX_SRC_ROOT), \
  "Couldn't find source root at %s" % SCSDK_NATIVE_CXX_SRC_ROOT


with open('scsdk/__init__.py') as f:
  import re
  v = re.search(r"^__version__ = ['\"]([^'\"]*)['\"]", f.read(), re.M).groups()[0]
  assert v == SCSDK_VERSION, \
    ("Please make scsdk/__init__.py __version__ match SCSDK_VERSION "
     "%s != %s" % (v, SCSDK_VERSION))

class BinaryDistribution(Distribution):
  def has_ext_modules(foo):
    return True

class CMakeExtension(Extension):
  def __init__(self, name, sourcedir=''):
    Extension.__init__(self, name, sources=[])
    self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
  def run(self):
    try:
      run_cmd("cmake --version")
    except OSError:
      raise RuntimeError("CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

    for ext in self.extensions:
      self.build_extension(ext)

  def build_extension(self, ext):
    self._build_extension_cmake(ext)

  def _build_extension_cmake(self, ext):
    extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
    # required for auto-detection of auxiliary "native" libs
    if not extdir.endswith(os.path.sep):
      extdir += os.path.sep

    cmake_args = [
      '-H' + SCSDK_NATIVE_CXX_SRC_ROOT,
      '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
      '-DPYTHON_EXECUTABLE=' + sys.executable,
      '-DPYBIND11_ONLY=1',
    ]

    cfg = 'Debug' if self.debug else 'Release'
    build_args = ['--config', cfg]

    if platform.system() == "Windows":
      cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
      if sys.maxsize > 2**32:
        cmake_args += ['-A', 'x64']
      build_args += ['--', '/m']
    else:
      cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
      build_args += ['--', '-j%s' % os.cpu_count(), 'scsdk_native']

    env = os.environ.copy()
    env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                self.distribution.get_version())
    if not os.path.exists(self.build_temp):
      os.makedirs(self.build_temp)
    subprocess.check_call(
      ['cmake', ext.sourcedir] + cmake_args,
      cwd=self.build_temp,
      env=env)
    subprocess.check_call([
      'cmake', '--build', '.'] + build_args,
      cwd=self.build_temp)

setup(
  name='scsdk',
  version=SCSDK_VERSION,
  author='Sven',
  author_email='sven@standardcyborg.com',
  description='The Python Standard Cyborg SDK',
  long_description='',
  license='All rights reserved.  Use only with written permission',
  
  packages=['scsdk'],
  ext_modules=[CMakeExtension('scsdk.scsdk_native')],
  cmdclass=dict(build_ext=CMakeBuild),
  zip_safe=False,
  distclass=BinaryDistribution,

  install_requires=INSTALL_REQUIRES,
  test_suite='scsdk_test',
  setup_requires=SETUP_REQUIRES,
  tests_require=TESTS_REQUIRE,
)


