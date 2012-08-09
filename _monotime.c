#include <Python.h>
#include <time.h>

#ifdef CLOCK_MONOTONIC_RAW
# define WHICH_CLOCK  CLOCK_MONOTONIC_RAW
#else
# define WHICH_CLOCK  CLOCK_MONOTONIC
#endif


static PyObject *monotonic(PyObject *self, PyObject *args)
{
  struct timespec ts;
  
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  
#ifdef CLOCK_MONOTONIC_RAW
  /*
   * Even if defined, the running kernel might not support it, in which case
   * this will return nonzero.  Then we'll fall back to CLOCK_MONOTONIC.
   * CLOCK_MONOTONIC_RAW is slightly better, if supported, because it takes
   * no adjustments at all (not even speed adjustments or leap seconds).
   */
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) < 0)
#endif
  if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
    return PyErr_SetFromErrno(PyExc_OSError);
  
  return Py_BuildValue("d", (double)(ts.tv_sec * 1.0 + ts.tv_nsec / 1e9));
}


static PyMethodDef _monotime_methods[] = {
    { "monotonic", monotonic, METH_VARARGS,
        "Returns a strictly increasing number of seconds since\n"
        "an arbitrary start point." },
    { NULL, NULL, 0, NULL },  // sentinel
};


PyMODINIT_FUNC init_monotime(void)
{
    Py_InitModule("_monotime", _monotime_methods);
}
