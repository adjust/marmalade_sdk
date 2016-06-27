#!/usr/bin/env python

import argparse
import shutil
import os
from subprocess import Popen, PIPE
import fnmatch
import re

from mod_pbxproj import XcodeProject

Log = None 
dd = None 

def main():
    parser = argparse.ArgumentParser(description="Adjust SDK build script for marmalade")
    parser.add_argument("--clean", "-c", action="store_true", help="Clean previously builds")
    parser.add_argument("--keep_source", "-k", action="store_true", help="Keep generated sources")
    parser.add_argument("--build_android", "-ba", action="store_true", help="Build android build")
    parser.add_argument("--compile_android", "-ca", action="store_true", help="Compile android build")
    parser.add_argument("--build_compile_android", "-bca", action="store_true", help="Build and compile android build")
    parser.add_argument("--build_iphone", "-bi", action="store_true", help="Build iphone build")
    parser.add_argument("--compile_iphone", "-ci", action="store_true", help="Compile iphone build")
    parser.add_argument("--build_compile_iphone", "-bci", action="store_true", help="Build and compile iphone build")
    parser.add_argument("--post_compile", "-pc", action="store_true", help="Post processing build")

    global parsed_args
    parsed_args = parser.parse_args()
    
    with open('adjust/AdjustBuildLog.txt','w') as fileLog:
        # Log function with file injected.
        global Log
        Log = LogInput(fileLog)
        if parsed_args.post_compile:
            post_compile()
            return
        if (parsed_args.clean):
            clean()
        if not parsed_args.clean:
            if not (parsed_args.compile_android or parsed_args.build_android or parsed_args.build_compile_android):
                if not parsed_args.compile_iphone:
                    build_iphone()
                if not parsed_args.build_iphone:
                    compile_iphone()
            if not (parsed_args.build_iphone or parsed_args.compile_iphone or parsed_args.build_compile_iphone):
                if not parsed_args.compile_android:
                    build_android()
                if not parsed_args.build_android:
                    compile_android()
                    post_compile()
    
def LogInput(writeObject):
    def Log(message, *args):
        messageNLine = str(message) + "\n"
        writeObject.write(messageNLine.format(*args))
    return Log

def clean():
    for dirpath, dirnames, filenames in os.walk("adjust/"):
        for dirname in dirnames:
            Log("Deleting folder: " + dirname)
            shutil.rmtree(os.path.join(dirpath, dirname))
        for filename in filenames:
            if fnmatch.fnmatch(filename, "*.mkb") or fnmatch.fnmatch(filename, "*.mkf"):
                Log("Deleting file: " + filename)
                os.remove(os.path.join(dirpath, filename))
        break

def build_android():
    edk_build = Popen(["edk-build", "--force", "adjust/AdjustMarmalade.s4e", "--platform=android"], stdout=PIPE, stderr=PIPE)
    out, err = edk_build.communicate()

    Log("edk-build android out: {0}".format(out))
    if (edk_build.returncode not in [0]):
        print("edk-build android code: {0}, err: {1}".format(
            edk_build.returncode, err))
    
    edit_android_mkb()
    edit_android_java_mkb()
    move_android_source()

    # Not needed due to move.
    #edit_mkf("android")

def build_iphone():
    edk_build = Popen(["edk-build", "--force", "adjust/AdjustMarmalade.s4e", "--platform=iphone"], stdout=PIPE, stderr=PIPE)
    out, err = edk_build.communicate()

    Log("edk-build iphone out: {0}".format(out))
    if (edk_build.returncode not in [0]):
        print("edk-build iphone code: {0}, err: {1}".format(
            edk_build.returncode, err))
    
    move_iphone_source()

    # Not needed due to move.
    #edit_mkf("iphone")
    #edit_build_iphone_mkf()

def compile_android():
    mkb_android_java = Popen(["mkb", "adjust/AdjustMarmalade_android_java.mkb"], stdout=PIPE, stderr=PIPE)
    out, err = mkb_android_java.communicate()

    Log("mkb android java out: {0}".format(out))
    if (mkb_android_java.returncode not in [0]):
        print("mkb android java code: {0}, err: {1}".format(
            mkb_android_java.returncode, err))

    mkb_android = Popen(["mkb", "adjust/AdjustMarmalade_android.mkb"], stdout=PIPE, stderr=PIPE)
    out, err = mkb_android.communicate()

    Log("mkb android out: {0}".format(out))
    if (mkb_android.returncode not in [0]):
        print("mkb android code: {0}, err: {1}".format(
            mkb_android.returncode, err))

