# kmodule
Python wrapper for Linux insmod, rmmod, lsmod and modinfo.  
It test in X86_64 and Rapberry pi 4.  
# Homepage
[kmodule](https://github.com/EfiPy/kmodule)
# Required packages
Ubuntu 20.04 as example...
```
apt update
apt-get install build-essential libtool pkgconf libzstd-dev liblzma-dev libssl-dev autoconf
apt-get install python3 python3-pip
```
# Pull kmodule from pypi
`pip3 install kmodule [--install-option=[--with-zstd | --with-xz | --with-zlib | --with-openssl]]`
# Build kmodule
Pull kmod source code with submodule.
```
git clone git@github.com:EfiPy/kmodule.git
cd kmodule
git submodule update --init --recursive
cd -
```
## Build and Install kmodule local
- `python3 setup.py install [--with-zstd --with-xz --with-zlib --with-openssl]`
## Build kmodule
- `python3 setup.py build [--with-zstd --with-xz --with-zlib --with-openssl]`
#  Exmaples
- Download [Linux kernel sample module hello-5 from](https://github.com/EfiPy/kmodule/tree/master/hello-5)
- Confirm Makefile, hello-5.c and sample.py exist.
- Install build kernel module required toolchain, header files and libraries.
- Goto hello-5 folder, run
```
make
python3 sample.py
```
# Example (python invoked as root)
    >>> import kmodule as km
    >>> mlist = km.lsmod ()
    >>> for m, v in mlist.items ():
    ...    print ('======================')
    ...    print ('name: %s' % v.name)
    ...    print ('    v.size: 0x%X' % v.size)
    ...    print ('  v.opened: %d'   % v.opened)
    ...    print ('  v.status: %s'   % v.status)
    ...    print ('  v.offset: 0x%X' % v.offset)
    ...    print ('  v.usedby:', v.usedby)
    ======================
    name: psmouse
        v.size: 0x25000
      v.opened: 0
      v.status: Live
      v.offset: 0xFFFFFFFFC03E2000
      ......
    ======================
    name: cryptd
        v.size: 0x6000
      v.opened: 3
      v.status: Live
      v.offset: 0xFFFFFFFFC04A9000
    >>>
    >>> km.insmod ("hello-5.ko")
    >>> km.rmmod("hello_5")
    >>> minfo = km.modinfo ("hello-5.ko", "e1000")
    >>> print (minfo)
      ......
# Usage
    version

        DESCRIPTION
                kmodule version information including kmod version and features.

        DATA STRUCTURE

            class _version

                KMODULE_VER
                    kmodule release version.
                KMOD_PACKAGE
                    kmodule referenced package.
                KMOD_VERSION
                    KMOD_PACKAGE version
                KMOD_FEATURES
                    KMOD_PACKAGE feature... xz, zlib, zstd, Openssl.
                BuildTime
                    installation date and time.

    lsmod()
        NAME
               kmodule.lsmod() - Show the status of modules in the Linux Kernel

        DESCRIPTION
               kmodule.lsmod() is a trivial program which nicely formats the contents
               of the /proc/modules, showing what kernel modules are currently loaded.

        RETURN
          Dict with module name as key, value is class _lsmod if success. Exception if fail.

        DATA STRUCT

          class _lsmod(object)

            Data defined here:
              name
                module name
              size
                module size
              opened
                module is opend
              status
                module status
              offset
                offset in memory
              usedby
                mouses using this module.

    modinfo(*modules, basedir='', kernel=None)
        NAME
               kmodule.modinfo - Show information about a Linux Kernel module

        DESCRIPTION
               kmodule.modinfo extracts information from the Linux Kernel modules given
               in python.

               If the module name is not a filename, then the /lib/modules/version
               directory is searched.

               kmodule.modinfo by default returns every attribute of the module in dict.
               The filename is listed the same way (although it's not really an attribute).

        OPTIONS
               basedir
                   Root directory for modules, / by default.

               kernel
                   Provide information about a kernel other than the running one.

                   This is particularly useful for distributions needing to extract
                   information from a newly installed (but not yet running) set of
                   kernel modules.

                   For example, you wish to find which firmware files are needed by
                   various modules in a new kernel for which you must make an
                   initrd/initramfs image prior to booting.

        RETURN
          Dict in tuple if success. Exception if fail.

        RETURN DATA

          (dict1, ... dictN)

    insmod(module, **params)
        NAME
          kmodule.insmod() - Simple program to insert a module into the Linux Kernel
    
        DESCRIPTION
          kmodule.insmod is a trivial program to insert a module into the kernel.
    
          Only the most general of error messages are reported: as the work of
          trying to link the module is now done inside the kernel, the dmesg
          usually gives more information about errors.
    
        RETURN
          None if success. Exception if fail.

    rmmod(*modules, force=False, syslog=False, wait=False, verbose=0)
        NAME
               kmodule.rmmod() - Simple program to remove a module from the Linux Kernel

        DESCRIPTION
               kmodule.rmmod() is a trivial program to remove a module (when module
               unloading support is provided) from the kernel.

        OPTIONS
               force
                   This option can be extremely dangerous: it has no effect unless
                   CONFIG_MODULE_FORCE_UNLOAD was set when the kernel was compiled.

                   With this option, you can remove modules which are being used, or
                   which are not designed to be removed, or have been marked as
                   unsafe (see kmodule.lsmod()).

               syslog
                   Send errors to syslog instead of standard error.

               verbose
                   Print messages about what the program is doing. Usually rmmod prints
                   messages only if something goes wrong.

        RETURN
          None if success. Exception if fail.
# History
### 0.6.0:
- invoke Linux official kmod source code as static link in kmodule
### 0.6.1
- Add install options: --with-zstd, --with-xz, --with-zlib, --with-openssl
```
    python3 setup.py build --help
      --with-zstd                          handle Zstandard-compressed modules
                                           [default=disabled]
      --with-xz                            handle Xz-compressed modules
                                           [default=disabled]
      --with-zlib                          handle gzipped modules
                                           [default=disabled]
      --with-openssl                       handle PKCS7 signatures
                                           [default=disabled]
```
- Fix rmmod verbose and syslog parameter malfunction
- Split insmod, rmmod, modinfo function from module.c
- Add version information.
# Arthur
Max Wu <EfiPy.Core@gmail.com>  
# License
GPLv2  
