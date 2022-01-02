#!/bin/env python3

# insmod3.py: python sample code for installling Linux kernel modules.
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

print('install kernel module with parameter sample.')
km.insmod ('lkmpg/examples/hello-5.ko',
           myshort=3,
           myint=100,
           mylong=0x100,
           mystring="String test",
           myintArray=(-1,0x100))
