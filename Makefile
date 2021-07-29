C_COMPILER := avr-gcc
CPP_COMPILER := avr-g++
LINKER := avr-g++
C_FLAGS :=\
-std=c99


CPP_FLAGS :=\
-std=c++17


COMMON_FLAGS :=\
-D F_CPU=16000000\
-mmcu=atmega2560\
-Os


GLOBAL_INC :=\
-include stdint.h\
-include settings.hpp


OUTPUT_DIR := Build
OUTPUT_NAME := main

MCU_PART = m2560
PROGRAMMER = usbasp-clone

FOLDER_INCLUDE:=\
-I code\
-I code/libs\
-I code/libs/linked_list\
-I code/libs/castimer\
-I code/libs/outputs_inputs\
-I code/libs/EEPROM\
-I code/libs/input\
-I code/libs/FreeRTOS_m2560_port-V10.4-develop\
-I code/libs/FreeRTOS_m2560_port-V10.4-develop/include\
-I code/audio-visual\
-I code/audio-visual/includes\
-I code/Events\
-I code/Events/inc\
-I code/common\
-I code/common/inc\
-I code/Power\
-I code/Power/inc\
-I code/User_UI\
-I code/User_UI/inc


CPP :=\
code/bootloader.cpp\
code/libs/castimer/castimer.cpp\
code/libs/outputs_inputs/outputs_inputs.cpp\
code/libs/EEPROM/eeprom.cpp\
code/libs/input/input.cpp\
code/audio-visual/avs_colors_animations.cpp\
code/audio-visual/auvs_task.cpp\
code/audio-visual/audio_system_functions.cpp\
code/Events/events.cpp\
code/common/common.cpp\
code/Power/Power.cpp\
code/User_UI/user_ui.cpp


C :=\
code/libs/FreeRTOS_m2560_port-V10.4-develop/port.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/heap_4.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/stream_buffer.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/list.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/timers.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/event_groups.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/croutine.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/queue.c\
code/libs/FreeRTOS_m2560_port-V10.4-develop/tasks.c


SRC_DIR :=\
code \
code/libs \
code/libs/linked_list \
code/libs/castimer \
code/libs/outputs_inputs \
code/libs/EEPROM \
code/libs/input \
code/libs/FreeRTOS_m2560_port-V10.4-develop \
code/libs/FreeRTOS_m2560_port-V10.4-develop/include \
code/audio-visual \
code/audio-visual/includes \
code/Events \
code/Events/inc \
code/common \
code/common/inc \
code/Power \
code/Power/inc \
code/User_UI \
code/User_UI/inc 


O := $(patsubst %.cpp,$(OUTPUT_DIR)/%.o,$(CPP))  
O += $(patsubst %.c,$(OUTPUT_DIR)/%.o,$(C))



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


$(OUTPUT_DIR)/%.o : %.c
	echo Compiling C SOURCE  $^
	$(C_COMPILER) -c $^ $(COMMON_FLAGS) $(C_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@

$(OUTPUT_DIR)/%.o : %.cpp
	echo Compiling CPP SOURCE  $^
	$(CPP_COMPILER) -c $^ $(COMMON_FLAGS) $(CPP_FLAGS) $(GLOBAL_INC) $(FOLDER_INCLUDE) -o $@



.SILENT:

