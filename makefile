build: main.o main.h
	gcc -o controller $^

.PHONY: clean

clean:
  rm -f *.o