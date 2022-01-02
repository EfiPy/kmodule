#!/bin/env python3

# modinfo.py: python sample code for listing Linux kernel modules information
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

def _modinf(args):
  minfo = km.modinfo(*args)
  for m in minfo:
    for k, v in m.items():
      if k == 'alias':
        for i in v:
          print ("%15s: %s" % (k, i))
      elif k == 'parm':
        for pk, pv in v.items():
          print ("%15s: %-15s: %s" % (k, pk, pv))
      else:
        print ("%15s: %s" % (k, v))

def _getargs():
  import argparse
  parser = argparse.ArgumentParser(description='List Linux kernel modules.')
  parser.add_argument('mname', metavar='"name"', nargs='+',
                    help='modules name list')

  args = parser.parse_args()
  return args.mname

if __name__ == '__main__':
  mname = _getargs ()
  _modinf (mname)
