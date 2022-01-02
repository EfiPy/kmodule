#!/bin/env python3

# modinfo1.py: python sample code for listing Linux kernel modules information
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

mname = 'lkmpg/examples/hello-5.ko'

minfo = km.modinfo(mname)
m = minfo[0]
print(f'name          {m["name"]}')
print(f'filename      {m["filename"]}')
print(f'license       {m["license"]}')
print(f'srcversion    {m["srcversion"]}')
print(f'retpoline     {m["retpoline"]}')
print(f'vermagic      {m["vermagic"]}')
print(f'depends       {m["depends"]}')
