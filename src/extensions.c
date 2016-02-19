#include <Python.h>
#include <stdlib.h>
#include <wchar.h>

#include "parse.h"
#include "extensions.h"

int extensions_init( void ) {
	PyObject *syspath, *pName;

	if(Py_GetPythonHome() == NULL)
		return 0;

	Py_SetProgramName("Lavendeux");
	Py_Initialize();

	/* Make sure it worked */
	if (!Py_IsInitialized())
		return 0;

	/* Add extension dir to search path */
	pName = PyString_FromString(EXTENSIONS_PATH);
	if ( (syspath = PySys_GetObject("path")) == 0) {
		extensions_destroy();
		return 0;
	}
	if (PyList_Insert(syspath, 0, pName) || PySys_SetObject("path", syspath)) {
		Py_DECREF(pName);
		extensions_destroy();
		return 0;
	}

	Py_DECREF(pName);
	return 1;
}

int extensions_homeset( void ) {
	return (Py_GetPythonHome() != NULL);
}

int extensions_available( void ) {
	return Py_IsInitialized();
}

void extensions_destroy( void ) {
	Py_Finalize();
}

int extensions_call(const char* name, value args[], int n_args, value* v) {
	int i;
	int_value_t iv;
	float_value_t fv;
	unsigned long err;
	PyObject *pName, *pModule, *pFunc, *pArgs, *pList, *pValue, *pResultType, *pResult;

	/* Can we? */
	if (!Py_IsInitialized())
		return FAILURE_BAD_EXTENSION;

	/* Try to get the module */
	pName = PyString_FromString(name);
	pModule = PyImport_Import(pName);

	/* Make sure it worked */
	if (pModule == NULL) {
		return FAILURE_BAD_EXTENSION;
	}

	/* Get function from module */
	pFunc = PyObject_GetAttrString(pModule, EXTENSIONS_FUNCTION);
	if (!pFunc || !PyCallable_Check(pFunc)) {
		//Py_DECREF(pModule);
		return FAILURE_BAD_EXTENSION;
	}

	/* Prepare arguments */
	pArgs = PyTuple_New(1);
	pList = PyList_New(n_args);
	for (i=0; i<n_args; i++) {
		pValue = (args[i].type == VALUE_INT) ? PyLong_FromLongLong(args[i].iv) : PyFloat_FromDouble(args[i].fv);
		if (!pValue) {
			Py_DECREF(pModule);
			Py_DECREF(pFunc);
			Py_DECREF(pArgs);
			return FAILURE_BAD_EXTENSION;
		}
		PyList_SetItem(pList, i, pValue);
	}
	PyTuple_SetItem(pArgs, 0, pList);


	/* Call function */
	Py_INCREF(pFunc);
	Py_INCREF(pArgs);
	pValue = PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pArgs);

	/* Remove a few references */
	Py_DECREF(pFunc);
	Py_DECREF(pModule);
	/* Use value */
	if (pValue != NULL && PyTuple_Check(pValue) && PyTuple_Size(pValue) == 2) {
		pResultType = PyTuple_GetItem(pValue, 0);
		pResult = PyTuple_GetItem(pValue, 1);

		switch (PyLong_AsLong(pResultType)) {
			case VALUE_ERROR:
				err = PyLong_AsUnsignedLong(pResult);
				return (err < N_FAILURES) ? err : FAILURE_BAD_EXTENSION;
			break;

			case VALUE_FLOAT:
				fv = PyFloat_AsDouble(pResult);
				Py_DECREF(pResult);
				v->type = VALUE_FLOAT;
				v->iv = fv;
			break;

			case VALUE_INT:
				iv = PyLong_AsLongLong(pResult);
				Py_DECREF(pResult);
				v->type = VALUE_INT;
				v->iv = iv;
			break;

			default:
				Py_DECREF(pValue);
				return FAILURE_BAD_EXTENSION;
		}
	} else {
		Py_DECREF(pValue);
		return FAILURE_BAD_EXTENSION;
	}

	Py_DECREF(pValue);
	return NO_FAILURE;
}