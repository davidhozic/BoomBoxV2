import os, shutil



LIB_SOURCE_PATH = "../LITL"
DESTINATION_PATH = "code/libs/"

FOLDERS_TO_COPY = ["castimer", "linked_list", "input"]     # Empty to copy all


if len(FOLDERS_TO_COPY) > 0:
    for folder in FOLDERS_TO_COPY:
        shutil.copytree(LIB_SOURCE_PATH+"/"+folder, DESTINATION_PATH+"/"+folder, dirs_exist_ok=True)
else:
    shutil.copytree(LIB_SOURCE_PATH, DESTINATION_PATH, dirs_exist_ok=True)


