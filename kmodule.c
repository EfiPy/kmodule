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
/// static function for modinfo
///
///////////////////////////////////////////////////////////////////////

/***********************************************************************
 *
 * is_module_filename:
 *
 ***********************************************************************/
static bool
is_module_filename (
  const char *name
  )
{
  struct stat st;

  if (stat(name, &st) == 0 && S_ISREG(st.st_mode) &&
    path_ends_with_kmod_ext(name, strlen(name)))
      return true;

  return false;
} // is_module_filename

static char separator = '\n';

struct param {
  struct param *next;
  const char *name;
  const char *param;
  const char *type;
  int namelen;
  int paramlen;
  int typelen;
};

/***********************************************************************
 *
 * add_param:
 *
 ***********************************************************************/
static struct param *add_param (
  const char    *name,
  int           namelen,
  const char    *param,
  int           paramlen,
  const char    *type,
  int           typelen,
  struct param  **list
  )
{
  struct param *it;

  for (it = *list; it != NULL; it = it->next) {
    if (it->namelen == namelen &&
      memcmp(it->name, name, namelen) == 0)
      break;
  }

  if (it == NULL) {
    it = malloc(sizeof(struct param));
    if (it == NULL)
      return NULL;
    it->next = *list;
    *list = it;
    it->name = name;
    it->namelen = namelen;
    it->param = NULL;
    it->type = NULL;
    it->paramlen = 0;
    it->typelen = 0;
  }

  if (param != NULL) {
    it->param = param;
    it->paramlen = paramlen;
  }

  if (type != NULL) {
    it->type = type;
    it->typelen = typelen;
  }

  return it;
} // add_param

/***********************************************************************
 *
 * process_parm:
 *
 ***********************************************************************/
static int
process_parm (
  const char    *key,
  const char    *value,
  struct param  **params
  )
{
  const char *name, *param, *type;
  int namelen, paramlen, typelen;
  struct param *it;
  const char *colon = strchr(value, ':');
  if (colon == NULL) {
    fprintf (stderr, "Found invalid \"%s=%s\": missing ':'\n",
        key, value);
    return 0;
  }

  name = value;
  namelen = colon - value;
  if (streq(key, "parm")) {
    param = colon + 1;
    paramlen = strlen(param);
    type = NULL;
    typelen = 0;
  } else {
    param = NULL;
    paramlen = 0;
    type = colon + 1;
    typelen = strlen(type);
  }

  it = add_param(name, namelen, param, paramlen, type, typelen, params);
  if (it == NULL) {
    PyErr_Format (PyExc_MemoryError, "Out of memory!\n");
    return -ENOMEM;
  }

  return 0;
} // process_parm

/***********************************************************************
 * kmodule_modinf_build_info:
 ***********************************************************************/
static int
kmodule_modinf_build_info (
  PyObject    *dict,
  const char  *name,
  const char  *val
  )
{
  int       ret;
  PyObject *TempStr;

  TempStr = PyUnicode_FromString (val);
  if (TempStr == NULL) return (-1);

  ret = PyDict_SetItemString (dict, name, TempStr);
  Py_DECREF (TempStr);

  return ret;
} // kmodule_modinf_build_info

/***********************************************************************
 *
 * modinfo_do:
 *
 ***********************************************************************/
