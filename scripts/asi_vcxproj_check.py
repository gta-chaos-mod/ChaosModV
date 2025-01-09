import os
import sys

BLACKLISTED_SUBDIRS = ( "x64" ".cache" "build" )

if len(sys.argv) < 3:
    print("Need source directory and file as parameter!")
    exit(1)

SOURCE_DIR = sys.argv[1]
PROJECT_FILE = sys.argv[2]

source_files = []
project_files = []
is_successful = True

for subdir, dirs, files in os.walk(SOURCE_DIR, topdown = True):
    subdir = "" if subdir == SOURCE_DIR else (subdir[len(SOURCE_DIR):].replace("/", "\\") + "\\")

    for file in files:
        if file.endswith(".cpp") or file.endswith(".h"):
            source_files.append(f"{subdir}{file}")

    dirs[:] = [dir for dir in dirs if not dir in BLACKLISTED_SUBDIRS]

with open(SOURCE_DIR + PROJECT_FILE, "r") as _in:
    for line in _in:
        if "<ClCompile " in line or "<ClInclude " in line:
            file = line.split("Include=\"")[1].split("\"")[0]
            if not file.startswith(".."):
                project_files.append(file)

for file in source_files:
    if file not in project_files:
        is_successful = False
        print(f"File {file} missing in {PROJECT_FILE}!")

exit(0 if is_successful else 1)