def compile_iphone():
    #mkb_iphone = Popen(["mkb", "adjust/AdjustMarmalade_iphone.mkb", "--buildenv=scons"],
    mkb_iphone = Popen(["mkb", "adjust/AdjustMarmalade_iphone.mkb", "--buildenv=xcode"],
            stdout=PIPE, stderr=PIPE)
    out, err = mkb_iphone.communicate()

    Log("mkb iphone out: {0}".format(out))
    if (mkb_iphone.returncode not in [0]):
        print("mkb iphone code: {0}, err: {1}".format(
            mkb_iphone.returncode, err))

    edit_xcode_project("adjust/build_adjustmarmalade_iphone_iphone_xcode/AdjustMarmalade_iphone.xcodeproj/project.pbxproj")

def copytree(src, dst, symlinks = True, ignore = None):
    if not os.path.exists(dst):
        os.makedirs(dst)
        shutil.copystat(src, dst)
    lst = os.listdir(src)
    if ignore:
        excl = ignore(src, lst)
        lst = [x for x in lst if x not in excl]
    for item in lst:
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if symlinks and os.path.islink(s):
            if os.path.lexists(d):
                os.remove(d)
            os.symlink(os.readlink(s), d)
            try:
                st = os.lstat(s)
                mode = stat.S_IMODE(st.st_mode)
                os.lchmod(d, mode)
            except:
                pass # lchmod not available
        elif os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)

def move_android_source():
    if parsed_args.keep_source:
        shutil.copy("adjust/source/android/AdjustMarmalade.java", "source/android/AdjustMarmalade.bak.java")
        shutil.copy("adjust/source/android/AdjustMarmalade_platform.cpp", "source/android/AdjustMarmalade_platform.bak.cpp")

        shutil.copy("adjust/AdjustMarmalade.mkf","source/AdjustMarmalade.bak.mkf")

        shutil.copy("adjust/interface/AdjustMarmalade_interface.cpp", "source/interface/AdjustMarmalade_interface.bak.cpp")

        shutil.copy("adjust/source/generic/AdjustMarmalade.cpp","source/generic/AdjustMarmalade.bak.cpp")
        shutil.copy("adjust/source/h/AdjustMarmalade_internal.h","source/h/AdjustMarmalade_internal.bak.h")

    else:
        shutil.copy("source/AdjustMarmalade.mkf", "adjust")
        shutil.copy("source/AdjustMarmalade_build.mkf", "adjust")

        shutil.copy("source/generic/AdjustMarmalade.cpp","adjust/source/generic/AdjustMarmalade.cpp")
        shutil.copy("source/h/AdjustMarmalade_internal.h","adjust/source/h/AdjustMarmalade_internal.h")

        shutil.copy("source/android/AdjustMarmalade.java", "adjust/source/android")
        shutil.copy("source/android/AdjustMarmalade_platform.cpp", "adjust/source/android")

        shutil.copytree("source/rapidjson/include", "adjust/rapidjson/include")
        shutil.copy("source/rapidjson/rapidjson.mkf", "adjust/rapidjson/rapidjson.mkf")
        copytree("source/sdk/Android", "adjust/sdk/Android");

    Log("Copied android source")

def move_iphone_source():
    if parsed_args.keep_source:
        shutil.copy("adjust/source/iphone/AdjustMarmalade_platform.mm", "source/iphone/AdjustMarmalade_platform.bak.mm")

        shutil.copy("adjust/AdjustMarmalade.mkf","source/AdjustMarmalade.bak.mkf")
        shutil.copy("adjust/AdjustMarmalade_build.mkf", "source/AdjustMarmalade_build.bak.mkf")

        shutil.copy("adjust/interface/AdjustMarmalade_interface.cpp", "source/interface/AdjustMarmalade_interface.bak.cpp")

        shutil.copy("adjust/source/generic/AdjustMarmalade.cpp","source/generic/AdjustMarmalade.bak.cpp")
        shutil.copy("adjust/source/h/AdjustMarmalade_internal.h","source/h/AdjustMarmalade_internal.bak.h")
    else:
        shutil.copy("source/AdjustMarmalade.mkf", "adjust")
        shutil.copy("source/AdjustMarmalade_build.mkf", "adjust")

        shutil.copy("source/generic/AdjustMarmalade.cpp","adjust/source/generic/AdjustMarmalade.cpp")
        shutil.copy("source/h/AdjustMarmalade_internal.h","adjust/source/h/AdjustMarmalade_internal.h")

        shutil.copy("source/iphone/AdjustMarmalade_platform.h", "adjust/source/iphone")
        shutil.copy("source/iphone/AdjustMarmalade_platform.mm", "adjust/source/iphone")

        copytree("source/rapidjson/include", "adjust/rapidjson/include")
        shutil.copy("source/rapidjson/rapidjson.mkf", "adjust/rapidjson/rapidjson.mkf")
        copytree("source/sdk/iOS", "adjust/sdk/iOS");

    Log("Copied iphone source")

