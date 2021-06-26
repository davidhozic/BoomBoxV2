CC := gcc


CCPP := g++

CPP_FLAGS := 
C_FLAGS := 
GLOBAL_H :=-include stdint.h\

FLD_INC := -I .//\
-I ./List_EXAMPLE/\
-I ./.vscode/\

SRC_DIRS := .//\
./List_EXAMPLE/\
./.vscode/\

CPP_SRC := ./List_EXAMPLE/test.cpp\

C_SRC := 
HEADER_SRC := .//llist_funct.hh\
.//list_settings.hh\
.//linked_list.hh\

O_FILES := Build/List_EXAMPLE/l_example.o\
##############
OUT_FILE := Test.bin
OUT_DIR := Build/
##############################
clean:
	rm -rf $(OUT_DIR)

CR_DIRS:
	for dir in $(SRC_DIRS);do mkdir -p $(OUT_DIR)$$dir; done

all: CR_DIRS $(O_FILES)
	$(CCPP) $(O_FILES) $(LINK_COMPILER_FLAGS) $(GLOBAL_H) $(FLD_INC) -o $(OUT_DIR)$(OUT_FILE)

Build/List_EXAMPLE/l_example.o : $(HEADER_SRC) ./List_EXAMPLE/l_example.cpp
	echo "Compiling "./List_EXAMPLE/l_example.cpp
	$(CCPP) -c ./List_EXAMPLE/l_example.cpp $(CPP_FLAGS) $(GLOBAL_H) $(FLD_INC) -o $@



.SILENT: