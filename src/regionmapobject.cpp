/*
 *
 * Copyright (C) 2002 Ichiro Fujinaga, Michael Droettboom, and Karl MacMillan
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "gameramodule.hpp"

using namespace Gamera;

extern "C" {
  static PyObject* regionmap_new(PyTypeObject* pytype, PyObject* args,
			     PyObject* kwds);
  static void regionmap_dealloc(PyObject* self);
  static PyObject* regionmap_lookup(PyObject* self, PyObject* args);
  static PyObject* regionmap_add_region(PyObject* self, PyObject* args);
}

static PyTypeObject RegionMapType = {
  PyObject_HEAD_INIT(NULL)
  0,
};

static PyMethodDef regionmap_methods[] = {
  { "lookup", regionmap_lookup, METH_VARARGS },
  { "add_region", regionmap_add_region, METH_VARARGS },
  { NULL }
};

PyTypeObject* get_RegionMapType() {
  return &RegionMapType;
}

bool is_RegionMapType(PyObject* x) {
  if (PyObject_TypeCheck(x, &RegionMapType))
    return true;
  else
    return false;
}

PyObject* create_RegionMapObject(const RegionMap& r) {
  RegionMapObject* so = (RegionMapObject*) RegionMapType.tp_alloc(&RegionMapType, 0);
  so->m_x = new RegionMap(r);
  return (PyObject*)so;
}

static PyObject* regionmap_new(PyTypeObject* pytype, PyObject* args,
			    PyObject* kwds) {
    RegionMapObject* so;
    so = (RegionMapObject*)pytype->tp_alloc(pytype, 0);
    so->m_x = new RegionMap();
    return (PyObject*)so;
}

static void regionmap_dealloc(PyObject* self) {
  RegionMapObject* r = (RegionMapObject*)self;
  delete r->m_x;
  self->ob_type->tp_free(self);
}

static PyObject* regionmap_lookup(PyObject* self, PyObject* args) {
  PyObject* key;
  if (PyArg_ParseTuple(args, "O", &key) <= 0)
    return 0;
  if (!is_RectObject(key)) {
    PyErr_SetString(PyExc_TypeError, "Key must be a Rect!");
    return 0;
  }
  RegionMapObject* r = (RegionMapObject*)self;
  Region tmp = r->m_x->lookup(*((RectObject*)key)->m_x);
  return create_RegionObject(tmp);
}

static PyObject* regionmap_add_region(PyObject* self, PyObject* args) {
  PyObject* key;
  if (PyArg_ParseTuple(args, "O", &key) <= 0)
    return 0;
  if (!is_RegionObject(key)) {
    PyErr_SetString(PyExc_TypeError, "Must be a Region!");
    return 0;
  }
  RegionMapObject* r = (RegionMapObject*)self;
  Region* region = (Region*)((RectObject*)key)->m_x;
  r->m_x->add_region(*region);
  Py_INCREF(Py_None);
  return Py_None;
}

void init_RegionMapType(PyObject* module_dict) {
  RegionMapType.ob_type = &PyType_Type;
  RegionMapType.tp_name = "gameracore.RegionMap";
  RegionMapType.tp_basicsize = sizeof(RegionMapObject);
  RegionMapType.tp_dealloc = regionmap_dealloc;
  RegionMapType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  RegionMapType.tp_methods = regionmap_methods;
  RegionMapType.tp_new = regionmap_new;
  RegionMapType.tp_getattro = PyObject_GenericGetAttr;
  RegionMapType.tp_alloc = PyType_GenericAlloc;
  RegionMapType.tp_free = _PyObject_Del;
  PyType_Ready(&RegionMapType);
  PyDict_SetItemString(module_dict, "RegionMap", (PyObject*)&RegionMapType);
}
