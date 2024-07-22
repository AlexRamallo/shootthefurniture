#! /usr/bin/env bash

PROJECT_ROOT=`dirname \`readlink -f "$0"\``
OUTDIR=${PROJECT_ROOT}/build/linux

pushd ${PROJECT_ROOT}

# for clang LSP
ln -sf ${OUTDIR}/compile_commands.json ${PROJECT_ROOT}/compile_commands.json

set -e

function build() {
	cmake --build build/linux --target $1 --config profile -j `nproc`
}

if [ "$1" = "configure" ]; then
	cmake 											\
		-B ${OUTDIR} 								\
		-S . 										\
		-G "Ninja Multi-Config" 					\
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON 			\
		-DLY_UNITY_BUILD=OFF 						\
		-DCMAKE_C_COMPILER=/usr/bin/ccache-clang 	\
		-DCMAKE_CXX_COMPILER=/usr/bin/ccache-clang++
elif [ "$1" = "clean" ]; then
	rm -rf ${OUTDIR}
elif [ "$1" = "build" ]; then
	build Editor
elif [ "$1" = "build_and_run" ]; then
	build Editor
	${OUTDIR}/bin/profile/Editor --project-path=${PROJECT_ROOT}
fi