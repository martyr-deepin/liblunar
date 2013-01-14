/* 
 * Copyright (C) 2013 Deepin, Inc.
 *               2013 Zhai Xiang
 *
 * Author:     Zhai Xiang <zhaixiang@linuxdeepin.com>
 * Maintainer: Zhai Xiang <zhaixiang@linuxdeepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Python.h>
#include <pygobject.h>
#include <lunar-calendar/lunar-calendar.h>

#define ERROR(v) PyErr_SetString(PyExc_TypeError, v)
#define INT(v) PyInt_FromLong(v)

/* Safe XDECREF for object states that handles nested deallocations */
#define ZAP(v) do {\
    PyObject *tmp = (PyObject *)(v); \
    (v) = NULL; \
    Py_XDECREF(tmp); \
} while (0)

typedef struct {
    PyObject_HEAD
    PyObject *dict; /* Python attributes dictionary */
    GtkWidget *handle;
} DeepinLunarObject;

static PyObject *m_deepin_lunar_object_constants = NULL;
static PyTypeObject *m_DeepinLunar_Type = NULL;

static DeepinLunarObject *m_init_deepin_lunar_object();
static DeepinLunarObject *m_new(PyObject *self, PyObject *args);

static PyMethodDef deepin_lunar_methods[] = 
{
    {"new", m_new, METH_NOARGS, "Deepin Lunar Construction"}, 
    {NULL, NULL, 0, NULL}
};

static PyObject *m_delete(DeepinLunarObject *self);
static PyObject *m_get_handle(DeepinLunarObject *self);
static PyObject *m_get_date(DeepinLunarObject *self);
static PyObject *m_mark_day(DeepinLunarObject *self, PyObject *args);
static PyObject *m_clear_marks(DeepinLunarObject *self);
static PyObject *m_set_editable(DeepinLunarObject *self, PyObject *args);

static PyMethodDef deepin_lunar_object_methods[] = 
{
    {"delete", m_delete, METH_NOARGS, "Deepin Lunar Object Destruction"}, 
    {"get_handle", m_get_handle, METH_NOARGS, "Get pygobject"}, 
    {"get_date", m_get_date, METH_NOARGS, "Get Date"}, 
    {"mark_day", m_mark_day, METH_VARARGS, "Mark day"}, 
    {"clear_marks", m_clear_marks, METH_NOARGS, "Clear all Marks"}, 
    {"set_editable", m_set_editable, METH_VARARGS, "Set Editable status"}, 
    {NULL, NULL, 0, NULL}
};

static void m_deepin_lunar_dealloc(DeepinLunarObject *self) 
{
    PyObject_GC_UnTrack(self);
    Py_TRASHCAN_SAFE_BEGIN(self)

    ZAP(self->dict);
    m_delete(self);

    PyObject_GC_Del(self);
    Py_TRASHCAN_SAFE_END(self)
}

static PyObject *m_getattr(PyObject *co, 
                           char *name, 
                           PyObject *dict1, 
                           PyObject *dict2, 
                           PyMethodDef *m)
{
    PyObject *v = NULL;
    
    if (!v && dict1)
        v = PyDict_GetItemString(dict1, name);
    if (!v && dict2)
        v = PyDict_GetItemString(dict2, name);
    if (v) {
        Py_INCREF(v);
        return v;
    }
    
    return Py_FindMethod(m, co, name);
}

static int m_setattr(PyObject **dict, char *name, PyObject *v)
{
    if (!v) {
        int rv = -1;
        if (*dict)
            rv = PyDict_DelItemString(*dict, name);
        if (rv < 0) {
            PyErr_SetString(PyExc_AttributeError, 
                            "delete non-existing attribute");
            return rv;
        }
    }
    if (!*dict) {
        *dict = PyDict_New();
        if (!*dict)
            return -1;
    }
    return PyDict_SetItemString(*dict, name, v);
}

static PyObject *m_deepin_lunar_getattr(DeepinLunarObject *dlo, 
                                        char *name) 
{
    return m_getattr((PyObject *)dlo, 
                     name, 
                     dlo->dict, 
                     m_deepin_lunar_object_constants, 
                     deepin_lunar_object_methods);
}

static PyObject *m_deepin_lunar_setattr(DeepinLunarObject *dlo, 
                                        char *name, 
                                        PyObject *v) 
{
    return m_setattr(&dlo->dict, name, v);
}

