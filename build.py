#!/usr/bin/env python

import argparse
import shutil
import os
from subprocess import Popen, PIPE
import fnmatch
import re

def main():
    parser = argparse.ArgumentParser(description="Adjust SDK build script for marmalade")
    parser.add_argument("--clean", "-c", action="store_true", help="Clean previously builds")
    args = parser.parse_args()
    
    clean()
    if not args.clean:
        build_android()
    
def clean():
    for dirpath, dirnames, filenames in os.walk("adjust/"):
        for dirname in dirnames:
            shutil.rmtree(os.path.join(dirpath, dirname))
            #print(dirname)
        for filename in filenames:
            if fnmatch.fnmatch(filename, "*.mkb") or fnmatch.fnmatch(filename, "*.mkf"):
                os.remove(os.path.join(dirpath, filename))
        break

def build_android():
    edk_build = Popen(["edk-build", "adjust/Adjust.s4e", "--platform=android"],
            stdout=PIPE, stderr=PIPE)
    out, err = edk_build.communicate()

    copy_android()

    edit_android_mkb()
    edit_android_mkf()

    mkb_android_java = Popen(["mkb", "adjust/Adjust_android_java.mkb"],
            stdout=PIPE, stderr=PIPE)
    out, err = mkb_android_java.communicate()

    mkb_android = Popen(["mkb", "adjust/Adjust_android.mkb"],
            stdout=PIPE, stderr=PIPE)
    out, err = mkb_android.communicate()

def copy_android():
    os.makedirs("adjust/incoming")
    shutil.copy("android_libs/adjust.jar", "adjust/incoming/")
    shutil.copy("android_libs/google-play-services.jar", "adjust/incoming/")

def edit_android_mkb():
    mkb_file_lines = []
    mkb_filename = "adjust/Adjust_android_java.mkb"

    librarypath_re = re.compile("s3e_release.jar")
    with open(mkb_filename) as f:
        for line in f:
            mkb_file_lines.append(line)
            if librarypath_re.search(line):
                mkb_file_lines.append("librarypath \"incoming/adjust.jar\"\n")
                mkb_file_lines.append("librarypath \"incoming/google-play-services.jar\"\n")
    with open(mkb_filename, "w+") as f:
        f.writelines(mkb_file_lines)

def edit_android_mkf():
    mkf_file_lines = []
    mkf_filename = "adjust/Adjust.mkf"

    external_jar_re = re.compile("android-external-jar")
    with open(mkf_filename) as f:
        for line in f:
            mkf_file_lines.append(line)
            if external_jar_re.search(line):
                mkf_file_lines.append("    android-external-jars=\"incoming/adjust.jar\"\n")
                mkf_file_lines.append("    android-external-jars=\"incoming/google-play-services.jar\"\n")
    with open(mkf_filename, "w+") as f:
        f.writelines(mkf_file_lines)

if __name__ == "__main__":
    main()
