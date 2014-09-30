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
    parser.add_argument("--build_iphone", "-bi", action="store_true", help="Build iphone build")
    parser.add_argument("--compile_iphone", "-ci", action="store_true", help="Compile iphone build")
    parser.add_argument("--build_compile_iphone", "-bci", action="store_true", help="Build and compile iphone build")
    parser.add_argument("--post_iphone", "-pi", action="store_true", help="Post processing iphone build")

    global parsed_args
    parsed_args = parser.parse_args()
    
    with open('adjust/AdjustBuildLog.txt','w') as fileLog:
        # log function with file injected
        global Log
        Log = LogInput(fileLog)
        if parsed_args.post_iphone:
            post_iphone()
            return
        if not (parsed_args.compile_android or parsed_args.compile_iphone):
            clean()
        if not parsed_args.clean:
            if not (parsed_args.build_iphone or parsed_args.compile_iphone or parsed_args.build_compile_iphone):
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
    #edit_mkf("android")
    move_android_source()

def build_iphone():
    edk_build = Popen(["edk-build", "adjust/AdjustMarmalade.s4e", "--platform=iphone"],
            stdout=PIPE, stderr=PIPE)
    out, err = edk_build.communicate()

    Log("edk-build iphone out: {0}".format(out))
    if (edk_build.returncode not in [0]):
        print("edk-build iphone code: {0}, err: {1}".format(
            edk_build.returncode, err))
    
    #edit_mkf("iphone")
    #edit_build_iphone_mkf()
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

    edit_xcode_project("adjust/build_adjustmarmalade_iphone_iphone_xcode/AdjustMarmalade_iphone.xcodeproj/project.pbxproj")

def move_android_source():
    if parsed_args.keep_source:
        shutil.copy("adjust/source/android/AdjustMarmalade.java", "source/android/AdjustMarmalade.bak.java")
        shutil.copy("adjust/source/android/AdjustMarmalade_platform.cpp", "source/android/AdjustMarmalade_platform.bak.cpp")

        shutil.copy("adjust/AdjustMarmalade.mkf","source/AdjustMarmalade.bak.mkf")
    else:
        shutil.copy("source/AdjustMarmalade.mkf", "adjust")

        shutil.copy("source/android/AdjustMarmalade.java", "adjust/source/android")
        shutil.copy("source/android/AdjustMarmalade_platform.cpp", "adjust/source/android")
        shutil.copytree("source/rapidjson", "adjust/rapidjson")


    Log("copied android source")

