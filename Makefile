BUILD_DIR=./build
SRC_DIR=./src

always:iFATp

make_build_dir:
	mkdir -p ${BUILD_DIR}

util: ${BUILD_DIR}/util.o

${BUILD_DIR}/util.o: ${SRC_DIR}/util.c make_build_dir 
	gcc -c ./src/util.c -o ${BUILD_DIR}/util.o

fat: ${BUILD_DIR}/fat.o

${BUILD_DIR}/fat.o: ${SRC_DIR}/fat.c make_build_dir
	gcc -c ./src/fat.c -o ${BUILD_DIR}/fat.o

iFATp: ${BUILD_DIR}/iFATp

${BUILD_DIR}/iFATp: ./src/main.c util fat
	gcc ./src/main.c ${BUILD_DIR}/util.o ${BUILD_DIR}/fat.o -o ./build/iFATp -static

clean:
	rm -rf ${BUILD_DIR}/*

test_all: test_fat12 test_fat16 test_fat32

test_fat12: iFATp
	${BUILD_DIR}/iFATp ./test/test_data/fat12.img
test_fat16: iFATp
	${BUILD_DIR}/iFATp ./test/test_data/fat16.img
test_fat32: iFATp
	${BUILD_DIR}/iFATp ./test/test_data/fat32.img
