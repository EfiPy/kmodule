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
from distutils.command.build_ext import build_ext

TopDir, KmodDir, KmodBuild = bf = KmodHelp.KmodFolderBuild ('kmod')
KmodSharedLib   = os.path.join (KmodBuild, 'shared/.libs')
KmodInternalLib = os.path.join (KmodBuild, 'libkmod/.libs')

class KmodFlag:

    user_options = [
            ('with-zstd',       None, 'handle Zstandard-compressed modules [default=disabled]'),
            ('with-xz',         None, 'handle Xz-compressed modules [default=disabled]'),
            ('with-zlib',       None, 'handle gzipped modules [default=disabled]'),
            ('with-openssl',    None, 'handle PKCS7 signatures [default=disabled]'),
        ]

    InstallFlag  = None

    ConfigString = []
    BuildString  = []

    @classmethod
    def BuildOptions (cls, xz, zstd, zlib, openssl):
        if xz is not None:
            cls.ConfigString += ['--with-xz']
            cls.BuildString  += ['-llzma']
        if zstd is not None:
            cls.ConfigString += ['--with-zstd']
            cls.BuildString  += ['-lzstd']
        if zlib is not None:
            cls.ConfigString += ['--with-zlib']
            cls.BuildString  += ['-lz']
        if openssl is not None:
            cls.ConfigString += ['--with-openssl']
            cls.BuildString  += ['-lcrypto']

class CustomCleanCommand(clean):

    def run(self):
        KmodHelp.KmodClean (KmodBuild)
        super().run()

class CustomInstallCommand(install):

    user_options = install.user_options + KmodFlag.user_options

    def initialize_options(self):
        self.with_zstd      = None
        self.with_xz        = None
        self.with_zlib      = None
        self.with_openssl   = None
        super().initialize_options()

    def finalize_options(self):
        KmodFlag.InstallFlag    = True
        KmodFlag.BuildOptions (self.with_xz, self.with_zstd, self.with_zlib, self.with_openssl)
        super().finalize_options()

    def run(self):

        KmodHelp.KmodConfigureBuild (KmodDir)
        KmodHelp.KmodMakefileBuild (KmodDir, KmodBuild, KmodFlag.ConfigString)
        KmodHelp.KmodBuild (KmodBuild)

        super().run()

class CustomBuildCommand(build):

    user_options = install.user_options + KmodFlag.user_options

    def initialize_options(self):
        self.with_zstd      = None
        self.with_xz        = None
        self.with_zlib      = None
        self.with_openssl   = None
        super().initialize_options()

    def finalize_options(self):
        if KmodFlag.InstallFlag == None:
            KmodFlag.BuildOptions (self.with_xz, self.with_zstd, self.with_zlib, self.with_openssl)
        super().finalize_options()

    def run(self):

        KmodHelp.KmodConfigureBuild (KmodDir)
        KmodHelp.KmodMakefileBuild (KmodDir, KmodBuild, KmodFlag.ConfigString)
        KmodHelp.KmodBuild (KmodBuild)

        super().run()

class CustomBuildExtCommand(build_ext):
    def build_extension(self, ext):
        ext.extra_link_args += KmodFlag.BuildString
        super().build_extension (ext)


with open("README.md", "r") as fh:
  long_description = fh.read()

kmodulec = Extension('_kmodule',
                     ['kmodule.c',
                      'insmod.c',
                      'rmmod.c',
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
        'build_ext': CustomBuildExtCommand,
    },
)