static PyObject *modinfo_do (
  struct kmod_module *mod
  )
{
  struct kmod_list *l, *list = NULL;
  struct param *params = NULL;
  int err, is_builtin;
  const char *filename = kmod_module_get_path(mod);

  is_builtin = (filename == NULL);

  PyObject *ModInfo_info  = NULL;
  PyObject *ModInfo_param = NULL;
  int       alias_count   = 0;
  PyObject *ModInfo_alias = NULL;
  PyObject *ModInfo_string = NULL;

  if (is_builtin) {
    // printf("%-16s%s%c", "name:", kmod_module_get_name(mod), separator);
    filename = "(builtin)";

    ModInfo_info = Py_BuildValue ("{s:s}", "name", "(builtin)");
    if (ModInfo_info == NULL) return ModInfo_info;
  }

  if (ModInfo_info == NULL) {
    ModInfo_info = Py_BuildValue ("{s:s}", "filename", filename);
    if (ModInfo_info == NULL) return NULL;
  } else {
    err = kmodule_modinf_build_info (ModInfo_info, "filename", filename);
    if (err < 0) goto end;
  }

  err = kmod_module_get_info(mod, &list);
  if (err < 0) {
    if (is_builtin && err == -ENOENT) {
      /*
       * This is an old kernel that does not have a file
       * with information about built-in modules.
       */
    }
    PyErr_Format (PyExc_MemoryError, "could not get modinfo from '%s': %s\n",
      kmod_module_get_name(mod), strerror(-err));
    goto end;
  }

  kmod_list_foreach(l, list) {

    const char *key = kmod_module_info_get_key(l);
    const char *value = kmod_module_info_get_value(l);

    if (streq(key, "alias")) {
      // printf("%s=%s%c", key, value, separator);
      alias_count++;
      continue;
    }

    if (streq(key, "parm") || streq(key, "parmtype")) {
      err = process_parm(key, value, &params);
      if (err < 0)
        goto end;
      continue;
    }

    if (separator == '\0') {
      // printf("%s=%s%c", key, value, separator);
      err = kmodule_modinf_build_info (ModInfo_info, key, value);
      if (err < 0) goto end;
      continue;
    }

    // keylen = strlen(key);
    err = kmodule_modinf_build_info (ModInfo_info, key, value);
    if (err < 0) goto end;
    // printf("%s:%-*s%s%c", key, 15 - keylen, "", value, separator);
  }

  if (alias_count != 0) {

    ModInfo_alias = PyTuple_New (alias_count);
    if (ModInfo_alias == NULL) {
      err = -1;
      goto end;
    }

    alias_count = 0;
    kmod_list_foreach(l, list) {
      const char *key = kmod_module_info_get_key(l);
      const char *value = kmod_module_info_get_value(l);

      if (streq(key, "alias")) {
        // printf("%s=%s%c", key, value, separator);
        ModInfo_string = PyUnicode_FromString (value);
        PyTuple_SET_ITEM (ModInfo_alias, alias_count, ModInfo_string);
        alias_count++;
      }
    }
  }

  if (params != NULL) {

    ModInfo_param = PyDict_New ();

    if (ModInfo_param == NULL) {
      goto end;
    }
  }

  while (params != NULL) {
    char parm_name[512];
    char parm_value[2048];
    struct param *p = params;
    p = params;
    params = p->next;

    sprintf (parm_name, "%.*s", p->namelen, p->name);
    if (p->param == NULL) {
      // printf("%-16s%.*s:%.*s%c", "parm:",
      //     p->namelen, p->name, p->typelen, p->type,
      //     separator);

      sprintf (parm_value, "%.*s", p->typelen, p->type);
      kmodule_modinf_build_info (ModInfo_param, parm_name, parm_value);
    }
    else if (p->type != NULL) {
      // printf("%-16s%.*s:%.*s (%.*s)%c", "parm:",
      //        p->namelen, p->name,
      //        p->paramlen, p->param,
      //        p->typelen, p->type,
      //        separator);
      sprintf (parm_value, "%.*s (%.*s)", p->paramlen, p->param, p->typelen, p->type);
      kmodule_modinf_build_info (ModInfo_param, parm_name, parm_value);
    }
    else {
      // printf("%-16s%.*s:%.*s%c",
      //        "parm:",
      //        p->namelen, p->name,
      //        p->paramlen, p->param,
       //       separator);
      sprintf (parm_value, "%.*s", p->paramlen, p->param);
      kmodule_modinf_build_info (ModInfo_param, parm_name, parm_value);
    }

    free(p);
  }

end:
  while (params != NULL) {
    void *tmp = params;
    params = params->next;
    free(tmp);
  }
  kmod_module_info_free_list(list);

  if (err < 0) {
    Py_DECREF (ModInfo_info);
    ModInfo_info = NULL;

    if (ModInfo_param != NULL)
      Py_DECREF (ModInfo_param);
    if (ModInfo_alias != NULL)
      Py_DECREF (ModInfo_alias);

  } else {
    if (ModInfo_param != NULL) {
      PyDict_SetItemString (ModInfo_info, "parm", ModInfo_param);
      Py_DECREF (ModInfo_param);
    }
    if (ModInfo_alias != NULL) {
      PyDict_SetItemString (ModInfo_info, "alias", ModInfo_alias);
      Py_DECREF (ModInfo_alias);
    }
  }

  return ModInfo_info;
} // modinfo_do

