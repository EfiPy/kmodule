# setup.py: install utility of python wrapper for module-init-tools
#          insmod, rmmod, lsmod, modinfo
#  Copyright (C) 2020 - 2021  MaxWu <EfiPy.core@gmail.com>.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#

import os

import setuphelp as KmodHelp
from setuptools import Extension
import setuptools

from setuptools.command.install import install
from distutils.command.build import build
from distutils.command.clean import clean

TopDir, KmodDir, KmodBuild = bf = KmodHelp.KmodFolderBuild ('kmod')
KmodSharedLib   = os.path.join (KmodBuild, 'shared/.libs')
KmodInternalLib = os.path.join (KmodBuild, 'libkmod/.libs')

class CustomCleanCommand(clean):

    def run(self):
        print (f"kmod folders are {bf}")
        KmodHelp.KmodClean (KmodBuild)
        super().run()

class CustomInstallCommand(install):

    def run(self):

        print (f"kmod folders are {bf}")
        KmodHelp.KmodConfigureBuild (KmodDir)
        KmodHelp.KmodMakefileBuild (KmodDir, KmodBuild)
        KmodHelp.KmodBuild (KmodBuild)

        super().run()

class CustomBuildCommand(build):

    def run(self):

        print (f"kmod folders are {bf}")
        KmodHelp.KmodConfigureBuild (KmodDir)
        KmodHelp.KmodMakefileBuild (KmodDir, KmodBuild)
        KmodHelp.KmodBuild (KmodBuild)

        super().run()

with open("README.md", "r") as fh:
  long_description = fh.read()

kmodulec = Extension('_kmodule',
                     ['kmodule.c',
                      'log.c',
                     ],
                     define_macros       =[("KMODULEPY", None)],
                     extra_compile_args  =['-Wl,--strip-all', '-g0', f"-I{KmodDir}", f'-I{KmodBuild}'],
                     extra_link_args     =['-Wl,--strip-all', f'-L{KmodSharedLib}', f'-L{KmodInternalLib}',],
                     libraries           =['kmod-internal', 'shared'],
                    )

kmodulep = ['kmodule.__init__']

setuptools.setup(
    name        = 'kmodule',
    py_modules  =  kmodulep,
    ext_modules = [kmodulec],
    version     = '0.6.0',
    author      = "MaxWu",
    author_email= "EfiPy.Core@gmail.com",
    description = "Python wrapper for insmod, rmmod, lsmod, modinfo of libkmod",
    long_description = long_description,
    long_description_content_type = "text/markdown",
    url         = "https://github.com/EfiPy/kmodule",
    packages    = setuptools.find_packages(),
    license     = 'GPLv2',
    platforms   = ['Linux'],
    classifiers = [
         "Programming Language :: Python :: 3",
         "License :: OSI Approved :: GNU General Public License v2 (GPLv2)",
         "Operating System :: POSIX :: Linux",
    ],
    cmdclass={
        'install': CustomInstallCommand,
        'clean': CustomCleanCommand,
        'build': CustomBuildCommand,
    },
)
