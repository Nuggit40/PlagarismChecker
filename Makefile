all: dir.c fileList.h
	gcc dir.c -pthread -lm -o detector
test: dir.c fileList.h
	make all
	./detector testdir
clean:
	rm detector