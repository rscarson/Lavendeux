#include <Python.h>
#include <stdlib.h>
#include <wchar.h>

#include "parse.h"
#include "extensions.h"

FILE* extensions_log = NULL;

int extensions_init( void ) {
	PyObject *syspath, *pName;
	extensions_add_log("Starting extensions...");

	if(Py_GetPythonHome() == NULL) {
		extensions_add_log("PYTHONHOME not set! Extensions unavailable");
		return 0;
	}

	Py_SetProgramName("Lavendeux");
	Py_Initialize();

	/* Make sure it worked */
	if (!Py_IsInitialized()) {
		extensions_add_log("PYTHONHOME not set! Extensions unavailable");
		return 0;
	}

	/* Add extension dir to search path */
	pName = PyString_FromString(EXTENSIONS_PATH);
	if ( (syspath = PySys_GetObject("path")) == 0) {
		extensions_add_log("Cannot fetch python path");
		extensions_destroy();
		return 0;
	}
	if (PyList_Insert(syspath, 0, pName) || PySys_SetObject("path", syspath)) {
		extensions_add_log("Cannot add to python path");
		Py_DECREF(pName);
		extensions_destroy();
		return 0;
	}

	extensions_add_log("Extensions available...");

	Py_DECREF(pName);
	return 1;
}

void extensions_log_enable() {
	extensions_log = fopen("extensions.log", "w+");
}

void extensions_add_log(const char* entry) {
	if (extensions_log != NULL)
		fprintf(extensions_log, "%s\n", entry);
}

int extensions_homeset( void ) {
	return (Py_GetPythonHome() != NULL);
}

int extensions_available( void ) {
	return Py_IsInitialized();
}

void extensions_destroy( void ) {
	fclose(extensions_log);
	Py_Finalize();
}

PyObject* extensions_module(const char* name, const char* function) {
	PyObject *pName, *pModule, *pFunc;
	extensions_add_log("Loading an extension");

	/* Can we? */
	if (!Py_IsInitialized()) {
		extensions_add_log("Extensions not available.");
		return NULL;
	}

	/* Try to get the module */
	pName = PyString_FromString(name);
	pModule = PyImport_Import(pName);

	/* Make sure it worked */
	if (pModule == NULL) {
		extensions_add_log("Cannot load requested extension module");
		return NULL;
	}

	/* Get function from module */
	pFunc = PyObject_GetAttrString(pModule, EXTENSIONS_FUNCTION);
	if (!pFunc || !PyCallable_Check(pFunc)) {
		//Py_DECREF(pModule);
		extensions_add_log("Cannot load module function");
		return NULL;
	}

	return pFunc;
}

int extensions_decorate(const char* name, value v,  wchar_t* decorated) {
	PyObject *pFunc, *pArgs, *pValue;

	/* Get function */
	pFunc = extensions_module(name, EXTENSIONS_DECORATOR);
	if (!pFunc) return FAILURE_BAD_EXTENSION;

	/* Prepare argument */
	pArgs = PyTuple_New(1);
	pValue = (v.type == VALUE_INT) ? PyLong_FromLongLong(v.iv) : PyFloat_FromDouble(v.fv);
	if (!pValue) {
		extensions_add_log("Error while preparing an argument");
		return FAILURE_BAD_EXTENSION;
	}
	PyTuple_SetItem(pArgs, 0, pValue);

	/* Call function */
	Py_INCREF(pFunc);
	Py_INCREF(pArgs);
	pValue = PyObject_CallObject(pFunc, pArgs);

	if (pValue != NULL) {
		pValue = PyObject_Str(pValue);
		if (pValue == NULL) {
			extensions_add_log("Argument cannot be converted to string");
			return FAILURE_BAD_EXTENSION;
		}

		if (mbstowcs(decorated, PyString_AsString(pValue), EXPRESSION_MAX_LEN) == EXPRESSION_MAX_LEN)
			decorated[EXPRESSION_MAX_LEN-1] = L'\0';
		return NO_FAILURE;
	} else {
		extensions_add_log("Error while executing extension");
		return FAILURE_BAD_EXTENSION;
	}
}

int extensions_call(const char* name, value args[], int n_args, value* v) {
	int i;
	int_value_t iv;
	float_value_t fv;
	unsigned long err;
	PyObject *pFunc, *pArgs, *pList, *pValue, *pResultType, *pResult;

	/* Get function */
	pFunc = extensions_module(name, EXTENSIONS_DECORATOR);
	if (!pFunc) return FAILURE_BAD_EXTENSION;

	/* Prepare arguments */
	pArgs = PyTuple_New(1);
	pList = PyList_New(n_args);
	for (i=0; i<n_args; i++) {
		pValue = (args[i].type == VALUE_INT) ? PyLong_FromLongLong(args[i].iv) : PyFloat_FromDouble(args[i].fv);
		if (!pValue) {
			extensions_add_log("Error while preparing an argument");
			return FAILURE_BAD_EXTENSION;
		}
		PyList_SetItem(pList, i, pValue);
	}
	PyTuple_SetItem(pArgs, 0, pList);

	/* Call function */
	Py_INCREF(pFunc);
	Py_INCREF(pArgs);
	pValue = PyObject_CallObject(pFunc, pArgs);

	/* Remove a few references */
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
				v->type = VALUE_FLOAT;
				v->iv = fv;
			break;

			case VALUE_INT:
				iv = PyLong_AsLongLong(pResult);
				v->type = VALUE_INT;
				v->iv = iv;
			break;

			default:
				extensions_add_log("Bad type returned");
				return FAILURE_BAD_EXTENSION;
		}
	} else {
		PyErr_Print();
		extensions_add_log("Error while executing extension");
		return FAILURE_BAD_EXTENSION;
	}

	extensions_add_log("Run complete");
	return NO_FAILURE;
}