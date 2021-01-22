def parse_line(line, _out):
    if not "static " in line or len(line.split("//")[0].strip()) == 0:
        return
    
    print(line)

    split = line.split("static ")[1].split(" ")
    return_type = split[0]

    split = line.split("(")[0].split(" ")
    native_name = split[len(split) - 1]

    split = line.split(">(")[1].split(");")[0].split(", ")

    hash = split[0]
    args = split[1:]
    for i in range(len(args)):
        if args[i] == "end" or args[i] == "repeat":
            args[i] = "_" + args[i]

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
    elif return_type == "const char*":
        target_type = "_s"
    elif return_type == "Vector3":
        target_type = "_v"

    _out.write("   ")
    if target_type != "_":
        _out.write("return ")
    _out.write("_invoke(" + hash + "," + target_type)

    if len(args) > 0:
        _out.write(",")
        for i in range(len(args)):
            _out.write(args[i])
            if i < len(args) - 1:
                _out.write(",")
    _out.write(")\nend\n\n")

with open("../vendor/scripthookv/inc/natives.h", "r") as _in:
    with open("natives_def.lua", "w") as _out:
        _out.write("local _,_b,_i,_f,_s,_v=ReturnType.None,ReturnType.Boolean,ReturnType.Integer,ReturnType.Float,ReturnType.String,ReturnType.Vector3\n\n")

        for line in _in:
            parse_line(line, _out)