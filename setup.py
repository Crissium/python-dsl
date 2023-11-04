#!/usr/bin/env python3

from setuptools import Extension, setup

setup(
	name='dsl',
	ext_modules=[
		Extension(
			'dsl',
			['utils.cc', 'parse.cc', 'build.cc', 'dslmodule.cc']
		)
	]
)
