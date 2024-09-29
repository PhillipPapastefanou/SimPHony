
import os
import glob
import sys
import importlib


def listdir_fullpath(d):
    return [os.path.join(d, f) for f in os.listdir(d)]

def Get_SimPHony_build_path(directory):
    root_path = os.path.join(directory, os.pardir, os.pardir)
    dir_entries = listdir_fullpath(root_path)
    folders = [a for a in dir_entries if os.path.isdir(a)]

    found_lib = False
    lib_path = ""
    lib_folder = ""
    for str_path in folders:

        sub_dir_entries = listdir_fullpath(str_path)
        files = [a for a in sub_dir_entries if os.path.isfile(a)]

        for file in files:
            if (file.endswith(".so")) & (not 'debug' in file):
                found_lib = True
                lib_path = file
                lib_folder = str_path
                break

    return found_lib, lib_path, lib_folder