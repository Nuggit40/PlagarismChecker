all: dir.c fileList.h
	gcc dir.c -pthread -o detector

clean:
	rm detector