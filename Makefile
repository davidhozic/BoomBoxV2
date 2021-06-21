C_COMPILER := avr-gcc
CPP_COMPILER := avr-g++
C_FLAGS := -std=c99\
-g\

CPP_FLAGS := -std=c++17\
-g\

COMMON_FLAGS := -D F_CPU=16000000\
-mmcu=atmega2560\

GLOBAL_INC := -include stdint.h\
-include settings.hh\

OUTPUT_DIR := Build
OUTPUT_NAME := main

MCU_PART = m2560
PROGRAMMER = usbasp-clone

FOLDER_INCLUDE:= -I ZZ/code/\
-I ZZ/code/audio-visual\
-I ZZ/code/audio-visual/includes\
-I ZZ/code/common\
-I ZZ/code/common/inc\
-I ZZ/code/Events\
-I ZZ/code/Events/inc\
-I ZZ/code/freeRTOS\
-I ZZ/code/freeRTOS/include\
-I ZZ/code/libs\
-I ZZ/code/libs/castimer\
-I ZZ/code/libs/EEPROM\
-I ZZ/code/libs/input\
-I ZZ/code/libs/linked_list\
-I ZZ/code/libs/outputs_inputs\
-I ZZ/code/Power\
-I ZZ/code/User_UI\
-I ZZ/code/User_UI/inc\

CPP := ZZ/code//bootloader.cpp\
ZZ/code/audio-visual/audio_system_functions.cpp\
ZZ/code/audio-visual/avs_task_main.cpp\
ZZ/code/audio-visual/meritve.cpp\
ZZ/code/common/global.cpp\
ZZ/code/Events/events.cpp\
ZZ/code/libs/castimer/castimer.cpp\
ZZ/code/libs/EEPROM/eeprom.cpp\
ZZ/code/libs/input/input.cpp\
ZZ/code/libs/outputs_inputs/outputs_inputs.cpp\
ZZ/code/Power/charging.cpp\
ZZ/code/Power/Power.cpp\
ZZ/code/User_UI/user_ui.cpp\

C := ZZ/code/freeRTOS/croutine.c\
ZZ/code/freeRTOS/event_groups.c\
ZZ/code/freeRTOS/heap_4.c\
ZZ/code/freeRTOS/list.c\
ZZ/code/freeRTOS/port.c\
ZZ/code/freeRTOS/queue.c\
ZZ/code/freeRTOS/stream_buffer.c\
ZZ/code/freeRTOS/tasks.c\
ZZ/code/freeRTOS/timers.c\

SRC_DIR := ZZ/code/ ZZ/code/audio-visual ZZ/code/audio-visual/includes ZZ/code/common ZZ/code/common/inc ZZ/code/Events ZZ/code/Events/inc ZZ/code/freeRTOS ZZ/code/freeRTOS/include ZZ/code/libs ZZ/code/libs/castimer ZZ/code/libs/EEPROM ZZ/code/libs/input ZZ/code/libs/linked_list ZZ/code/libs/outputs_inputs ZZ/code/Power ZZ/code/User_UI ZZ/code/User_UI/inc 


O := $(OUTPUT_DIR)/ZZ/code/freeRTOS/croutine.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/event_groups.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/heap_4.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/list.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/port.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/queue.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/stream_buffer.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/tasks.o\
$(OUTPUT_DIR)/ZZ/code/freeRTOS/timers.o\
$(OUTPUT_DIR)/ZZ/code//bootloader.o\
$(OUTPUT_DIR)/ZZ/code/audio-visual/audio_system_functions.o\
$(OUTPUT_DIR)/ZZ/code/audio-visual/avs_task_main.o\
$(OUTPUT_DIR)/ZZ/code/audio-visual/meritve.o\
$(OUTPUT_DIR)/ZZ/code/common/global.o\
$(OUTPUT_DIR)/ZZ/code/Events/events.o\
$(OUTPUT_DIR)/ZZ/code/libs/castimer/castimer.o\
$(OUTPUT_DIR)/ZZ/code/libs/EEPROM/eeprom.o\
$(OUTPUT_DIR)/ZZ/code/libs/input/input.o\
$(OUTPUT_DIR)/ZZ/code/libs/outputs_inputs/outputs_inputs.o\
$(OUTPUT_DIR)/ZZ/code/Power/charging.o\
$(OUTPUT_DIR)/ZZ/code/Power/Power.o\
$(OUTPUT_DIR)/ZZ/code/User_UI/user_ui.o\