/***********************************************************************
 *
 * modinfo_path_do:
 *
 ***********************************************************************/
static PyObject *
modinfo_path_do (
  struct kmod_ctx *ctx,
  const char      *path
  )
{
  PyObject *ret, *m;
  struct kmod_module *mod;
  int err = kmod_module_new_from_path(ctx, path, &mod);
  if (err < 0) {
    PyErr_Format (PyExc_MemoryError, "Module file %s not found.\n", path);
    return NULL;
  }
  ret = PyList_New (1);
  if (!ret) goto end;

  m = modinfo_do(mod);

  PyList_SET_ITEM (ret, 0, m);
end:
  kmod_module_unref(mod);
  return ret;
} // modinfo_path_do

/***********************************************************************
 *
 * modinfo_alias_do:
 *
 ***********************************************************************/
static PyObject * modinfo_alias_do (
  struct kmod_ctx *ctx,
  const char      *alias
  )
{
  PyObject  *ret;
  int        count;
  struct kmod_list *l, *list = NULL;

  int err = kmod_module_new_from_lookup(ctx, alias, &list);
  if (err < 0) {
    PyErr_Format (PyExc_MemoryError, "Module alias %s not found.\n", alias);
    return NULL;
  }

  if (list == NULL) {
    PyErr_Format (PyExc_MemoryError, "Module %s not found.\n", alias);
    return NULL;
  }

  count = 0;
  kmod_list_foreach(l, list) {
    count++;
  }

  ret = PyList_New (count);
  if (!ret) goto end;

  count = 0;
  kmod_list_foreach(l, list) {
    PyObject *m;
    struct kmod_module *mod = kmod_module_get_module(l);

    m = modinfo_do(mod);

    PyList_SET_ITEM (ret, count, m);
    count++;

    kmod_module_unref(mod);
  }
end:
  kmod_module_unref_list(list);

  return ret;
} // modinfo_alias_do

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

/***********************************************************************
 *
 * kmodule_modinfo:
 *
 ***********************************************************************/
static PyObject *
kmodule_modinfo (
  PyObject    *Self,
  PyObject    *Args,
  PyObject    *KwArgs
  )
{
  char  *root, *kversion;
  char  *module;

  struct kmod_ctx *ctx;
  char dirname_buf[PATH_MAX];
  const char *dirname = NULL;
  const char *null_config = NULL;

  PyObject   *ret;

  static char   *kwlist[] = {"module", "basedir", "kversion", NULL};

  if (!PyArg_ParseTupleAndKeywords (
      Args,
      KwArgs,
      "s|zz",
      kwlist,
      &module,
      &root,
      &kversion)) {
    return NULL;
  }

  if (root != NULL || kversion != NULL) {
    struct utsname u;
    if (root == NULL)
      root = "";
    if (kversion == NULL) {
      if (uname(&u) < 0) {
        PyErr_Format (PyExc_MemoryError, "uname() failed: %m\n");
        return NULL;
      }
      kversion = u.release;
    }
    snprintf(dirname_buf, sizeof(dirname_buf), "%s/lib/modules/%s",
       root, kversion);
    dirname = dirname_buf;
  }

  ctx = kmod_new(dirname, &null_config);
  if (!ctx) {
    PyErr_Format (PyExc_MemoryError, "kmod_new() failed!\n");
    return NULL;
  }

  if (is_module_filename(module))
    ret = modinfo_path_do(ctx, module);
  else
    ret = modinfo_alias_do(ctx, module);

  kmod_unref(ctx);

  return ret;

} // kmodule_modinfo

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
