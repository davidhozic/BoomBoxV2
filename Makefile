C_COMPILER := avr-gcc
CPP_COMPILER := avr-g++
LINKER := avr-g++
C_FLAGS := -std=c99\

CPP_FLAGS := -std=c++17\

COMMON_FLAGS := -D F_CPU=16000000\
-mmcu=atmega2560\
-Os

GLOBAL_INC := -include stdint.h\
-include settings.hpp\

OUTPUT_DIR := Build
OUTPUT_NAME := main

MCU_PART = m2560
PROGRAMMER = usbasp-clone

FOLDER_INCLUDE:= -I code/\
-I code/audio-visual\
-I code/audio-visual/includes\
-I code/common\
-I code/common/inc\
-I code/Events\
-I code/Events/inc\
-I code/libs\
-I code/libs/castimer\
-I code/libs/castimer/.vscode\
-I code/libs/EEPROM\
-I code/libs/FreeRTOS_m2560\
-I code/libs/FreeRTOS_m2560/include\
-I code/libs/input\
-I code/libs/linked_list\
-I code/libs/outputs_inputs\
-I code/Power\
-I code/Power/inc\
-I code/User_UI\
-I code/User_UI/inc\

CPP := code//bootloader.cpp\
code/audio-visual/audio_system_functions.cpp\
code/audio-visual/auvs_task.cpp\
code/audio-visual/avs_colors_animations.cpp\
code/common/common.cpp\
code/Events/events.cpp\
code/libs/castimer/castimer.cpp\
code/libs/EEPROM/eeprom.cpp\
code/libs/input/input.cpp\
code/libs/outputs_inputs/outputs_inputs.cpp\
code/Power/Power.cpp\
code/User_UI/user_ui.cpp\

C := code/libs/FreeRTOS_m2560/croutine.c\
code/libs/FreeRTOS_m2560/event_groups.c\
code/libs/FreeRTOS_m2560/heap_4.c\
code/libs/FreeRTOS_m2560/list.c\
code/libs/FreeRTOS_m2560/port.c\
code/libs/FreeRTOS_m2560/queue.c\
code/libs/FreeRTOS_m2560/stream_buffer.c\
code/libs/FreeRTOS_m2560/tasks.c\
code/libs/FreeRTOS_m2560/timers.c\

SRC_DIR := code/ code/audio-visual code/audio-visual/includes code/common code/common/inc code/Events code/Events/inc code/libs code/libs/castimer code/libs/castimer/.vscode code/libs/EEPROM code/libs/FreeRTOS_m2560 code/libs/FreeRTOS_m2560/include code/libs/input code/libs/linked_list code/libs/outputs_inputs code/Power code/Power/inc code/User_UI code/User_UI/inc 