all: clean compile flash


clean:
	echo "------------------------------------"
	echo " STEP[]: Cleaning Folder $(OUTPUT_DIR)     "
	echo "------------------------------------"
	rm -rf $(OUTPUT_DIR)
	sleep 2



compile: echo_compile mkdir $(O)
	echo "------------------------------------"
	echo " STEP[]: LINKING INTO ELF           "
	echo "------------------------------------"
	sleep 2
	$(CPP_COMPILER) $(O) $(COMMON_FLAGS) -o $(OUTPUT_DIR)/$(OUTPUT_NAME).elf
	echo "------------------------------------"
	echo " STEP[]: CONVERTING INTO HEX        "
	echo "------------------------------------"
	sleep 2
	avr-objcopy -j .text -j .data -O ihex $(OUTPUT_DIR)/$(OUTPUT_NAME).elf $(OUTPUT_DIR)/$(OUTPUT_NAME).hex

flash : 
	echo "------------------------------------"
	echo " STEP[]: FLASHING TO $(MCU_PART)    "
	echo "------------------------------------"
	sleep 2
	avrdude -p $(MCU_PART) -c $(PROGRAMMER) -U flash:w:"$(OUTPUT_DIR)/$(OUTPUT_NAME).hex"

mkdir : 
	for dir in $(SRC_DIR); do mkdir -p $(OUTPUT_DIR)/$$dir; done

echo_compile : 
	echo "------------------------------------"
	echo " STEP[]: COMPILING SOURCE FILES     "
	echo "------------------------------------"
	sleep 2

$(OUTPUT_DIR)/ZZ/code//bootloader.o : ZZ/code//bootloader.cpp ZZ/code/Events/inc/events.hh 
	echo "Compiling C++ source file:bootloader.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code//bootloader.cpp

$(OUTPUT_DIR)/ZZ/code/audio-visual/audio_system_functions.o : ZZ/code/audio-visual/audio_system_functions.cpp ZZ/code/common/inc/global.hh ZZ/code/libs/EEPROM/EEPROM.hh ZZ/code/audio-visual/includes/audio.hh ZZ/code/libs/outputs_inputs/outputs_inputs.hh 
	echo "Compiling C++ source file:audio_system_functions.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/audio-visual/audio_system_functions.cpp

$(OUTPUT_DIR)/ZZ/code/audio-visual/avs_task_main.o : ZZ/code/audio-visual/avs_task_main.cpp ZZ/code/libs/EEPROM/EEPROM.hh ZZ/code/audio-visual/includes/audio.hh ZZ/code/libs/castimer/castimer.hh ZZ/code/libs/input/input.hh ZZ/code/libs/outputs_inputs/outputs_inputs.hh 
	echo "Compiling C++ source file:avs_task_main.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/audio-visual/avs_task_main.cpp

$(OUTPUT_DIR)/ZZ/code/audio-visual/meritve.o : ZZ/code/audio-visual/meritve.cpp ZZ/code/libs/outputs_inputs/outputs_inputs.hh ZZ/code/common/inc/global.hh ZZ/code/libs/EEPROM/EEPROM.hh ZZ/code/audio-visual/includes/audio.hh ZZ/code/libs/castimer/castimer.hh 
	echo "Compiling C++ source file:meritve.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/audio-visual/meritve.cpp

$(OUTPUT_DIR)/ZZ/code/common/global.o : ZZ/code/common/global.cpp ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/common/inc/global.hh ZZ/code/libs/input/input.hh 
	echo "Compiling C++ source file:global.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/common/global.cpp

$(OUTPUT_DIR)/ZZ/code/Events/events.o : ZZ/code/Events/events.cpp ZZ/code/Events/inc/events.hh ZZ/code/common/inc/global.hh ZZ/code/libs/outputs_inputs/outputs_inputs.hh ZZ/code/libs/EEPROM/EEPROM.hh ZZ/code/audio-visual/includes/audio.hh 
	echo "Compiling C++ source file:events.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/Events/events.cpp

$(OUTPUT_DIR)/ZZ/code/libs/castimer/castimer.o : ZZ/code/libs/castimer/castimer.cpp ZZ/code/libs/castimer/castimer.hh 
	echo "Compiling C++ source file:castimer.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/libs/castimer/castimer.cpp

