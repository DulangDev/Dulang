frame.o: runtimeEnv/frame.c
	cc -c runtimeEnv/frame.c -o frame.o
lexer.o: compiler/lexer.c
	cc -c compiler/lexer.c -o lexer.o

RE = runtimeEnv/coro_sheduler.c runtimeEnv/execeasy.c
COMPILER = compiler/astwork.c compiler/bytecodegen.c compiler/compilerr.c compiler/process_filename.c compiler/asteasy.c compiler/asttest.c compiler/asttype_pred.c
OBJECTS = objects/object.c objects/type.c
all: frame.o lexer.o
	cc ${RE} ${COMPILER} ${OBJECTS} main.c frame.o lexer.o -std=c99 -o DulVM
