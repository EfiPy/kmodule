/*
 * kmod-rmmod - remove modules from linux kernel using libkmod.
 *
 * Copyright (C) 2011-2013  ProFUSION embedded systems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef KMODULEPY
#include <Python.h>
#define PYSAMPLE_MODULE
#include "structmember.h"

#include <sys/utsname.h>
#include <libkmod/libkmod.h>

#include <shared/util.h>
#include <tools/kmod.h>
#else
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <shared/macro.h>

#include <libkmod/libkmod.h>

#include "kmod.h"

#define DEFAULT_VERBOSE LOG_ERR
static int verbose = DEFAULT_VERBOSE;
static int use_syslog;

static const char cmdopts_s[] = "fsvVwh";
static const struct option cmdopts[] = {
	{"force", no_argument, 0, 'f'},
	{"syslog", no_argument, 0, 's'},
	{"verbose", no_argument, 0, 'v'},
	{"version", no_argument, 0, 'V'},
	{"help", no_argument, 0, 'h'},
	{NULL, 0, 0, 0}
};

static void help(void)
{
	printf("Usage:\n"
		"\t%s [options] modulename ...\n"
		"Options:\n"
		"\t-f, --force       forces a module unload and may crash your\n"
		"\t                  machine. This requires Forced Module Removal\n"
		"\t                  option in your kernel. DANGEROUS\n"
		"\t-s, --syslog      print to syslog, not stderr\n"
		"\t-v, --verbose     enables more messages\n"
		"\t-V, --version     show version\n"
		"\t-h, --help        show this help\n",
		program_invocation_short_name);
}
#endif // KMODULEPY

static int check_module_inuse(struct kmod_module *mod) {
	struct kmod_list *holders;
	int state, ret;

	state = kmod_module_get_initstate(mod);

	if (state == KMOD_MODULE_BUILTIN) {
		ERR("Module %s is builtin.\n", kmod_module_get_name(mod));
		return -ENOENT;
	} else if (state < 0) {
		ERR("Module %s is not currently loaded\n",
				kmod_module_get_name(mod));
		return -ENOENT;
	}

	holders = kmod_module_get_holders(mod);
	if (holders != NULL) {
		struct kmod_list *itr;

		ERR("Module %s is in use by:", kmod_module_get_name(mod));

		kmod_list_foreach(itr, holders) {
			struct kmod_module *hm = kmod_module_get_module(itr);
			fprintf(stderr, " %s", kmod_module_get_name(hm));
			kmod_module_unref(hm);
		}
		fputc('\n', stderr);

		kmod_module_unref_list(holders);
		return -EBUSY;
	}

	ret = kmod_module_get_refcnt(mod);
	if (ret > 0) {
		ERR("Module %s is in use\n", kmod_module_get_name(mod));
		return -EBUSY;
	} else if (ret == -ENOENT) {
		ERR("Module unloading is not supported\n");
	}

	return ret;
}

#define DEFAULT_VERBOSE LOG_ERR
/***********************************************************************
 *
 * kmodule_rmmod:
 *
 ***********************************************************************/
PyObject *
kmodule_rmmod (
  PyObject    *Self,
  PyObject    *Args,
  PyObject    *KwArgs
  )
{
  PyObject  *ret;
  int       wait = 0, force = 0, verbose=DEFAULT_VERBOSE;
  PyObject  *modules;

  static char   *kwlist[] = {"module", "force", "wait", "verbose", NULL};

  if (!PyArg_ParseTupleAndKeywords (
      Args,
      KwArgs,
      "O|ppi",
      kwlist,
      &modules,
      &wait,
      &force,
      &verbose)) {
    return NULL;
  }
  verbose += DEFAULT_VERBOSE;

  if (1)
  {
    struct kmod_ctx *ctx;
    const char      *null_config = NULL;
    struct kmod_module *mod;

    Py_ssize_t      i, mNum;
    PyObject        *modName;

    int flags = KMOD_REMOVE_NOWAIT;

    if (force) flags |=  KMOD_REMOVE_FORCE;
    if (wait)  flags &= ~KMOD_REMOVE_NOWAIT;

    ctx = kmod_new(NULL, &null_config);
      if (!ctx) {
        ERR("kmod_new() failed!\n");
        PyErr_Format (PyExc_MemoryError, "kmod_new() failed!");
        return NULL;
      }

    ret = Py_None;
    Py_INCREF (Py_None);
    log_setup_kmod_log(ctx, verbose);

    mNum = PyTuple_Size (modules);
    for (i = 0; i < mNum; i++) {

      const char *modStr;
      struct stat st;
      int         err;

      modName = PyTuple_GetItem (modules, i);
      if (modName == NULL) {
        PyErr_Print ();
        PyErr_Clear ();
        continue;
      }
      modStr = PyUnicode_AsUTF8 (modName);
      if (modStr == NULL) {
        PyErr_Print ();
        PyErr_Clear ();
        continue;
      }

      if (stat(modStr, &st) == 0)
        err = kmod_module_new_from_path(ctx, modStr, &mod);
      else
        err = kmod_module_new_from_name(ctx, modStr, &mod);

      if (err < 0) {
        ERR("could not use module %s: %s\n", modStr, strerror(-err));
        PyErr_Format (PyExc_OSError, "could not use module %s: %s\n", modStr, strerror(-err));
        ret = NULL;
        Py_DECREF (Py_None);
        break;
      }

      if (!(flags & KMOD_REMOVE_FORCE) && check_module_inuse(mod) < 0) {
        goto next;
      }

      err = kmod_module_remove_module(mod, flags);
      if (err < 0) {
        ERR ("could not remove module %s: %s\n", modStr, strerror(-err));
      }
next:
      kmod_module_unref(mod);

    }

    kmod_unref(ctx);
  }

  return ret;

} // kmodule_rmmod

#ifndef KMODULEPY
const struct kmod_cmd kmod_cmd_compat_rmmod = {
	.name = "rmmod",
	.cmd = do_rmmod,
	.help = "compat rmmod command",
};
#endif
