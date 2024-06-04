CC=gcc
CFLAGS=-I.
LIBS=-lssl -lcrypto

# ��������� �����
OBJS=main.o func.o

# �������� ����
all : controller

# ������ ��������� ������������ �����
controller : $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

# ������� ��� main.o
main.o : main.cc func.h
	$(CC) -c $< $(CFLAGS)

# ������� ��� func.o
func.o : func.cc func.h
	$(CC) -c $< $(CFLAGS)

# ������� ��� �������
.PHONY: clean
clean:
	rm -f *.o controller