def post_compile():
    shutil.copy("source/interface/AdjustMarmalade_interface.cpp", "adjust/interface/AdjustMarmalade_interface.cpp")

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
                mkb_file_lines.append("librarypath \"sdk/Android/adjust-android.jar\"\n")
                Log("Pppend native SDK jar to android java mkb")
    with open(mkb_filename, "w+") as f:
        f.writelines(mkb_file_lines)

def edit_mkf(platform):
    mkf_file_lines = []
    mkf_filename = "adjust/AdjustMarmalade.mkf"

    iphone_links_opts = re.compile("#iphone-link-opts")
    external_jar_re = re.compile("android-external-jar")
    with open(mkf_filename) as f:
        for line in f:
            mkf_file_lines.append(line)
            if platform == "iphone" and iphone_links_opts.search(line):
                mkf_file_lines.append("    iphone-link-opts=\"-weak_framework iAd\" -weak_framework AdSupport\"\n")
                Log("append iphone weak frameworks to mkf")
            if platform == "android" and external_jar_re.search(line):
                mkf_file_lines.append("    android-external-jars=\"incoming/adjust.jar\"\n")
                Log("append android external jars to mkf")
    with open(mkf_filename, "w+") as f:
        f.writelines(mkf_file_lines)

def edit_build_iphone_mkf():
    mkf_build_file_lines = []
    mkf_build_filename = "adjust/AdjustMarmalade_build.mkf"

    iphone_links_opts = re.compile("I3D_OS_IPHONE")
    with open(mkf_build_filename) as f:
        for line in f:
            mkf_build_file_lines.append(line)

    with open(mkf_build_filename, "w+") as f:
        f.writelines(mkf_build_file_lines)

def edit_xcode_project(xcode_project_path):
    proc = Popen(["xcodebuild", "-version", "-sdk"], stdout=PIPE, stderr=PIPE)
    out, err = proc.communicate()
    
    if proc.returncode not in [0, 66]:
        Log("Could not retrieve Xcode sdk path. code: {0}, err: {1}", proc.returncode, err)
        return None

    match = re.search("iPhoneOS.*?Path: (?P<sdk_path>.*?)\n", out, re.DOTALL)
    xcode_sdk_path = match.group('sdk_path') if match else None
    Log("XCode SDK path: {0}", xcode_sdk_path)

    framework_path = xcode_sdk_path + "/System/Library/Frameworks/"
    Log("Framework path: {0}", framework_path)

    # Load iOS pbxproj project file.
    ios_XcodeProject = XcodeProject.Load(xcode_project_path)
    
    # Add adSupport framework.
    ios_XcodeProject.add_file_if_doesnt_exist(framework_path + "AdSupport.framework", tree="SDKROOT", create_build_files=True, weak=True)
    Log("Added adSupport framework")

    # Add iAd framework.
    ios_XcodeProject.add_file_if_doesnt_exist(framework_path + "iAd.framework", tree="SDKROOT", create_build_files=True, weak=True)
    Log("Added iAd framework")

    # Add Adjust framework.
    adjust_framework_path = os.path.dirname(os.path.abspath(__file__)) + "/adjust/sdk/ios/"
    ios_XcodeProject.add_file_if_doesnt_exist(adjust_framework_path + "AdjustSdk.framework", tree="SDKROOT", create_build_files=True, weak=True)

    # Save changes.
    ios_XcodeProject.saveFormat3_2()

if __name__ == "__main__":
    main()