O := $(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/croutine.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/event_groups.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/heap_4.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/list.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/port.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/queue.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/stream_buffer.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/tasks.o\
$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/timers.o\
$(OUTPUT_DIR)/code//bootloader.o\
$(OUTPUT_DIR)/code/audio-visual/audio_system_functions.o\
$(OUTPUT_DIR)/code/audio-visual/auvs_task.o\
$(OUTPUT_DIR)/code/audio-visual/avs_colors_animations.o\
$(OUTPUT_DIR)/code/common/common.o\
$(OUTPUT_DIR)/code/Events/events.o\
$(OUTPUT_DIR)/code/libs/castimer/castimer.o\
$(OUTPUT_DIR)/code/libs/EEPROM/eeprom.o\
$(OUTPUT_DIR)/code/libs/input/input.o\
$(OUTPUT_DIR)/code/libs/outputs_inputs/outputs_inputs.o\
$(OUTPUT_DIR)/code/Power/Power.o\
$(OUTPUT_DIR)/code/User_UI/user_ui.o\

all: clean compile flash


clean:
	echo "------------------------------------"
	echo " STEP[]: Cleaning Folder $(OUTPUT_DIR)     "
	echo "------------------------------------"
	rm -rf $(OUTPUT_DIR)
	sleep 0.5



compile: echo_compile mkdir $(O)
	echo "------------------------------------"
	echo " STEP[]: LINKING INTO ELF           "
	echo "------------------------------------"
	sleep 0.5
	$(LINKER) $(O) $(COMMON_FLAGS) -o $(OUTPUT_DIR)/$(OUTPUT_NAME).elf
	echo "------------------------------------"
	echo " STEP[]: CONVERTING INTO HEX        "
	echo "------------------------------------"
	sleep 0.5
	avr-objcopy -j .text -j .data -R .eeprom -O ihex $(OUTPUT_DIR)/$(OUTPUT_NAME).elf $(OUTPUT_DIR)/$(OUTPUT_NAME).hex

flash : 
	echo "------------------------------------"
	echo " STEP[]: FLASHING TO $(MCU_PART)    "
	echo "------------------------------------"
	sleep 0.5
	avrdude -p $(MCU_PART) -c $(PROGRAMMER) -U flash:w:"$(OUTPUT_DIR)/$(OUTPUT_NAME).hex"

mkdir : 
	for dir in $(SRC_DIR); do mkdir -p $(OUTPUT_DIR)/$$dir; done

echo_compile : 
	echo "------------------------------------"
	echo " STEP[]: COMPILING SOURCE FILES     "
	echo "------------------------------------"
	sleep 0.5

$(OUTPUT_DIR)/code//bootloader.o : code//bootloader.cpp code/Events/inc/events.hpp code/libs/outputs_inputs/outputs_inputs.hpp 
	echo "Compiling C++ source file:bootloader.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code//bootloader.cpp

$(OUTPUT_DIR)/code/audio-visual/audio_system_functions.o : code/audio-visual/audio_system_functions.cpp code/common/inc/common.hpp code/libs/EEPROM/EEPROM.hpp code/audio-visual/includes/audio.hpp code/libs/outputs_inputs/outputs_inputs.hpp 
	echo "Compiling C++ source file:audio_system_functions.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/audio-visual/audio_system_functions.cpp

$(OUTPUT_DIR)/code/audio-visual/auvs_task.o : code/audio-visual/auvs_task.cpp code/libs/outputs_inputs/outputs_inputs.hpp code/common/inc/common.hpp code/libs/EEPROM/EEPROM.hpp code/audio-visual/includes/audio.hpp code/libs/castimer/castimer.hpp 
	echo "Compiling C++ source file:auvs_task.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/audio-visual/auvs_task.cpp

$(OUTPUT_DIR)/code/audio-visual/avs_colors_animations.o : code/audio-visual/avs_colors_animations.cpp code/audio-visual/includes/audio.hpp 
	echo "Compiling C++ source file:avs_colors_animations.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/audio-visual/avs_colors_animations.cpp

$(OUTPUT_DIR)/code/common/common.o : code/common/common.cpp code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/common/inc/common.hpp code/libs/input/input.hpp 
	echo "Compiling C++ source file:common.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/common/common.cpp

$(OUTPUT_DIR)/code/Events/events.o : code/Events/events.cpp code/Events/inc/events.hpp code/common/inc/common.hpp code/libs/outputs_inputs/outputs_inputs.hpp code/libs/EEPROM/EEPROM.hpp code/audio-visual/includes/audio.hpp 
	echo "Compiling C++ source file:events.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/Events/events.cpp

$(OUTPUT_DIR)/code/libs/castimer/castimer.o : code/libs/castimer/castimer.cpp code/libs/castimer/castimer.hpp code/libs/FreeRTOS_m2560/include/atomic.h 
	echo "Compiling C++ source file:castimer.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/castimer/castimer.cpp

$(OUTPUT_DIR)/code/libs/EEPROM/eeprom.o : code/libs/EEPROM/eeprom.cpp code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/EEPROM/EEPROM.hpp code/common/inc/common.hpp 
	echo "Compiling C++ source file:eeprom.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/EEPROM/eeprom.cpp

$(OUTPUT_DIR)/code/libs/input/input.o : code/libs/input/input.cpp code/libs/input/input.hpp 
	echo "Compiling C++ source file:input.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/input/input.cpp

$(OUTPUT_DIR)/code/libs/outputs_inputs/outputs_inputs.o : code/libs/outputs_inputs/outputs_inputs.cpp code/libs/outputs_inputs/outputs_inputs.hpp code/libs/FreeRTOS_m2560/include/atomic.h code/common/inc/common.hpp 
	echo "Compiling C++ source file:outputs_inputs.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/outputs_inputs/outputs_inputs.cpp

$(OUTPUT_DIR)/code/Power/Power.o : code/Power/Power.cpp code/common/inc/common.hpp code/Events/inc/events.hpp code/libs/input/input.hpp code/libs/castimer/castimer.hpp code/libs/outputs_inputs/outputs_inputs.hpp code/libs/EEPROM/EEPROM.hpp code/Power/inc/power.hpp 
	echo "Compiling C++ source file:Power.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/Power/Power.cpp

$(OUTPUT_DIR)/code/User_UI/user_ui.o : code/User_UI/user_ui.cpp code/libs/EEPROM/EEPROM.hpp code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/common/inc/common.hpp code/audio-visual/includes/audio.hpp code/libs/outputs_inputs/outputs_inputs.hpp code/libs/input/input.hpp code/libs/castimer/castimer.hpp code/User_UI/inc/user_ui.hpp code/Events/inc/events.hpp 
	echo "Compiling C++ source file:user_ui.cpp"
	$(CPP_COMPILER) $(CPP_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/User_UI/user_ui.cpp

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/croutine.o : code/libs/FreeRTOS_m2560/croutine.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h code/libs/FreeRTOS_m2560/include/croutine.h 
	echo "Compiling C source file:croutine.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/croutine.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/event_groups.o : code/libs/FreeRTOS_m2560/event_groups.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h code/libs/FreeRTOS_m2560/include/timers.h code/libs/FreeRTOS_m2560/include/event_groups.h 
	echo "Compiling C source file:event_groups.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/event_groups.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/heap_4.o : code/libs/FreeRTOS_m2560/heap_4.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h 
	echo "Compiling C source file:heap_4.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/heap_4.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/list.o : code/libs/FreeRTOS_m2560/list.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/list.h 
	echo "Compiling C source file:list.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/list.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/port.o : code/libs/FreeRTOS_m2560/port.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h 
	echo "Compiling C source file:port.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/port.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/queue.o : code/libs/FreeRTOS_m2560/queue.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h code/libs/FreeRTOS_m2560/include/queue.h code/libs/FreeRTOS_m2560/include/croutine.h 
	echo "Compiling C source file:queue.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/queue.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/stream_buffer.o : code/libs/FreeRTOS_m2560/stream_buffer.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h code/libs/FreeRTOS_m2560/include/stream_buffer.h 
	echo "Compiling C source file:stream_buffer.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/stream_buffer.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/tasks.o : code/libs/FreeRTOS_m2560/tasks.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h code/libs/FreeRTOS_m2560/include/timers.h code/libs/FreeRTOS_m2560/include/stack_macros.h 
	echo "Compiling C source file:tasks.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/tasks.c

$(OUTPUT_DIR)/code/libs/FreeRTOS_m2560/timers.o : code/libs/FreeRTOS_m2560/timers.c code/libs/FreeRTOS_m2560/include/FreeRTOS.h code/libs/FreeRTOS_m2560/include/task.h code/libs/FreeRTOS_m2560/include/queue.h code/libs/FreeRTOS_m2560/include/timers.h 
	echo "Compiling C source file:timers.c"
	$(C_COMPILER) $(C_FLAGS) $(COMMON_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@ -c code/libs/FreeRTOS_m2560/timers.c



.SILENT:

