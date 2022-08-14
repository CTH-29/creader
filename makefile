SRC_LIST = main.c creader.c

SRC_PERFIX = src
INC_PERFIX = inc
OBJ_PERFIX = bin
BIN_PERFIX = bin
LIB_PERFIX = lib

TARGET = creader

vpath %.c $(SRC_PERFIX)
vpath %.h $(INC_PERFIX)

CC = gcc
CXX = g++

SRC = $(addprefix $(SRC_PERFIX)/,$(SRC_LIST))
INC = $(wildcard $(INC_PERFIX)/*.h)
OBJ = $(patsubst %.c,$(OBJ_PERFIX)/%.o,$(SRC_LIST))

CBASEFLAGS = $(addprefix -I,$(INC_PERFIX))

CFLAGS = $(CBASEFLAGS)

$(TARGET): $(OBJ)
	@ mkdir -p $(BIN_PERFIX)
	$(CXX) -o $(BIN_PERFIX)/$@ $^

$(OBJ_PERFIX)/%.o : $(SRC_PERFIX)/%.c $(INC)
	@ mkdir -p $(OBJ_PERFIX)
	$(CXX) -c $< -o $@ $(CFLAGS)

.PHONY : clean cleanobj run
cleanobj :
	rm -r $(OBJ_PERFIX)/*.o

clean :
	rm -r $(OBJ_PERFIX)/*.o
	rm -r $(BIN_PERFIX)/$(TARGET)

run : $(TARGET)
	@ ./$(BIN_PERFIX)/$(TARGET)

