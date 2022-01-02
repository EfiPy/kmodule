#!/bin/env python3

# insmod2.py: python sample code for installling Linux kernel modules.
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

mname = ('lkmpg/examples/hello-5.ko', 'lkmpg/examples/hello-1.ko')

for i in mname:
  print (f'Install module {i}...')
  km.insmod(i)