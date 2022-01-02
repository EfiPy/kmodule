#!/bin/env python3

# lsmod3.py: python sample code for listing Linux kernel modules 
#  Copyright (C) 2022  MaxWu <EfiPy.core@gmail.com>.
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

import kmodule as km

def _lsmod(args):

  modules = km.lsmod ()

  if len(args) == 0:
    args = modules

  for m in args:
    try:
      print(modules[m])
    except KeyError as e:
      print (f'WARNING: Module "{m}" is not installed.')
    except Exception as e:
      print (f'unknown exception')
      print (e.args)

def _getargs():
  import argparse
  parser = argparse.ArgumentParser(description='List Linux kernel modules.')
  parser.add_argument('mname', metavar='"name"', nargs='*',
                    help='modules name list')

  args = parser.parse_args()
  return args.mname

if __name__ == '__main__':
  mname = _getargs ()
  _lsmod (mname)
