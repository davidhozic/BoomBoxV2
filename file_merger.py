import os
import sys
import time


SEARCH_DIR = "./"
OUTPUT_FILE = "ALL_IN_ONE.cpp"

EXTENSIONS_TO_USE = [".cpp", ".c", ".h", ".hpp", ".hh"]
FOLDERS_TO_IGNORE = ["FreeRTOS_m2560"]

LANGUAGE_ENDINGS  = [ ("Python", ".py"), ("C", ".c", ".h"), ("C++", ".cpp", ".cc", ".hpp", ".hh")]


#########################################################################################################################

def find_lang_by_extension(extension):
    for lang in LANGUAGE_ENDINGS:
        if extension in lang:
            return lang[0]
    return "NEZNAN"

# STEP CREATE FILE DATA
data_output_file = ""
file_count = 0
total_line_count = 0
for dir, dirname, files in os.walk(SEARCH_DIR, topdown=True):

    for ign_fld in FOLDERS_TO_IGNORE:
        if dir.find(ign_fld) != -1:
            break
    else:
        if OUTPUT_FILE in files:
            files.remove(OUTPUT_FILE)
        for file in files:
            for extension in EXTENSIONS_TO_USE:
                if file.endswith(extension):
                    print("Processing file: " + file)
                    dir = dir.replace("\\", "/")
                    file = dir + "/" + file

                    file_count += 1
                    total_line_count += sum(1 for line in open(file=file,
                                            mode="r", encoding="utf-8"))

                    lang = find_lang_by_extension(extension)
                    data_output_file += "\n\
/*###############################################################################################################################################\n\
-> IME DATOTEKE: " + file + " \n\
-> JEZIK: " + lang + "\n\
#################################################################################################################################################*/\n"
                    with open(file=file, mode="r", encoding="utf-8") as opened_file:
                        data_output_file += opened_file.read()
                    break


file_info = "\n\n\n\n\
============================================================================================\n\
    ! ! ! AUTOMATICNO GENERIRANA DATOTEKA PREK SKRIPTE ! ! !    \n\
    --------------------------------------------------------------\n\
    -> STEVILO SKUPNIH VRSTIC (s presledki): " + str(total_line_count) + "\n" + "\
    -> STEVILO DATOTEK VKLJUCENIH V SKUPNO DATOTEKO: " + str(file_count) + "\n\
============================================================================================\n\n\n\n"

data_output_file = file_info + data_output_file


# STEP WRITE FILE
output_file = open(file=OUTPUT_FILE, mode="w", encoding="utf-8")
output_file.write(data_output_file)
output_file.close()
