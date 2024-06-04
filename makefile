CC=gcc
CFLAGS=-I.
LIBS=-lssl -lcrypto

# Объектные файлы
OBJS=main.o func.o

# Основная цель
all : controller

# Ссылка конечного исполняемого файла
controller : $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

# Правило для main.o
main.o : main.cc func.h
	$(CC) -c $< $(CFLAGS)

# Правило для func.o
func.o : func.cc func.h
	$(CC) -c $< $(CFLAGS)

# Правило для очистки
.PHONY: clean
clean:
	rm -f *.o controller
