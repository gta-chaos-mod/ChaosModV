# ChaosMod ASI Project

This is where the source code for the actual mod resides in. Note that the compiled binary is simply a .dll file renamed to .asi for asiloader.

## Building

See the README.md situated in the project root.

## Coding conventions

- This project provides a custom .clang-format file for use with clang-format. Please make sure to format your source code before contributing, there should be support for just about every IDE / editor out there - either officially or using an extension.
- Always use brackets for if, for and while expressions.
- This project uses precompiled headers; make sure to add `#include <stdafx.h>` as the first line in source files. If you include other headers as well separate those two using an empty line to prevent clang-format from reshuffling the stdafx.h include.
- Naming:
    - Private member variables: `m_Foo`
    - Static global variables specific to a translation unit: `ms_Foo`
    - Global variables: `g_Foo`