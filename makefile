CC = gcc

SRC = http.c 

OBJ = http.o 

EXE = http 

all: $(EXE) 

# compile the source to the object
$(OBJ): $(SRC) http.h
	$(CC) -c $(SRC)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE)

clean:
	rm -f $(OBJ) $(EXE)