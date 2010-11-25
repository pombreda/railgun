/* -*- c -*- */
#include <Python.h>
#include "structmember.h"
#include <numpy/arrayobject.h>

#define CStyle_MAXDIM 5

typedef struct cstyle_{
  PyObject_HEAD
  PyObject *pyarray;
  Py_ssize_t pointer;
  void *carray;
} CStyle;


void **
cstyle2d_alloc(PyObject* pyarray)
{
  int i, num0;
  void **carray;

  num0 = PyArray_DIM(pyarray, 0);
  carray = (void**) malloc(sizeof(void*) * num0);
  if (carray == NULL){
    return NULL;
  }
  for (i = 0; i < num0; ++i){
    carray[i] = PyArray_GETPTR2(pyarray, i, 0);
  }
  return carray;
}


void
cstyle2d_free(void **carray)
{
  free(carray);
}


void ***
cstyle3d_alloc(PyObject* pyarray)
{
  int i, j, num0, num1;
  void ***carray;

  num0 = PyArray_DIM(pyarray, 0);
  num1 = PyArray_DIM(pyarray, 1);
  carray = (void***) malloc(sizeof(void**) * num0);
  if (carray == NULL){
    return NULL;
  }
  for (i = 0; i < num0; ++i){
    carray[i] = (void**) malloc(sizeof(void*) * num1);
  }
  for (i = 0; i < num0; ++i){
    for (j = 0; j < num1; ++j){
      carray[i][j] = PyArray_GETPTR3(pyarray, i, j, 0);
    }
  }
  return carray;
}


void
cstyle3d_free(void ***carray, PyObject* pyarray)
{
  int i, num0;
  num0 = PyArray_DIM(pyarray, 0);

  for (i = 0; i < num0; ++i){
    free(carray[i]);
  }
  free(carray);
}


void ****
cstyle4d_alloc(PyObject* pyarray)
{
  int i, j, k, num0, num1, num2;
  void ****carray;

  num0 = PyArray_DIM(pyarray, 0);
  num1 = PyArray_DIM(pyarray, 1);
  num2 = PyArray_DIM(pyarray, 2);
  carray = (void****) malloc(sizeof(void***) * num0);
  if (carray == NULL){
    return NULL;
  }
  for (i = 0; i < num0; ++i){
    carray[i] = (void***) malloc(sizeof(void**) * num1);
    for (j = 0; j < num1; ++j){
      carray[i][j] = (void**) malloc(sizeof(void*) * num2);
    }
  }
  for (i = 0; i < num0; ++i){
    for (j = 0; j < num1; ++j){
      for (k = 0; k < num2; ++k){
        carray[i][j][k] = PyArray_GETPTR4(pyarray, i, j, k, 0);
      }
    }
  }
  return carray;
}


void
cstyle4d_free(void ****carray, PyObject* pyarray)
{
  int i, j, num0, num1;
  num0 = PyArray_DIM(pyarray, 0);
  num1 = PyArray_DIM(pyarray, 1);

  for (i = 0; i < num0; ++i){
    for (j = 0; j < num1; ++j){
      free(carray[i][j]);
    }
    free(carray[i]);
  }
  free(carray);
}


void *****
cstyle5d_alloc(PyObject* pyarray)
{
  int i, j, k, l, num0, num1, num2, num3;
  npy_intp ind[5];
  void *****carray;

  num0 = PyArray_DIM(pyarray, 0);
  num1 = PyArray_DIM(pyarray, 1);
  num2 = PyArray_DIM(pyarray, 2);
  num3 = PyArray_DIM(pyarray, 3);
  carray = (void*****) malloc(sizeof(void****) * num0);
  if (carray == NULL){
    return NULL;
  }
  for (i = 0; i < num0; ++i){
    carray[i] = (void****) malloc(sizeof(void***) * num1);
    for (j = 0; j < num1; ++j){
      carray[i][j] = (void***) malloc(sizeof(void**) * num2);
      for (k = 0; k < num2; ++k){
        carray[i][j][k] = (void**) malloc(sizeof(void*) * num3);
      }
    }
  }
  ind[4] = 0;
  for (i = 0; i < num0; ++i){
    ind[0] = i;
    for (j = 0; j < num1; ++j){
      ind[1] = j;
      for (k = 0; k < num2; ++k){
        ind[2] = k;
        for (l = 0; l < num3; ++l){
          ind[3] = l;
          carray[i][j][k][l] = PyArray_GetPtr((PyArrayObject*) pyarray, ind);
        }
      }
    }
  }
  return carray;
}


void
cstyle5d_free(void *****carray, PyObject* pyarray)
{
  int i, j, k, num0, num1, num2;
  num0 = PyArray_DIM(pyarray, 0);
  num1 = PyArray_DIM(pyarray, 1);
  num2 = PyArray_DIM(pyarray, 2);

  for (i = 0; i < num0; ++i){
    for (j = 0; j < num1; ++j){
      for (k = 0; k < num2; ++k){
        free(carray[i][j][k]);
      }
      free(carray[i][j]);
    }
    free(carray[i]);
  }
  free(carray);
}


