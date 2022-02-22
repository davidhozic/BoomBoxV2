import os, sys, time



SEARCH_DIR = "./"
OUTPUT_FILE = "ALL_IN_ONE.cpp"

EXTENSIONS_TO_USE = [".cpp",".c",".h",".hpp",".hh",".py",".pyw"]
FOLDERS_TO_IGNORE = ["FreeRTOS"]



class LANGEXTENSION:
    def __init__(this, lang,  *extensions):
        this.lang = lang
        this.extensions = extensions

CONST_EXTENSIONS = \
    [
        LANGEXTENSION("C", ".c",".h"),
        LANGEXTENSION("C++", ".cpp",".hpp"),
        LANGEXTENSION("Python", ".py",".pyw")
    ]



#STEP CREATE FILE DATA
data_output_file = ""
file_count = 0
total_line_count = 0
for dir, dirname, files in os.walk(SEARCH_DIR, topdown=False):

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
                    dir = dir.replace("\\","/")
                    file = dir + "/" + file

                    file_count += 1
                    total_line_count += sum (1 for line in open(file=file, mode="r", encoding="utf-8") if line.strip() != "")

                    lang = "Unknown"
                    for ext in CONST_EXTENSIONS:
                        for ending in ext.extensions:
                            if file.endswith(ending):
                                lang = ext.lang
                                break


                    data_output_file += "\n\
/*###############################################################################################################################################\n\
-> FILENAME : " + file + " \n\
-> LANGUAGE: " + lang + "\n\
#################################################################################################################################################*/\n"
                    with open(file=file,mode="r",encoding="utf-8") as opened_file:
                        data_output_file += opened_file.read()
                    break


file_info = "\n\n\n\n\
============================================================================================\n\
    ! ! ! AUTOMATICALLY GENERATED FILE ! ! !    \n\
    --------------------------------------------------------------\n\
    -> NUMBER OF LINES (excluding empty lines): " + str(total_line_count) + "\n" + "\
    -> NUMBER OF FILES MERGED: " + str(file_count) + "\n\
============================================================================================\n\n\n\n"

data_output_file = file_info + data_output_file



#STEP WRITE FILE
output_file = open(file=OUTPUT_FILE, mode="w",encoding="utf-8")
output_file.write(data_output_file)
output_file.close()