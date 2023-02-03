from distutils.core import setup, Extension
import glob

cpp_args = ['-std=c++11']

ext_modules = [
    Extension(
    'PySurfaceNets',
        ['src/SurfaceNets.cpp'] + glob.glob('src/SNLib/*.cpp'),
        include_dirs=['extern/pybind11/include', 'src/'],
    language='c++',
    extra_compile_args = cpp_args,
    ),
]

setup(
    name='PySurfaceNets',
    version='0.0.1',
    author='TurtleIzzy',
    author_email='agnimon@gmail.com',
    description='Example',
    ext_modules=ext_modules,
)