# sample.py: sample code of python wrapper for python wrapper for kmod/tools
#            insmod, rmmod, lsmod, modinfo
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

import kmodule as km

def TestLsmod ():
  modules = km.lsmod ()
  for mk, mv in modules.items ():
    print (mv)

def modinfo (*module):

  info = km.modinfo (*module)
  for i in info:
    print ("===================================================")
    for k, v in i.items():
      if k == 'parm':
        for pk, pv in v.items():
          print ('parm   %s: %s' % (pk, pv))
      elif k == 'alias':
        for pv in v:
          print ('alias  %s' % pv)
      else:
        print ('%s: %s' % (k, v))

if __name__ == '__main__':

  try:
    modinfo ('hello-5.ko', 'e1000')
    print ("=========================================================================")
    TestLsmod ()
    print ("=========================================================================")
  except Exception as e:
    print ('Exception1...', e)

  try:
    km.insmod ('hello-5.ko', myshort=3, myint=100, mylong=0x100, mystring="String test", myintArray=(-1,0x100))
    mod = km.lsmod ()
    print (mod['hello_5'])
    km.rmmod ('hello_5')
  except Exception as e:
    print ('Exception2...', e)
