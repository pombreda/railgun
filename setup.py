from setuptools import setup
from railgun import __author__, __version__, __license__

setup(
    name='railgun',
    version=__version__,
    packages=['railgun'],
    description=('ctypes utilities for faster and easier '
                 'simulation programming in C and Python'),
    long_description=open('README.rst').read(),
    author=__author__,
    author_email='aka.tkf@gmail.com',
    url='http://bitbucket.org/tkf/railgun/src',
    keywords='numerical simulation, research, ctypes, numpy, c',
    license=__license__,
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "Operating System :: POSIX :: Linux",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python",

        "Topic :: Scientific/Engineering",
        ],
    )