static PyObject *m_deepin_lunar_traverse(DeepinLunarObject *self, 
                                         visitproc visit, 
                                         void *args) 
{
    int err;
#undef VISIT
#define VISIT(v)    if ((v) != NULL && ((err = visit(v, args)) != 0)) return err

    VISIT(self->dict);

    return 0;
#undef VISIT
}

static PyObject *m_deepin_lunar_clear(DeepinLunarObject *self) 
{
    ZAP(self->dict);
    return 0;
}

static PyTypeObject DeepinLunar_Type = {
    PyObject_HEAD_INIT(NULL)
    0, 
    "deepin_lunar.new", 
    sizeof(DeepinLunarObject), 
    0, 
    (destructor)m_deepin_lunar_dealloc,
    0, 
    (getattrfunc)m_deepin_lunar_getattr, 
    (setattrfunc)m_deepin_lunar_setattr, 
    0, 
    0, 
    0,  
    0,  
    0,  
    0,  
    0,  
    0,  
    0,  
    0,  
    Py_TPFLAGS_HAVE_GC,
    0,  
    (traverseproc)m_deepin_lunar_traverse, 
    (inquiry)m_deepin_lunar_clear
};

PyMODINIT_FUNC initdeepin_lunar() 
{
    PyObject *m = NULL;

    init_pygobject();
             
    m_DeepinLunar_Type = &DeepinLunar_Type;
    DeepinLunar_Type.ob_type = &PyType_Type;

    m = Py_InitModule("deepin_lunar", deepin_lunar_methods);
    if (!m)
        return;

    m_deepin_lunar_object_constants = PyDict_New();
}

static DeepinLunarObject *m_init_deepin_lunar_object() 
{
    DeepinLunarObject *self = NULL;

    self = (DeepinLunarObject *) PyObject_GC_New(DeepinLunarObject, 
                                                 m_DeepinLunar_Type);
    if (!self)
        return NULL;
    PyObject_GC_Track(self);

    self->dict = NULL;
    self->handle = NULL;

    return self;
}

static DeepinLunarObject *m_new(PyObject *dummy, PyObject *args) 
{
    DeepinLunarObject *self = NULL;
    
    self = m_init_deepin_lunar_object();
    if (!self)
        return NULL;
    
    self->handle = lunar_calendar_new();
    
    return self;
}

static PyObject *m_get_handle(DeepinLunarObject *self) 
{
    return pygobject_new(G_OBJECT(self->handle));
}

static PyObject *m_delete(DeepinLunarObject *self) 
{
    if (self->handle) 
        self->handle = NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *m_mark_day(DeepinLunarObject *self, PyObject *args) 
{
    gint day = -1;                                                            
                                                                                
    if (!PyArg_ParseTuple(args, "i", &day)) {                          
        ERROR("invalid arguments to mark_day");                                  
        return NULL;                                                            
    }            

    dltk_calendar_mark_day(self->handle, day);

    Py_INCREF(Py_True);                                                         
    return Py_True; 
}

static PyObject *m_clear_marks(DeepinLunarObject *self) 
{
    dltk_calendar_clear_marks(self->handle);

    Py_INCREF(Py_True);
    return Py_True;
}

static PyObject *m_set_editable(DeepinLunarObject *self, PyObject *args) 
{
    PyObject *value = NULL;                                                     
                                                                                
    if (!PyArg_ParseTuple(args, "O", &value)) {                          
        ERROR("invalid arguments to set_editable");                                                    
        return NULL;                                                        
    }                                                                           
                                                                                
    if (!PyBool_Check(value)) {                                                 
        Py_INCREF(Py_False);                                                    
        return Py_False;                                                        
    }                                                                           
                                                                                
    if (value == Py_True)    
        dltk_calendar_set_editable(self->handle, TRUE);
    else
        dltk_calendar_set_editable(self->handle, FALSE);

    Py_INCREF(Py_True);
    return Py_True;
}

static PyObject *m_get_date(DeepinLunarObject *self) 
{
    PyObject *date = NULL;
    int year, month, day;

    dltk_calendar_get_date(self->handle, &year, &month, &day);
    /* FIXME: why need +1 for month */
    date = PyTuple_Pack(3, INT(year), INT(month + 1), INT(day));

    return date;
}
