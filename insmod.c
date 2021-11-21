/*
 * insmod.c: python wrapper for python wrapper for kmod/tools/insmod.c
 *   Copyright (C) 2021  MaxWu <EfiPy.core@gmail.com>.

 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.

 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#include <config.h>

#include <Python.h>
#define PYSAMPLE_MODULE
#include "structmember.h"

#include <sys/utsname.h>
#include <libkmod/libkmod.h>

#include <shared/util.h>
#include <tools/kmod.h>

///////////////////////////////////////////////////////////////////////
///
/// static function insmod
///
///////////////////////////////////////////////////////////////////////

/***********************************************************************
 *
 * mod_strerror:
 *
 ***********************************************************************/
static const char *
mod_strerror (
  int err
  )
{
  switch (err) {
  case ENOEXEC:
    return "Invalid module format";
  case ENOENT:
    return "Unknown symbol in module";
  case ESRCH:
    return "Module has wrong symbol version";
  case EINVAL:
    return "Invalid parameters";
  default:
    return strerror(err);
  }
} // mod_strerror

///////////////////////////////////////////////////////////////////////
///
/// kmodule public function
///
///////////////////////////////////////////////////////////////////////

/***********************************************************************
 *
 * kmodule_insmod:
 *
 ***********************************************************************/
PyObject *
kmodule_insmod (
  PyObject    *Self,
  PyObject    *Args,
  PyObject    *KwArgs
  )
{
  int    ret;
  char  *ModuleName;
  char  *Parameters;
  static char   *kwlist[] = {"Module name", "parameter", NULL};

  if (!PyArg_ParseTupleAndKeywords (
      Args,
      KwArgs,
      "s|s",
      kwlist,
      &ModuleName,
      &Parameters)) {
    return NULL;
  }

  {

    struct kmod_ctx *ctx;
    struct kmod_module *mod;

    unsigned int flags = 0;

    const char *null_config = NULL;

    ctx = kmod_new(NULL, &null_config);
    if (!ctx) {
      PyErr_Format (PyExc_MemoryError, "Internal resource initial fail.\n");
      return NULL;
    }

    ret = kmod_module_new_from_path(ctx, ModuleName, &mod);
    if (ret < 0) {
      PyErr_Format (PyExc_SystemError, "Could not load module %s: %s\n", ModuleName, strerror(-ret));
      goto end;
    }

    ret = kmod_module_insert_module(mod, flags, Parameters);
    if (ret < 0) {
      PyErr_Format (PyExc_SystemError, "could not insert module %s: %s\n", ModuleName, mod_strerror(-ret));
    } else {
      ret = 0;
    }
    kmod_module_unref(mod);

    end:
      kmod_unref(ctx);
  }

  if (ret != 0) {
      return NULL;
  } else {
    Py_INCREF (Py_None);
    return Py_None;
  }

} // kmodule_insmod