$(OUTPUT_DIR)/ZZ/code/libs/EEPROM/eeprom.o : ZZ/code/libs/EEPROM/eeprom.cpp ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/libs/EEPROM/EEPROM.hh ZZ/code/common/inc/global.hh 
	echo "Compiling C++ source file:eeprom.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/libs/EEPROM/eeprom.cpp

$(OUTPUT_DIR)/ZZ/code/libs/input/input.o : ZZ/code/libs/input/input.cpp ZZ/code/libs/input/input.hh 
	echo "Compiling C++ source file:input.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/libs/input/input.cpp

$(OUTPUT_DIR)/ZZ/code/libs/outputs_inputs/outputs_inputs.o : ZZ/code/libs/outputs_inputs/outputs_inputs.cpp ZZ/code/libs/outputs_inputs/outputs_inputs.hh ZZ/code/freeRTOS/include/atomic.h ZZ/code/common/inc/global.hh 
	echo "Compiling C++ source file:outputs_inputs.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/libs/outputs_inputs/outputs_inputs.cpp

$(OUTPUT_DIR)/ZZ/code/Power/charging.o : ZZ/code/Power/charging.cpp ZZ/code/libs/input/input.hh ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/libs/castimer/castimer.hh ZZ/code/common/inc/global.hh ZZ/code/libs/outputs_inputs/outputs_inputs.hh ZZ/code/libs/EEPROM/EEPROM.hh 
	echo "Compiling C++ source file:charging.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/Power/charging.cpp

$(OUTPUT_DIR)/ZZ/code/Power/Power.o : ZZ/code/Power/Power.cpp ZZ/code/libs/outputs_inputs/outputs_inputs.hh ZZ/code/common/inc/global.hh ZZ/code/libs/castimer/castimer.hh ZZ/code/libs/input/input.hh ZZ/code/Events/inc/events.hh 
	echo "Compiling C++ source file:Power.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/Power/Power.cpp

$(OUTPUT_DIR)/ZZ/code/User_UI/user_ui.o : ZZ/code/User_UI/user_ui.cpp ZZ/code/libs/EEPROM/EEPROM.hh ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/common/inc/global.hh ZZ/code/audio-visual/includes/audio.hh ZZ/code/libs/outputs_inputs/outputs_inputs.hh ZZ/code/libs/input/input.hh ZZ/code/libs/castimer/castimer.hh ZZ/code/User_UI/inc/user_ui.hh ZZ/code/Events/inc/events.hh 
	echo "Compiling C++ source file:user_ui.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/User_UI/user_ui.cpp

$(OUTPUT_DIR)/ZZ/code/freeRTOS/croutine.o : ZZ/code/freeRTOS/croutine.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h ZZ/code/freeRTOS/include/croutine.h 
	echo "Compiling C source file:croutine.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/croutine.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/event_groups.o : ZZ/code/freeRTOS/event_groups.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h ZZ/code/freeRTOS/include/timers.h ZZ/code/freeRTOS/include/event_groups.h 
	echo "Compiling C source file:event_groups.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/event_groups.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/heap_4.o : ZZ/code/freeRTOS/heap_4.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h 
	echo "Compiling C source file:heap_4.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/heap_4.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/list.o : ZZ/code/freeRTOS/list.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/list.h 
	echo "Compiling C source file:list.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/list.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/port.o : ZZ/code/freeRTOS/port.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h 
	echo "Compiling C source file:port.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/port.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/queue.o : ZZ/code/freeRTOS/queue.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h ZZ/code/freeRTOS/include/queue.h ZZ/code/freeRTOS/include/croutine.h 
	echo "Compiling C source file:queue.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/queue.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/stream_buffer.o : ZZ/code/freeRTOS/stream_buffer.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h ZZ/code/freeRTOS/include/stream_buffer.h 
	echo "Compiling C source file:stream_buffer.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/stream_buffer.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/tasks.o : ZZ/code/freeRTOS/tasks.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h ZZ/code/freeRTOS/include/timers.h ZZ/code/freeRTOS/include/stack_macros.h 
	echo "Compiling C source file:tasks.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/tasks.c

$(OUTPUT_DIR)/ZZ/code/freeRTOS/timers.o : ZZ/code/freeRTOS/timers.c ZZ/code/freeRTOS/include/FreeRTOS.h ZZ/code/freeRTOS/include/task.h ZZ/code/freeRTOS/include/queue.h ZZ/code/freeRTOS/include/timers.h 
	echo "Compiling C source file:timers.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c ZZ/code/freeRTOS/timers.c



.SILENT:

