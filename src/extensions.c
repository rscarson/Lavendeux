#include <Python.h>
#include <stdlib.h>
#include <wchar.h>

#include "parse.h"
#include "extensions.h"

int extensions_init( void ) {
	PyObject *syspath, *pName;

	Py_SetProgramName("Lavendeux");
	Py_NoSiteFlag=1;
	Py_SetPythonHome("./");
	Py_Initialize();

	/* Make sure it worked */
	if (!Py_IsInitialized()) {
		printf("Could not load python\n");
		return 0;
	}

	/* Add extension dir to search path */
	pName = PyString_FromString(EXTENSIONS_PATH);
	if ( (syspath = PySys_GetObject("path")) == 0) {
		printf("Could not fetch python path\n");
		extensions_destroy();
		return 0;
	}
	if (PyList_Insert(syspath, 0, pName) || PySys_SetObject("path", syspath)) {
		printf("Could not append to python path\n");
		Py_DECREF(pName);
		extensions_destroy();
		return 0;
	}

	printf("Extensions ready\n");

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

PyObject* extensions_module(const char* name, const char* function) {
	PyObject *pName, *pModule, *pFunc;
	printf("Loading an extension module: %s; %s function\n", name, function);

	/* Can we? */
	if (!Py_IsInitialized()) {
		printf("Extensions not available\n");
		return NULL;
	}

	/* Try to get the module */
	pName = PyString_FromString(name);
	pModule = PyImport_Import(pName);

	/* Make sure it worked */
	if (pModule == NULL) {
		printf("Cannot load requested extension module\n");
		return NULL;
	}

	/* Get function from module */
	pFunc = PyObject_GetAttrString(pModule, EXTENSIONS_FUNCTION);
	if (!pFunc || !PyCallable_Check(pFunc)) {
		//Py_DECREF(pModule);
		printf("Cannot load module function\n");
		return NULL;
	}

	return pFunc;
}

int extensions_decorate(const char* name, value v,  wchar_t* decorated) {
	PyObject *pFunc, *pArgs, *pValue;
	printf("Running %s as a decorator\n", name);

	/* Get function */
	pFunc = extensions_module(name, EXTENSIONS_DECORATOR);
	if (!pFunc) return FAILURE_BAD_EXTENSION;

	/* Prepare argument */
	pArgs = PyTuple_New(1);
	pValue = (v.type == VALUE_INT) ? PyLong_FromLongLong(v.iv) : PyFloat_FromDouble(v.fv);
	if (!pValue) {
		printf("Error while preparing an argument\n");
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
			printf("Argument cannot be converted to string\n");
			return FAILURE_BAD_EXTENSION;
		}

		if (mbstowcs(decorated, PyString_AsString(pValue), EXPRESSION_MAX_LEN) == EXPRESSION_MAX_LEN)
			decorated[EXPRESSION_MAX_LEN-1] = L'\0';
		return NO_FAILURE;
	} else {
		printf("Error while executing extension\n");
		return FAILURE_BAD_EXTENSION;
	}
}

int extensions_call(const char* name, value args[], int n_args, value* v) {
	int i;
	int_value_t iv;
	float_value_t fv;
	unsigned long err;
	PyObject *pFunc, *pArgs, *pList, *pValue, *pResultType, *pResult;
	printf("Trying to run %s as an extention function\n", name);

	/* Get function */
	pFunc = extensions_module(name, EXTENSIONS_FUNCTION);
	if (!pFunc) return FAILURE_BAD_EXTENSION;

	/* Prepare arguments */
	pArgs = PyTuple_New(1);
	pList = PyList_New(n_args);
	for (i=0; i<n_args; i++) {
		pValue = (args[i].type == VALUE_INT) ? PyLong_FromLongLong(args[i].iv) : PyFloat_FromDouble(args[i].fv);
		if (!pValue) {
			printf("Error while preparing an argument\n");
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
				printf("Bad type returned\n");
				return FAILURE_BAD_EXTENSION;
		}
	} else {
		printf("Error while executing extension");
		PyErr_Print();
		return FAILURE_BAD_EXTENSION;
	}

	printf("Run complete\n");
	return NO_FAILURE;
}