static void
CStyle_dealloc(CStyle *self)
{
  if (self->pyarray == NULL) return;

  switch (PyArray_NDIM(self->pyarray)){
  case 2: cstyle2d_free(self->carray); break;
  case 3: cstyle3d_free(self->carray, self->pyarray); break;
  case 4: cstyle4d_free(self->carray, self->pyarray); break;
  case 5: cstyle5d_free(self->carray, self->pyarray); break;
  }

  Py_XDECREF(self->pyarray);
}


static int
CStyle_init(CStyle *self, PyObject *args, PyObject *kwds)
{
  PyObject *pyarray=NULL, *tmp;
  static char *kwlist[] = {"pyarray", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist,
                                   &PyArray_Type, &pyarray)){
    return -1;
  }
  /* check dimension (<= CStyle_MAXDIM) */
  if (PyArray_NDIM(pyarray) > CStyle_MAXDIM){
    PyErr_Format(PyExc_ValueError,
                 "ndim of given numpy array (=%d) is larger than %d",
                 PyArray_NDIM(pyarray), CStyle_MAXDIM);
    return -1;
  }else if (PyArray_NDIM(pyarray) == 1){
    PyErr_SetString(PyExc_ValueError,
                    "CStyle is not needed for 1D array!");
    return -1;
  }

  /* set to member */
  if (pyarray) {
    tmp = self->pyarray;
    Py_INCREF(pyarray);
    self->pyarray = pyarray;
    Py_XDECREF(tmp);
  }

  /* Allocate C-array */
  switch (PyArray_NDIM(pyarray)){
  case 2: self->carray = cstyle2d_alloc(self->pyarray); break;
  case 3: self->carray = cstyle3d_alloc(self->pyarray); break;
  case 4: self->carray = cstyle4d_alloc(self->pyarray); break;
  case 5: self->carray = cstyle5d_alloc(self->pyarray); break;
  }

  if (self->carray == NULL){
    PyErr_SetString(PyExc_RuntimeError, "failed to allocate memory");
    Py_XDECREF(pyarray);
    return -1;
  }
  self->pointer = (Py_ssize_t)self->carray;

  return 0;
}


static PyMemberDef CStyle_members[] = {
  {"pyarray", T_OBJECT_EX, offsetof(CStyle, pyarray), READONLY,
   "numpy array"},
  {"pointer", T_PYSSIZET, offsetof(CStyle, pointer), READONLY,
   "pointer for accessing numpy array in C style (a[i][j][k])"},
  {NULL}  /* Sentinel */
};


static PyTypeObject CStyleType = {
  PyObject_HEAD_INIT(NULL)
  0,                               /*ob_size*/
  "cstyle.CStyle",       /*tp_name*/
  sizeof(CStyle),             /*tp_basicsize*/
  0,                               /*tp_itemsize*/
  (destructor)CStyle_dealloc, /*tp_dealloc*/
  0,                               /*tp_print*/
  0,                               /*tp_getattr*/
  0,                               /*tp_setattr*/
  0,                               /*tp_compare*/
  0,                               /*tp_repr*/
  0,                               /*tp_as_number*/
  0,                               /*tp_as_sequence*/
  0,                               /*tp_as_mapping*/
  0,                               /*tp_hash */
  0,                               /*tp_call*/
  0,                               /*tp_str*/
  0,                               /*tp_getattro*/
  0,                               /*tp_setattro*/
  0,                               /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,              /*tp_flags*/
  "CStyle object",            /* tp_doc */
  0,                               /* tp_traverse */
  0,                               /* tp_clear */
  0,                               /* tp_richcompare */
  0,                               /* tp_weaklistoffset */
  0,                               /* tp_iter */
  0,                               /* tp_iternext */
  0,                               /* tp_methods */
  CStyle_members,             /* tp_members */
  0,                               /* tp_getset */
  0,                               /* tp_base */
  0,                               /* tp_dict */
  0,                               /* tp_descr_get */
  0,                               /* tp_descr_set */
  0,                               /* tp_dictoffset */
  (initproc)CStyle_init,      /* tp_init */
};


static PyMethodDef module_methods[] = {
  {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initcstyle(void)
{
  PyObject* m;

  CStyleType.tp_new = PyType_GenericNew;
  if (PyType_Ready(&CStyleType) < 0){
    return;
  }

  m = Py_InitModule3("cstyle", module_methods,
                     "this module provides CStyle");

  if (m == NULL){
    return;
  }

  Py_INCREF(&CStyleType);
  PyModule_AddObject(m, "CStyle", (PyObject *)&CStyleType);
  PyModule_AddIntConstant(m, "MAXDIM", CStyle_MAXDIM);

  import_array();  /* NumPy initialization */
}