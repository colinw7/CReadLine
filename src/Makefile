CPP_CC = g++

AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: dirs $(LIB_DIR)/libCReadLine.a

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../lib ]; then mkdir ../lib; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

CPP_SRC = \
CReadLine.cpp \
CHistory.cpp \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

CFLAGS = \
$(CDEBUG) \
-std=c++17 \
-I$(INC_DIR) \
-I../../CFile/include \
-I../../COS/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I. \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCReadLine.a

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CFLAGS)

$(LIB_DIR)/libCReadLine.a: $(CPP_OBJS)
	$(AR) crv $(LIB_DIR)/libCReadLine.a $(CPP_OBJS)
