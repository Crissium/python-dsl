#include <Python.h>
#include "dsl.h"

std::pair<std::string, std::vector<std::string>> to_html(const std::string &dsl, const std::string &base_url_static_files, const std::string &base_url_lookup)
{
	dom tree(dsl);
	builder b(base_url_static_files, base_url_lookup);
	std::string html = b.get_html(tree.root);
	return std::make_pair(html, b.resources_name);
}

static PyObject *to_html_wrapper(PyObject *self, PyObject *args)
{
	const char *dsl;
	const char *base_url_static_files;
	const char *base_url_lookup;

	if (!PyArg_ParseTuple(args, "sss", &dsl, &base_url_static_files, &base_url_lookup))
	{
		return NULL;
	}

	builder b(base_url_static_files, base_url_lookup);
	std::string html;

	Py_BEGIN_ALLOW_THREADS
		dom tree(dsl);
		html = b.get_html(tree.root);
	Py_END_ALLOW_THREADS

		PyObject *html_str = PyUnicode_DecodeUTF8(html.c_str(), html.length(), "strict");

	PyObject *resources_list = PyList_New(b.resources_name.size());
	for (size_t i = 0; i < b.resources_name.size(); i++)
	{
		PyObject *resource_str = PyUnicode_DecodeUTF8(b.resources_name[i].c_str(), b.resources_name[i].length(), "strict");
		PyList_SET_ITEM(resources_list, i, resource_str);
	}

	PyObject *result_tuple = PyTuple_Pack(2, html_str, resources_list);

	Py_DECREF(html_str);
	Py_DECREF(resources_list);

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
