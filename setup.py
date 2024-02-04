#!/usr/bin/env python3

from setuptools import Extension, setup

setup(
	name='dsl',
	ext_modules=[
		Extension(
			'dsl',
			['src/utils.cc', 'src/parse.cc', 'src/build.cc', 'src/dslmodule.cc'],
			extra_compile_args=['-std=c++11']
		)
	]
)
