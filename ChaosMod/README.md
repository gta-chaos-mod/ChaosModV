# ChaosMod ASI Project

This is where the source code for the actual mod resides in. Note that the compiled binary is simply a .dll file renamed to .asi for asiloader.

## Building

<details>
  <summary>CMake (CLI)</summary>
	
  1. `mkdir build`

  2. `cd build`

  3. `cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..`

  4. `ninja`
	
</details>

<details>
  <summary>CMake (Visual Studio)</summary>

  Make sure the `C++ CMake tools for Windows` component is installed through the Visual Studio Installer.

  1. Click on the `Switch between solutions and available views` button in the Solution Explorer (next to the home icon)

  2. Click on `CMake Targets View`

  3. Build the `ChaosMod Project` target

  The .asi should be located inside the `out/build/<Configuration>` folder.
	
</details>

## Coding conventions

- This project provides a custom .clang-format file for use with clang-format. Please make sure to format your source code before contributing, there should be support for just about every IDE / editor out there - either officially or using an extension.
- Always use brackets for if, for and while expressions.
- This project uses precompiled headers; make sure to add `#include <stdafx.h>` as the first line in source files. If you include other headers as well separate those two using an empty line to prevent clang-format from reshuffling the stdafx.h include.
- Naming:
    - Private member variables: `m_Foo`
    - Static global variables specific to a translation unit: `ms_Foo`
    - Global variables: `g_Foo`
    - Global member inside struct: `Foo`
