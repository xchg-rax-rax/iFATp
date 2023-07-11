BUILD_DIR=./build
SRC_DIR=./src

always:
	mkdir -p ${BUILD_DIR}

util: ${BUILD_DIR}/util.o

${BUILD_DIR}/util.o: ${SRC_DIR}/util.c always
	gcc -c ./src/util.c -o ${BUILD_DIR}/util.o

fat: ${BUILD_DIR}/fat.o

${BUILD_DIR}/fat.o: ${SRC_DIR}/fat.c always
	gcc -c ./src/fat.c -o ${BUILD_DIR}/fat.o

iFATp: ${BUILD_DIR}/iFATp

${BUILD_DIR}/iFATp: ./src/main.c util fat
	gcc ./src/main.c ${BUILD_DIR}/util.o ${BUILD_DIR}/fat.o -o ./build/iFATp -static

clean:
	rm -rf ${BUILD_DIR}/*

test: test-12

test-12: iFATp
	${BUILD_DIR}/iFATp ./test/test.img
