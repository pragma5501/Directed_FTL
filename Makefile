
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
INC_DIR = \
		./src \
		-I ./src/util \

GCCFLAGS += \
	-g \
	-O0 \
	-Wall \
	-fsanitize=address \


OBJ_SRC += \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/FTL.c \
	$(SRC_DIR)/ssd.c \
	$(SRC_DIR)/util/queue.c \

TARGET_OBJ =\
	$(patsubst %.c,%.o,$(OBJ_SRC))

all : my_dftl

my_dftl: $(TARGET_OBJ)
	@mkdir -p $(BIN_DIR)
	gcc -o    $(BIN_DIR)/$@ $^ -I$(INC_DIR)

.c.o:
	gcc -c $< -o $@ -I$(INC_DIR)

-include $(OBJS:.o=.d)

clean:
	@rm -vf $(BIN_DIR)/*
	@rm -vf $(OBJ_DIR)/*.o
	@rm -vf $(SRC_DIR)/*.o