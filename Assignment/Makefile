CC = gcc
FLAGS = -pthread -lpthread
OBJ = main.o queue.o
EXEC = cq

$(EXEC) : $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(EXEC)

main.o : main.c
	$(CC) -c main.c

queue.o : queue.c queue.h
	$(CC) -c queue.c

customStructs.o : customStructs.c customStructs.h
	$(CC) -c customStructs.c

clean:
	rm -f $(EXEC) $(OBJ)