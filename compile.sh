#!/bin/bash

CXX="g++"
CXXFLAGS="-fmodules-ts -std=c++20 -pedantic -Wall -Wextra"
MAKEMODULE="${CXX} ${CXXFLAGS} -c"
MAKESTDMODULE="${CXX} ${CXXFLAGS} -xc++-system-header"

HEADERS="cstring cstdlib cerrno string cstdint optional coroutine stdexcept vector"

for std in ${HEADERS}; do
	echo "Building standard module ${std}"
	${MAKESTDMODULE} $std
done

FILES="file_context coro_task async_read main"
OBJS=""

for file in ${FILES}; do
	echo "Building user module ${file}"
	${MAKEMODULE} ${file}.cc
	OBJS="${OBJS} ${file}.o"
done

${CXX} ${CXXFLAGS} ${OBJS} -o main -lrt

