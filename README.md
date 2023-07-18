# Chaos Mod V

A replica of the chaos mods found on previous GTA games for GTA V.

See the [GTA5-Mods mod page](https://www.gta5-mods.com/scripts/chaos-mod-v-beta) for more information and instructions on how to install it.

Feel free to join the Discord Server for community support or simply to stay up to date on this (and other) mods.

Also make sure to check the [Wiki](https://github.com/gta-chaos-mod/ChaosModV/wiki)!

[![](https://discord.com/api/guilds/785656433529716757/widget.png)](https://discord.gg/w2tDeKVaF9)

## Building

### Preparation

1. Clone the repo `git clone https://github.com/gta-chaos-mod/ChaosModV.git`

2. `cd ChaosModV`

3. Initialize all submodules

```
git submodule init
git submodule update --recursive
```

Afterwards choose one of the following methods depending on the projects you want to build:

### ASI only

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

### All projects

<details>
  <summary>MSBuild (Visual Studio)</summary>

  NOTE: The methods above are preferred over this one for the ASI (the ChaosMod project). There may be components of the ASI only available to CMake due to complications with third-party libraries.

  1. Open `vendor/minhook/build/VC16/MinHookVC16.sln` in Visual Studio

  2. Compile libMinHook as x64 Release build

  3. Open `ChaosMod.sln` in the root folder in Visual Studio

  4. Compiling should work now. If there's an error referencing `MsBuildMajorVersion` when building either the ConfigApp or TwitchChatProxy projects, close and open Visual Studio again.
	
</details>
	
## Adding new effects

You can easily add and share your own effects using the integrated Lua scripting engine. See [here](https://github.com/gta-chaos-mod/ChaosModV/wiki/Lua-Scripting) for more information.

Otherwise, if you want to integrate your effect directly into the mod:

1. Create a new .cpp file in the appropriate folder under `ChaosMod/Effects/db/` with a fitting name

Layout of the file should look like this:

```cpp
/*
	Effect by <Your Name>
*/

#include <stdafx.h>

static void OnStart()
{
	
}

static void OnStop()
{
	
}

static void OnTick()
{
	
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Generic Effect",
		.Id = "player_funny_stuff"
	}
);
```

The project makes use of clang-format which will mess up the formatting of the list-initialization, thus it's necessary to exempt it using `// clang-format off`.

2. Add the same info to `ConfigApp/Effects.cs`
