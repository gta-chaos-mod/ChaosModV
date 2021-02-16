# Generates native definitions for lua scripts
# Move the generated natives_def.lua file into your chaosmod folder

import json
import urllib.request

def parse_native(native_hash, native_data, _out):
    return_type = native_data["return_type"]

    native_name = native_data["name"]

    print(native_name)

    args = []
    for arg in native_data["params"]:
        argName = arg["name"]

        if argName == "end" or argName == "repeat":
            argName = "_" + argName
        
        args.append(argName)

    _out.write("function " + native_name + "(")
    for i in range(len(args)):
        _out.write(args[i])
        if i < len(args) - 1:
            _out.write(",")
    _out.write(")\n")

    target_type = "_"
    if return_type == "BOOL":
        target_type = "_b"
    elif (return_type == "int" or return_type == "Entity" or return_type == "Ped" or return_type == "Vehicle" or return_type == "Object"
        or return_type == "Hash" or return_type == "Pickup" or return_type == "Blip" or return_type == "Interior"):
        target_type = "_i"
    elif return_type == "float":
        target_type = "_f"
    elif return_type == "const char*" or return_type == "char*":
        target_type = "_s"
    elif return_type == "Vector3":
        target_type = "_v"

    _out.write("   ")
    if target_type != "_":
        _out.write("return ")
    _out.write("_invoke(" + native_hash + "," + target_type)

    if len(args) > 0:
        _out.write(",")
        for i in range(len(args)):
            _out.write(args[i])
            if i < len(args) - 1:
                _out.write(",")
    _out.write(")\nend\n\n")

# Thanks to alloc8or for the natives.json
try:
    result = urllib.request.urlopen("https://raw.githubusercontent.com/alloc8or/gta5-nativedb-data/master/natives.json").read()
except urllib.error.URLError:
    print("Error while fetching natives.json, aborting!")
    exit()

json_in = json.loads(result)

with open("natives_def.lua", "w") as _out:
    _out.write("local _,_b,_i,_f,_s,_v=ReturnType.None,ReturnType.Boolean,ReturnType.Integer,ReturnType.Float,ReturnType.String,ReturnType.Vector3\n\n")

    for _, native_namespace in json_in.items():
        for native_hash, native_data in native_namespace.items():
            parse_native(native_hash, native_data, _out)