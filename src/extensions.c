#ifdef EXTENSIONS_INCLUDED
	#include <Python.h>
	#include <stdlib.h>
	#include <wchar.h>

	#include "parse.h"
	#include "extensions.h"
	#include "interface.h"

	int _extensions_available;

	void extensions_init( void ) {
		PyObject *syspath, *pName, *pModule, *stdOut;
		char* path;
		_extensions_available = 0;
		printf("Attempting to load extensions\n");

		Py_SetProgramName("Lavendeux");
		Py_NoSiteFlag=1;
		Py_DontWriteBytecodeFlag = 1;
		Py_SetPythonHome("./");
		Py_Initialize();

		/* Make sure it worked */
		if (!Py_IsInitialized()) {
			printf("Could not load python\n");
			return;
		}

		/* Add dirs to search path */
		if ( (syspath = PySys_GetObject("path")) == 0) {
			printf("Could not fetch python path\n");
			extensions_destroy();
		
			return;
		}

		path = self_path();
		strcat(path, &EXTENSIONS_PATH[2]);
		PyList_Insert(syspath, 0, PyString_FromString(path));
		free(path);

		path = self_path();
		strcat(path, &EXTENSIONS_LIB_PATH[2]);
		PyList_Insert(syspath, 0, PyString_FromString(path));
		free(path);

		path = self_path();
		strcat(path, "python27.zip");
		PyList_Insert(syspath, 0, PyString_FromString(path));
		free(path);

		path = self_path();
		strcat(path, "python27.zip/site-packages");
		PyList_Insert(syspath, 0, PyString_FromString(path));
		free(path);

		PyList_Insert(syspath, 0, PyString_FromString(EXTENSIONS_PATH));
		PyList_Insert(syspath, 0, PyString_FromString(EXTENSIONS_LIB_PATH));
		PyList_Insert(syspath, 0, PyString_FromString("python27.zip"));
		PyList_Insert(syspath, 0, PyString_FromString("python27.zip/site-packages"));
		PySys_SetObject("path", syspath);

		/* Test libraries */
		pModule = PyImport_ImportModule("_socket");
		if (pModule == NULL) {
			printf("Could not find python dynamic libraries!\n");
			return;
		}
		Py_DECREF(pModule);

		/* Reopen console output */
		pModule = PyImport_ImportModule("io");
		if (pModule == NULL) {
			/* Cannot load standard library! */
			printf("Could not find standard library! python27.zip missing or corrupted.\n");
			return;
		}
		stdOut = PyObject_CallMethod(pModule, "open", "ssi", "CONOUT$", "wb", 0);
		Py_DECREF(pModule);

		/* Redirect errors */
		pModule = PyImport_ImportModule("sys");
		if (pModule == NULL) {
			/* Cannot load standard library! */
			printf("Could not find standard library! python27.zip missing or corrupted.\n");
			return;
		}
		PyObject_SetAttrString(pModule, "stderr", stdOut);
		PyObject_SetAttrString(pModule, "stdout", stdOut);
		Py_DECREF(pModule);


		printf("Extensions ready\n");

			
		_extensions_available = 1;
		return;
	}

	int extensions_homeset( void ) {
		return (Py_GetPythonHome() != NULL);
	}

	int extensions_available( void ) {
		return Py_IsInitialized() && _extensions_available;
	}

	void extensions_destroy( void ) {
		Py_Finalize();
	}

	PyObject* extensions_module(const char* name, const char* function) {
		PyObject *pModule, *pFunc, *pErr;
		printf("Loading an extension module: %s; %s function\n", name, function);

		/* Can we? */
		if (!Py_IsInitialized()) {
			printf("Extensions not available\n");
			return NULL;
		}

		/* Try to get the module */
		pModule = PyImport_ImportModule(name);
		if (pModule == NULL) {
			printf("Cannot load requested extension module\n");
			if (PyErr_Occurred()) {
				printf("Error loading extension :\n");
				PyErr_Print();
			}

			return NULL;
		}

		/* Call reload */
		pModule = PyImport_ReloadModule(pModule);
		if (pModule == NULL) {
			printf("Cannot load requested extension module\n");
			if (PyErr_Occurred()) {
				printf("Error loading extension :\n");
				PyErr_Print();
			}

			return NULL;
		}

		/* Get function from module */
		pFunc = PyObject_GetAttrString(pModule, function);
		if (!pFunc || !PyCallable_Check(pFunc)) {
			printf("Cannot load module function\n");
			Py_DECREF(pModule);
			return NULL;
		}

		Py_DECREF(pModule);
		printf("Successfully loaded module\n");
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
		if (v.type == VALUE_INT)
			pValue = PyLong_FromLongLong(v.iv);
		else if (v.type == VALUE_FLOAT)
			pValue = PyFloat_FromDouble(v.fv);
		else if (v.type == VALUE_STRING)
			pValue = PyUnicode_FromWideChar(v.sv, wcslen(v.sv));

		if (!pValue) {
			printf("Error while preparing an argument\n");
			Py_DECREF(pFunc);
			return FAILURE_BAD_EXTENSION;
		}
		PyTuple_SetItem(pArgs, 0, pValue);

		/* Call function */
		pValue = PyObject_CallObject(pFunc, pArgs);
		//Py_DECREF(pArgs);
		Py_DECREF(pFunc);

		if (pValue != NULL) {
			pValue = PyObject_Unicode(pValue);
			if (pValue == NULL) {
				printf("Argument cannot be converted to string\n");
				Py_DECREF(pValue);
				return FAILURE_BAD_EXTENSION;
			}

			decorated[ PyUnicode_AsWideChar((PyUnicodeObject*) pValue, decorated, EXPRESSION_MAX_LEN-1) ] = L'\0';

			Py_DECREF(pValue);
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
		char *result;
		PyObject *pFunc, *pArgs, *pList, *pValue, *pResultType, *pResult, *pUnicode;
		printf("Trying to run %s as an extension function\n", name);

		/* Help? */
		if (strcmp(name, EXTENSIONS_HELP) == 0 && n_args==1 && args[0].type == VALUE_STRING) {
			result = malloc(sizeof(char) * (wcslen(args[0].sv) + 1));
			if (result == NULL)
				return FAILURE_ALLOCATION;
			result[ wcstombs(result, args[0].sv, wcslen(args[0].sv)) ] = L'\0';
			
			i = extensions_help(result, v);
			free(result);
			return i;
		}

		/* Get function */
		pFunc = extensions_module(name, EXTENSIONS_FUNCTION);
		if (!pFunc) return FAILURE_BAD_EXTENSION;

		/* Prepare arguments */
		pArgs = PyTuple_New(1);
		pList = PyList_New(n_args);
		pValue = NULL;
		for (i=0; i<n_args; i++) {
			if (args[i].type == VALUE_INT)
				pValue = PyLong_FromLongLong(args[i].iv);
			else if (args[i].type == VALUE_FLOAT)
				pValue = PyFloat_FromDouble(args[i].fv);
			else if (args[i].type == VALUE_STRING)
				pValue = PyUnicode_FromWideChar(args[i].sv, wcslen(args[i].sv));

			if (!pValue) {
				printf("Error while preparing an argument\n");
				Py_DECREF(pFunc);
				return FAILURE_BAD_EXTENSION;
			}
			PyList_SetItem(pList, i, pValue);
		}
		PyTuple_SetItem(pArgs, 0, pList);

		/* Call function */
		pValue = PyObject_CallObject(pFunc, pArgs);
		printf("Function call complete.\n");
		//Py_DECREF(pArgs);
		Py_DECREF(pFunc);

		/* Remove a few references */
		/* Use value */
		if (pValue != NULL) {
			if (!PyTuple_Check(pValue) || PyTuple_Size(pValue) != 2) {
				Py_DECREF(pValue);
				printf("Bad type returned\n");
				return FAILURE_BAD_EXTENSION;
			}
			pResultType = PyTuple_GetItem(pValue, 0);
			pResult = PyTuple_GetItem(pValue, 1);

			switch (PyLong_AsLong(pResultType)) {
				case VALUE_ERROR:
					err = PyLong_AsUnsignedLong(pResult);

					Py_DECREF(pValue);
					return (err < N_FAILURES) ? err : FAILURE_BAD_EXTENSION;
				break;

				case VALUE_FLOAT:
					fv = PyFloat_AsDouble(pResult);
					v->type = VALUE_FLOAT;
					v->fv = fv;
				break;

				case VALUE_INT:
					iv = PyLong_AsLongLong(pResult);
					v->type = VALUE_INT;
					v->iv = iv;
				break;

				case VALUE_STRING:
					if (PyUnicode_Check(pResult)) {
						pUnicode = (PyObject*) PyUnicode_FromObject(pResult);
						v->sv[ PyUnicode_AsWideChar( (PyUnicodeObject*) pUnicode, v->sv, EXPRESSION_MAX_LEN-1) ] = L'\0';
						v->type = VALUE_STRING;
					} else {
						v->type = VALUE_STRING;
						result = PyString_AsString(pResult);
						v->sv[ mbstowcs(v->sv, result, EXPRESSION_MAX_LEN-1) ] = L'\0';
						free(result);
					}


				break;

				default:
					printf("Bad type returned\n");

					Py_DECREF(pValue);
					return FAILURE_BAD_EXTENSION;
			}

			Py_DECREF(pValue);
		} else {
			printf("Error while executing extension\n");
			PyErr_Print();
			return FAILURE_BAD_EXTENSION;
		}

		printf("Run complete\n");
		return NO_FAILURE;
	}

	int extensions_help(const char *name, value *v) {
		PyObject *pFunc, *pValue, *pUnicode;

		/* Get function */
		pFunc = extensions_module(name, EXTENSIONS_HELP);
		if (!pFunc) return FAILURE_BAD_EXTENSION;

		pValue = PyObject_CallObject(pFunc, NULL);

		pUnicode = (PyObject*) PyUnicode_FromObject(pValue);
		v->sv[ PyUnicode_AsWideChar( (PyUnicodeObject*) pUnicode, v->sv, EXPRESSION_MAX_LEN-1) ] = L'\0';
		v->type = VALUE_STRING;

		Py_DECREF(pUnicode);
		Py_DECREF(pValue);

		return NO_FAILURE;
	}
#endif
