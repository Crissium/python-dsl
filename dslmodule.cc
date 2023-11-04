#include <Python.h>
#include "dsl.h"

std::pair<std::string, std::vector<std::string>> to_html(const std::string &dsl, const std::string &name_dict)
{
	dom tree(dsl);
	builder b(name_dict);
	b.build(tree.root);
	return std::make_pair(b.html, b.resources_name);
}

static PyObject *to_html_wrapper(PyObject *self, PyObject *args)
{
	const char *dsl;
	const char *name_dict;

	if (!PyArg_ParseTuple(args, "ss", &dsl, &name_dict))
	{
		return NULL;
	}

	std::pair<std::string, std::vector<std::string>> result;

	Py_BEGIN_ALLOW_THREADS
		result = to_html(dsl, name_dict);
	Py_END_ALLOW_THREADS

		PyObject *html_str = PyUnicode_DecodeUTF8(result.first.c_str(), result.first.length(), "strict");

	PyObject *resources_list = PyList_New(result.second.size());
	for (size_t i = 0; i < result.second.size(); i++)
	{
		PyObject *resource_str = PyUnicode_DecodeUTF8(result.second[i].c_str(), result.second[i].length(), "strict");
		PyList_SET_ITEM(resources_list, i, resource_str);
	}

	PyObject *result_tuple = PyTuple_Pack(2, html_str, resources_list);

	Py_XDECREF(html_str);
	Py_XDECREF(resources_list);

	return result_tuple;
}

static PyMethodDef DSLMethods[] = {
	{"to_html", to_html_wrapper, METH_VARARGS, "Convert DSL to HTML"},
	{NULL, NULL, 0, NULL}};

static struct PyModuleDef dslmodule = {
	PyModuleDef_HEAD_INIT,
	"dsl",
	NULL,
	-1,
	DSLMethods};

PyMODINIT_FUNC PyInit_dsl(void)
{
	return PyModule_Create(&dslmodule);
}
