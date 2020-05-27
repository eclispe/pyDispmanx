/*  PyDispmanx provides a buffer interface to a Raspberry Pi GPU layer
*   Copyright (C) 2020  Tim Clark
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

#include "imageLayer.h"

#include "bcm_host.h"

// Python layer object struct
typedef struct {
    PyObject_HEAD
    int32_t number;
    IMAGE_LAYER_T imageLayer;
    DISPMANX_DISPLAY_HANDLE_T display;
} dispmanxLayer;

// setup the display when the object is created
static PyObject *dispmanxLayer_new (PyTypeObject *type, PyObject *args, PyObject *kwds)  {
    dispmanxLayer *self;
    self = (dispmanxLayer *) type->tp_alloc (type,0);

    bcm_host_init();
    if (self != NULL) {
        self->number = 1;
        self->display = vc_dispmanx_display_open (0);
        if (self->display == 0) {
            vc_dispmanx_display_close (self->display);
            return NULL;
        }
    }
    return (PyObject *) self;
}

// create a fullscreen transparent layer when a new object is created
static int dispmanxLayer_init (dispmanxLayer *self, PyObject *args, PyObject *kwds)  {
    if (!PyArg_ParseTuple (args, "i", &self->number)) {
        return -1;
    }
    DISPMANX_MODEINFO_T info;
    vc_dispmanx_display_get_info (self->display, &info);
    initImage (& (self->imageLayer.image), VC_IMAGE_RGBA32, info.width, info.height, true);
    createResourceImageLayer (& (self->imageLayer), self->number);
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start (0);
    addElementImageLayerOffset (& (self->imageLayer), 0, 0, self->display, update);
    vc_dispmanx_update_submit_sync (update);
    return 0;
}


// when the object is deleted delete both the layer and the display
static void dispmanxLayer_dealloc (dispmanxLayer *self) {
    destroyImageLayer (& (self->imageLayer));
    vc_dispmanx_display_close (self->display);
}

// function to trigger an update to the display
static PyObject *method_updateLayer (dispmanxLayer *self, PyObject *args) {
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start (0);
    changeSourceImageLayer (& (self->imageLayer), update);
    vc_dispmanx_update_submit_sync (update);
    Py_RETURN_TRUE;
}

static PyMethodDef dispmanxMethods[] = {
    {"updateLayer", (PyCFunction) method_updateLayer, METH_NOARGS, "update display to show current buffer"},
    {NULL}
};

// getter for the size of the display as part of the object
static PyObject *dispmanx_getsize (dispmanxLayer *self, void *closure) {
    DISPMANX_MODEINFO_T info;
    vc_dispmanx_display_get_info (self->display, &info);
    return Py_BuildValue ("(ii)", info.width, info.height);
}

static PyGetSetDef dispmanx_getsetters[] = {
    {"size", (getter) dispmanx_getsize, NULL, "display size", NULL},
    {NULL}  /* Sentinel */
};

static PyMemberDef dispmanxLayer_members[] = {
    {"number", T_INT, offsetof (dispmanxLayer, number), 0, "layer number"},
    {NULL}
};

// setup the buffer interface to access the underlying buffer
static int dispmanxLayer_getbuffer (dispmanxLayer *self, Py_buffer *view, int flags) {
    if (view == NULL) {
        PyErr_SetString (PyExc_ValueError, "NULL view in getbuffer");
        return -1;
    }

    view->obj = (PyObject *)self;
    view->buf = (void *)self->imageLayer.image.buffer;
    view->len = self->imageLayer.image.size/sizeof (char);
    view->readonly = 0;
    view->itemsize = sizeof (char);
    view->format = "c";  // character
    view->ndim = 1;
    view->shape = &view->len;
    view->strides = &view->itemsize; 
    view->suboffsets = NULL;
    view->internal = NULL;

    Py_INCREF (self); // need to increase the reference count
    return 0;
}

static PyBufferProcs dispmanxLayer_as_buffer = {
    (getbufferproc)dispmanxLayer_getbuffer,
    (releasebufferproc)0,
};

// object definition
static PyTypeObject dispmanxLayerType = {
    PyVarObject_HEAD_INIT (NULL, 0)
    .tp_name = "dispmanx.dispmanxLayer",
    .tp_doc = "displamanx layer",
    .tp_basicsize = sizeof (dispmanxLayer),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT || Py_TPFLAGS_BASETYPE,
    .tp_new = dispmanxLayer_new,
    .tp_init = (initproc)dispmanxLayer_init,
    .tp_dealloc = (destructor) dispmanxLayer_dealloc,
    .tp_members = dispmanxLayer_members,
    .tp_methods = dispmanxMethods,
    .tp_getset = dispmanx_getsetters,
    .tp_as_buffer = &dispmanxLayer_as_buffer,
};


// function to get the display size directly from the module
static PyObject *pydispmanx_getDisplaySize (PyObject *self, void *closure) {
    bcm_host_init();
    DISPMANX_DISPLAY_HANDLE_T display = vc_dispmanx_display_open (0);
    if (display!=0) {
        DISPMANX_MODEINFO_T info;
        vc_dispmanx_display_get_info (display, &info);
        vc_dispmanx_display_close (display);
        return Py_BuildValue ("(ii)", info.width, info.height);
    } else {
        Py_RETURN_FALSE;
    }
}

static PyMethodDef pydispmanxMethods[] = {
    {"getDisplaySize", (PyCFunction) pydispmanx_getDisplaySize, METH_NOARGS, "Get the display size as a tuple"},
    {NULL}
};

// module defintion
static struct PyModuleDef dispmanxModule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pydispmanx",
    .m_doc = "Module for displaying things with raspberry pi dispmanx interface",
    .m_size = -1,
    .m_methods = pydispmanxMethods,
};

PyMODINIT_FUNC PyInit_pydispmanx (void) {
    PyObject *m;
    if (PyType_Ready (&dispmanxLayerType) < 0) {
        return NULL;
    }

    m=PyModule_Create (&dispmanxModule);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF (&dispmanxLayerType);
    if (PyModule_AddObject (m, "dispmanxLayer", (PyObject *) &dispmanxLayerType) < 0) {
        Py_DECREF (&dispmanxLayerType);
        Py_DECREF (m);
        return NULL;
    }
    return m;
}
