#!/usr/bin/env python

import argparse
import shutil
import os
from subprocess import Popen, PIPE
import fnmatch
import re

Log = None 
parsed_args = None 

def main():
    parser = argparse.ArgumentParser(description="Adjust SDK build script for marmalade")
    parser.add_argument("--clean", "-c", action="store_true", help="Clean previously builds")
    parser.add_argument("--compile_android", "-ca", action="store_true", help="Compile android build")
    parser.add_argument("--build_android", "-ba", action="store_true", help="Build android build")
    parser.add_argument("--keep_source", "-k", action="store_true", help="Keep generated sources")

    global parsed_args
    parsed_args = parser.parse_args()
    
    with open('adjust/AdjustBuildLog.txt','w') as fileLog:
        # log function with file injected
        global Log
        Log = LogInput(fileLog)
        if not parsed_args.compile_android:
            clean()
        if not parsed_args.clean:
            if not parsed_args.compile_android:
                build_android()
            if not parsed_args.build_android:
                compile_android()
    
def LogInput(writeObject):
    def Log(message, *args):
        messageNLine = str(message) + "\n"
        writeObject.write(messageNLine.format(*args))
    return Log

def clean():
    for dirpath, dirnames, filenames in os.walk("adjust/"):
        for dirname in dirnames:
            Log("deleting folder: " + dirname)
            shutil.rmtree(os.path.join(dirpath, dirname))
        for filename in filenames:
            if fnmatch.fnmatch(filename, "*.mkb") or fnmatch.fnmatch(filename, "*.mkf"):
                Log("deleting file: " + filename)
                os.remove(os.path.join(dirpath, filename))
        break

def build_android():
    edk_build = Popen(["edk-build", "adjust/AdjustMarmalade.s4e", "--platform=android"],
            stdout=PIPE, stderr=PIPE)
    out, err = edk_build.communicate()

    Log("edk-build out: {0}".format(out))
    if (edk_build.returncode not in [0]):
        print("edk-build code: {0}, err: {1}".format(
            edk_build.returncode, err))

    copy_android()
    
    edit_android_mkb()
    edit_android_java_mkb()
    edit_android_mkf()
    move_android_source()

def compile_android():
    mkb_android_java = Popen(["mkb", "adjust/AdjustMarmalade_android_java.mkb"],
            stdout=PIPE, stderr=PIPE)
    out, err = mkb_android_java.communicate()

    Log("mkb android java out: {0}".format(out))
    if (mkb_android_java.returncode not in [0]):
        print("mkb android java code: {0}, err: {1}".format(
            mkb_android_java.returncode, err))

    mkb_android = Popen(["mkb", "adjust/AdjustMarmalade_android.mkb"],
            stdout=PIPE, stderr=PIPE)
    out, err = mkb_android.communicate()

    Log("mkb android out: {0}".format(out))
    if (mkb_android.returncode not in [0]):
        print("mkb android code: {0}, err: {1}".format(
            mkb_android.returncode, err))


def move_android_source():
    if parsed_args.keep_source:
        shutil.copy("adjust/source/android/AdjustMarmalade.java", "source/android/AdjustMarmalade.bak.java")
        shutil.copy("adjust/source/android/AdjustMarmalade_platform.cpp", "source/android/AdjustMarmalade_platform.bak.cpp")
        shutil.copy("adjust/source/h/AdjustMarmalade_internal.h", "source/h/AdjustMarmalade_internal.bak.h")
    else:
        shutil.copy("source/android/AdjustMarmalade.java", "adjust/source/android")
        shutil.copy("source/android/AdjustMarmalade_platform.cpp", "adjust/source/android")
        shutil.copy("source/h/AdjustMarmalade_internal.h", "adjust/source/h")
        shutil.copytree("source/generic/rapidjson", "adjust/rapidjson")
        #shutil.copytree("source/generic/json-parser", "adjust/json-parser")
        #shutil.copytree("source/generic/cJSON-for-marmalade", "adjust/cJSON")
        #shutil.copy("source/generic/cJSON-for-marmalade/cJSON.c", "adjust/source/generic/cJSON.c")
        #shutil.copy("source/generic/cJSON-for-marmalade/cJSON.h", "adjust/source/generic/cJSON.h")

    Log("copied java source")

def copy_android():
    os.makedirs("adjust/incoming")
    shutil.copy("android_libs/adjust.jar", "adjust/incoming/")
    Log("copied jar libs")

def edit_android_mkb():
    mkb_filename = "adjust/AdjustMarmalade_android.mkb"

    with open(mkb_filename, "a") as f:
        f.write("subproject rapidjson")

def edit_android_java_mkb():
    mkb_file_lines = []
    mkb_filename = "adjust/AdjustMarmalade_android_java.mkb"

    librarypath_re = re.compile("s3e_release.jar")
    with open(mkb_filename) as f:
        for line in f:
            mkb_file_lines.append(line)
            if librarypath_re.search(line):
                mkb_file_lines.append("librarypath \"incoming/adjust.jar\"\n")
                Log("append librarypath to android java mkb")
    with open(mkb_filename, "w+") as f:
        f.writelines(mkb_file_lines)

def edit_android_mkf():
    mkf_file_lines = []
    mkf_filename = "adjust/AdjustMarmalade.mkf"

    external_jar_re = re.compile("android-external-jar")
    with open(mkf_filename) as f:
        for line in f:
            mkf_file_lines.append(line)
            if external_jar_re.search(line):
                mkf_file_lines.append("    android-external-jars=\"incoming/adjust.jar\"\n")
                Log("append android external jars to mkf")
    with open(mkf_filename, "w+") as f:
        f.writelines(mkf_file_lines)

if __name__ == "__main__":
    main()
