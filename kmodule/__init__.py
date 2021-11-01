# kmodule: python wrapper for kmod/tools
#          insmod, rmmod, lsmod, modinfo
#  Copyright (C) 2020  MaxWu <EfiPy.core@gmail.com>.
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

from _kmodule import _rmmod, _logging, _modinfo, _insmod

class _lsmod:

  def __init__ (self, line):

    mName, mSize, mUsedOpen, mUsedBy, mStatus, mOffset = line.split ()[:6]
    self.name   = mName
    self.size   = int(mSize)
    self.opened = int(mUsedOpen)
    self.status = mStatus
    self.offset = int(mOffset, 16)

    if mUsedBy == '-':
      self.usedby = None
    else:
      self.usedby = mUsedBy.split (',')[:-1]

  def __str__ (self):
    return "%-24s %8d, %3d, %10s, 0x%016X, %s" % (
             self.name,
             self.size,
             self.opened,
             self.status,
             self.offset,
             self.usedby)

  __repr__ = __str__

def lsmod ():
  '''
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
'''

  ret = {}

  with open("/proc/modules", "r") as mf:
    lines = mf.readlines()

  for line in lines:
    _a = _lsmod(line)
    ret[_a.name] = _a
    del (_a)

  return ret

def BuildParam (key, value):

  if type(value) is int:
    return '%s=%d' % (key, value)
  elif type(value) is str:
    return '%s="%s"' % (key, value)
  elif type(value) in (list, tuple):
    vf = 1
    vs = ""
    for v in value:

      if type (v) is str:

        if vf == 1:
          vs += '"%s"' % v
          vf = 0
        else:
          vs += ',"%s"' % v

      elif type (v) is int:

        if vf == 1:
          vs += '%d' % v
          vf = 0
        else:
          vs += ',%d' % v

      else:
        raise TypeError ('Invalid parameter type')

    return '%s=%s' % (key, vs)

def insmod (module, **params):
  '''
NAME
  kmodule.insmod() - Simple program to insert a module into the Linux Kernel

DESCRIPTION
  kmodule.insmod is a trivial program to insert a module into the kernel.

  Only the most general of error messages are reported: as the work of
  trying to link the module is now done inside the kernel, the dmesg
  usually gives more information about errors.

RETURN
  None if success. Exception if fail.

'''

  pString = ""

  for key, value in params.items():

    pString += "%s " % BuildParam (key, value)
    vf = 0;

  _insmod (module, pString)

def modinfo (*modules, basedir = '', kernel = None):
  '''
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

'''
  ret = []

  for _m in modules:
    ret.extend (_modinfo (_m, basedir, kernel))

  return tuple(ret)

def rmmod (*modules, force=False, syslog=False, wait=False, verbose=0):
  '''
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
'''
  if syslog == True:
    _logging (True)

  if verbose < 0:
    verbose = 0

  _rmmod (modules, force, wait, verbose)

  if syslog == True:
    _logging (False)

__all__ = ["insmod", "rmmod", "lsmod", "modinfo"]
