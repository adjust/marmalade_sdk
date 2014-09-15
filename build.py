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
    parser.add_argument("--keep_source", "-k", action="store_true", help="Keep generated sources")
    parser.add_argument("--build_android", "-ba", action="store_true", help="Build android build")
    parser.add_argument("--compile_android", "-ca", action="store_true", help="Compile android build")
    parser.add_argument("--build_iphone", "-bi", action="store_true", help="Build iphone build")
    parser.add_argument("--compile_iphone", "-ci", action="store_true", help="Compile iphone build")


    global parsed_args
    parsed_args = parser.parse_args()
    
    with open('adjust/AdjustBuildLog.txt','w') as fileLog:
        # log function with file injected
        global Log
        Log = LogInput(fileLog)
        if not (parsed_args.compile_android or parsed_args.compile_iphone):
            clean()
        if not parsed_args.clean:
            if not (parsed_args.build_iphone or parsed_args.compile_iphone):
                if not parsed_args.compile_android:
                    build_android()
                if not parsed_args.build_android:
                    compile_android()
            if not (parsed_args.compile_android or parsed_args.build_android):
                if not parsed_args.compile_iphone:
                    build_iphone()
                if not parsed_args.build_iphone:
                    compile_iphone()
    
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

    Log("edk-build android out: {0}".format(out))
    if (edk_build.returncode not in [0]):
        print("edk-build android code: {0}, err: {1}".format(
            edk_build.returncode, err))

    copy_android()
    
    edit_android_mkb()
    edit_android_java_mkb()
    edit_android_mkf()
    move_android_source()

def build_iphone():
    edk_build = Popen(["edk-build", "adjust/AdjustMarmalade.s4e", "--platform=iphone"],
            stdout=PIPE, stderr=PIPE)
    out, err = edk_build.communicate()

    Log("edk-build iphone out: {0}".format(out))
    if (edk_build.returncode not in [0]):
        print("edk-build iphone code: {0}, err: {1}".format(
            edk_build.returncode, err))

    move_iphone_source()

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

def compile_iphone():
    mkb_iphone = Popen(["mkb", "adjust/AdjustMarmalade_iphone.mkb", "--buildenv=xcode"],
    #mkb_iphone = Popen(["mkb", "adjust/AdjustMarmalade_iphone.mkb", "--buildenv=scons"],
            stdout=PIPE, stderr=PIPE)
    out, err = mkb_iphone.communicate()

    Log("mkb iphone out: {0}".format(out))
    if (mkb_iphone.returncode not in [0]):
        print("mkb iphone code: {0}, err: {1}".format(
            mkb_iphone.returncode, err))

def move_android_source():
    if parsed_args.keep_source:
        shutil.copy("adjust/source/android/AdjustMarmalade.java", "source/android/AdjustMarmalade.bak.java")
        shutil.copy("adjust/source/android/AdjustMarmalade_platform.cpp", "source/android/AdjustMarmalade_platform.bak.cpp")
        #shutil.copy("adjust/source/h/AdjustMarmalade_internal.h", "source/h/AdjustMarmalade_internal.bak.h")
    else:
        shutil.copy("source/android/AdjustMarmalade.java", "adjust/source/android")
        shutil.copy("source/android/AdjustMarmalade_platform.cpp", "adjust/source/android")
        #shutil.copy("source/h/AdjustMarmalade_internal.h", "adjust/source/h")
        shutil.copytree("source/rapidjson", "adjust/rapidjson")

    Log("copied android source")

def move_iphone_source():
    if parsed_args.keep_source:
        shutil.copy("adjust/source/iphone/AdjustMarmalade_platform.mm", "source/iphone/AdjustMarmalade_platform.bak.mm")
    else:
        shutil.copy("source/iphone/AdjustMarmalade_platform.mm", "adjust/source/iphone")

    Log("copied iphone source")

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
