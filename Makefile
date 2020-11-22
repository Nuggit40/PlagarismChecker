all: dir.c fileList.h
	gcc dir.c -pthread -o detector
test: dir.c fileList.h
	make all
	./detector testdir
clean:
	rm detector