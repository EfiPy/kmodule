#!/bin/env python3

# lsmod2.py: python sample code for listing Linux kernel modules 
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

mname = 'hello_5'
modules = km.lsmod ()

try:
  hello = modules[mname]
  print(hello)
  print(f'name   : {hello.name}')
  print(f'offset : {hello.offset}')
  print(f'opened : {hello.opened}')
  print(f'size   : {hello.size}')
  print(f'status : {hello.status}')
  print(f'usedby : {hello.usedby}')
  exit(0)
except KeyError as e:
  print (f'{mname} is not installed.')
  exit(-2)
except Exception as e:
  print (f'unknown exception')
  print (e.args)
  exit(-1)