def move_iphone_source():
    if parsed_args.keep_source:
        shutil.copy("adjust/source/iphone/AdjustMarmalade_platform.mm", "source/iphone/AdjustMarmalade_platform.bak.mm")

        shutil.copy("adjust/AdjustMarmalade.mkf","source/AdjustMarmalade.bak.mkf")
        shutil.copy("adjust/AdjustMarmalade_build.mkf", "source/AdjustMarmalade_build.bak.mkf")

        shutil.copy("adjust/interface/AdjustMarmalade_interface.cpp", "source/interface/AdjustMarmalade_interface.bak.cpp")
    else:
        shutil.copy("source/AdjustMarmalade.mkf", "adjust")
        shutil.copy("source/AdjustMarmalade_build.mkf", "adjust")

        shutil.copy("source/iphone/AdjustMarmalade_platform.mm", "adjust/source/iphone")
        shutil.copy("source/iphone/AdjustMarmalade_platform.h", "adjust/source/iphone")
        
        os.makedirs("adjust/source/iphone/Adjust/AIAdditions")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityHandler.m", "adjust/source/iphone/Adjust/AIActivityHandler.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityHandler.h", "adjust/source/iphone/Adjust/AIActivityHandler.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityKind.h",    "adjust/source/iphone/Adjust/AIActivityKind.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityKind.m",    "adjust/source/iphone/Adjust/AIActivityKind.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityPackage.h", "adjust/source/iphone/Adjust/AIActivityPackage.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityPackage.m", "adjust/source/iphone/Adjust/AIActivityPackage.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityState.h",   "adjust/source/iphone/Adjust/AIActivityState.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIActivityState.m",   "adjust/source/iphone/Adjust/AIActivityState.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdjustFactory.h",   "adjust/source/iphone/Adjust/AIAdjustFactory.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdjustFactory.m",   "adjust/source/iphone/Adjust/AIAdjustFactory.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AILogger.h",          "adjust/source/iphone/Adjust/AILogger.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AILogger.m",          "adjust/source/iphone/Adjust/AILogger.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIPackageBuilder.h",  "adjust/source/iphone/Adjust/AIPackageBuilder.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIPackageBuilder.m",  "adjust/source/iphone/Adjust/AIPackageBuilder.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIPackageHandler.h",  "adjust/source/iphone/Adjust/AIPackageHandler.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIPackageHandler.m",  "adjust/source/iphone/Adjust/AIPackageHandler.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIRequestHandler.h",  "adjust/source/iphone/Adjust/AIRequestHandler.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIRequestHandler.m",  "adjust/source/iphone/Adjust/AIRequestHandler.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIResponseData.h",    "adjust/source/iphone/Adjust/AIResponseData.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIResponseData.m",    "adjust/source/iphone/Adjust/AIResponseData.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AITimer.h",           "adjust/source/iphone/Adjust/AITimer.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AITimer.m",           "adjust/source/iphone/Adjust/AITimer.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIUtil.h",            "adjust/source/iphone/Adjust/AIUtil.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIUtil.m",            "adjust/source/iphone/Adjust/AIUtil.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/Adjust.h",            "adjust/source/iphone/Adjust/Adjust.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/Adjust.m",            "adjust/source/iphone/Adjust/Adjust.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdditions/NSData+AIAdditions.m", "adjust/source/iphone/Adjust/AIAdditions/NSDataAIAdditions.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdditions/NSData+AIAdditions.h", "adjust/source/iphone/Adjust/AIAdditions/NSData+AIAdditions.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdditions/NSString+AIAdditions.m", "adjust/source/iphone/Adjust/AIAdditions/NSStringAIAdditions.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdditions/NSString+AIAdditions.h", "adjust/source/iphone/Adjust/AIAdditions/NSString+AIAdditions.h")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdditions/UIDevice+AIAdditions.m", "adjust/source/iphone/Adjust/AIAdditions/UIDeviceAIAdditions.m")
        shutil.copy("source/iphone/ios_sdk/Adjust/AIAdditions/UIDevice+AIAdditions.h", "adjust/source/iphone/Adjust/AIAdditions/UIDevice+AIAdditions.h")
    Log("copied iphone source")

def post_iphone():
    shutil.copy("source/interface/AdjustMarmalade_interface.cpp", "adjust/interface/AdjustMarmalade_interface.cpp")

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
    Log("Xcode sdk path: {0}", xcode_sdk_path)

    framework_path = xcode_sdk_path + "/System/Library/Frameworks/"
    Log("framework path: {0}", framework_path)

    # load iOS pbxproj project file
    ios_XcodeProject = XcodeProject.Load(xcode_project_path)
    
    # add adSupport framework
    ios_XcodeProject.add_file_if_doesnt_exist(framework_path + "AdSupport.framework", tree="SDKROOT", create_build_files=True,weak=True)
    Log("added adSupport framework")

    # add iAd framework
    ios_XcodeProject.add_file_if_doesnt_exist(framework_path + "iAd.framework", tree="SDKROOT", create_build_files=True,weak=True)
    Log("added iAd framework")

    # regex for adjust sdk files
    re_adjust_files = re.compile(r"AI.*\.m|.*AIAdditions.*\.m|Adjust\.m|AdjustMarmalade_platform\.mm")

    # iterate all objects in the Xcode iOS project file
    for key in ios_XcodeProject.get_ids():
        obj = ios_XcodeProject.get_obj(key)
        
        name = obj.get('name')
        isa = obj.get('isa')
        path = obj.get('path')
        fileref = obj.get('fileRef')

        #Log("key: {0}, name: {1}, isa: {2}, path: {3}, fileref: {4}", key, name, isa, path, fileref)

        #check if file reference match any adjust file
        adjust_file_match = re_adjust_files.match(name if name else "")
        if (adjust_file_match):
            #Log("file match, group: {0}", adjust_file_match.group())
            # get the build file, from the file reference id
            build_files = ios_XcodeProject.get_build_files(key)
            for build_file in build_files:
                # add the ARC compiler flag to the adjust file if doesn't exist
                build_file.add_compiler_flag('-fobjc-arc')
                Log("added ARC flag to file {0}", name)

    # save changes
    ios_XcodeProject.saveFormat3_2()

if __name__ == "__main__":
    main()
