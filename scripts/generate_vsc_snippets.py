# Generates lua native snippets for Visual Studio Code
# Move the generated lua.json file to:
# Windows: %APPDATA%/Code/User/snippets/lua.json
# Linux: ~/.config/Code/User/snippets/lua.json

import json
import urllib.request

NATIVE_URLS = {
    "https://raw.githubusercontent.com/alloc8or/gta5-nativedb-data/master/natives.json", # Thanks to alloc8or for the natives.json
    "https://gopong.dev/chaos/natives.json"
}

out_data = {}

def parse_native(native_hash, native_data):
    native_name = native_data["name"]
    print(native_name)

    args = native_data["params"]
    argc = len(args)
    args_prefix = ""
    args_body = ""
    for i in range(argc):
        arg = args[i]
        argName = arg["name"]
        args_prefix += arg["type"] + " " + argName
        args_body += "${" + str(argc-i) + ":" + argName + "}"
        if i < argc - 1:
            args_prefix += ", "
            args_body += ", "

    native_comment = native_data["comment"]
    if native_comment:
        native_comment = "\n\n" + native_comment

    out_data[native_hash] = {
        "prefix": native_name + "(" + args_prefix + ")",
        "body": native_name + "(" + args_body + ")",
        "description": "Returns: " + native_data["return_type"] + native_comment
    }


def parse_natives(url):
    try:
        result = urllib.request.urlopen(url).read()
    except urllib.error.URLError:
        print(f"Error while fetching {url}, aborting!")
        exit()

    json_in = json.loads(result)

    for _, native_namespace in json_in.items():
        for native_hash, native_data in native_namespace.items():
            parse_native(native_hash, native_data)

for url in NATIVE_URLS:
    parse_natives(url)

with open("lua.json", "w") as _out:
    json.dump(out_data, _out)
