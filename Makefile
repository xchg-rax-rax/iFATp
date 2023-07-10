BUILD_DIR=./build
SRC_DIR=./src

always:
	mkdir -p ${BUILD_DIR}

util: ${BUILD_DIR}/util.o

${BUILD_DIR}/util.o: ${SRC_DIR}/util.c always
	gcc -c ./src/util.c -o ${BUILD_DIR}/util.o

iFATp: ${BUILD_DIR}/iFATp

${BUILD_DIR}/iFATp: ./src/main.c util
	gcc ./src/main.c ${BUILD_DIR}/util.o -o ./build/iFATp -static

clean:
	rm -rf ${BUILD_DIR}/*

test: test-12

test-12: iFATp
	${BUILD_DIR}/iFATp ./test/test.img
