
import os, shutil

SOURCE_DIR = "C:/Users/McHea/Google Drive/Projekti"
LIB_NAME = "Knjiznice"
TARGET_DIR = "D:/Documents/GitHub/BoomBoxV2/ZZ/code/libs"



shutil.copytree(SOURCE_DIR+"/"+LIB_NAME, TARGET_DIR, dirs_exist_ok=True)
    