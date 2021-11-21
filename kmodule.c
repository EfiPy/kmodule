/*
 * kmodule.c: python wrapper for python wrapper for kmod/tools
 *             insmod, rmmod, lsmod, modinfo
 *   Copyright (C) 2020 - 2021  MaxWu <EfiPy.core@gmail.com>.

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
/// kmodule public function
///
///////////////////////////////////////////////////////////////////////

PyObject *
kmodule_insmod (
  PyObject    *Self,
  PyObject    *Args,
  PyObject    *KwArgs
  );

PyObject *
kmodule_rmmod (
  PyObject    *Self,
  PyObject    *Args,
  PyObject    *KwArgs
  );

/***********************************************************************
 *
 * kmodule_modprobe:
 *
 ***********************************************************************/
static PyObject *
kmodule_modprobe (
  PyObject    *Self,
  PyObject    *Args,
  PyObject    *KwArgs
  )
{
  PyErr_Format (PyExc_MemoryError, "modprobe not implement, yet.");
  return NULL;
} // kmodule_modprobe

PyObject *
kmodule_modinfo (
  PyObject    *Self,
  PyObject    *Args,
  PyObject    *KwArgs
  );

/***********************************************************************
 *
 * kmodule_logging:
 *
 ***********************************************************************/
static PyObject *
kmodule_logging (
  PyObject    *Self,
  PyObject    *Args
  )
{
  int logging;

  if (!PyArg_ParseTuple (Args, "p", &logging)) {
      return NULL;
  }

  if (logging) {
    log_open (logging);
  }
  else {
    log_close();
  }

  Py_INCREF (Py_None);
  return Py_None;

} // kmodule_logging

///////////////////////////////////////////////////////////////////////
///
/// PyMethodDef of kmodule
///
///////////////////////////////////////////////////////////////////////

static PyMethodDef kmodule_methods [] = {

  { "_modinfo",     (PyCFunction) kmodule_modinfo,  METH_VARARGS | METH_KEYWORDS, NULL},
  { "_rmmod",       (PyCFunction) kmodule_rmmod,    METH_VARARGS | METH_KEYWORDS, NULL},
  { "_insmod",      (PyCFunction) kmodule_insmod,   METH_VARARGS | METH_KEYWORDS, NULL},
  { "_modprobe",    (PyCFunction) kmodule_modprobe, METH_VARARGS | METH_KEYWORDS, NULL},
  { "_logging",     kmodule_logging,                METH_VARARGS, NULL},

  { NULL, NULL, 0, NULL}

}; // kmodule_methods

/***************************************************************************
 *
 * Module structure
 *
 ***************************************************************************/
static struct PyModuleDef kmodule = {

  PyModuleDef_HEAD_INIT,

  "_kmodule",           /* name of module */
  "_kmodule module",    /* Doc string (may be NULL) */
  -1,                   /* Size of per-interpreter state or -1 */
  kmodule_methods       /* Method table */

}; // kmodule

///////////////////////////////////////////////////////////////////////
///
/// Module Initialization
///
///////////////////////////////////////////////////////////////////////

/***********************************************************************
 *
 * PyInit__kmodule:
 *
 ***********************************************************************/
PyMODINIT_FUNC
PyInit__kmodule (
  void
  )
{
  Py_Initialize();

  return PyModule_Create(&kmodule);

} // PyInit__kmodule
