# Chaos Mod V

A replica of the chaos mod found on previous GTA games for GTA V.

See the [GTA5-Mods mod page](https://www.gta5-mods.com/scripts/chaos-mod-v-beta) for more information and instructions on how to install it.

## Building

1. Clone the repo `git clone https://github.com/gta-chaos-mod/ChaosModV.git`

2. `cd ChaosModV`

3. Initialize all submodules

```
git submodule init
git submodule update --recursive
```

4. Open `vendor/minhook/build/VC15/MinHookVC15.sln` in Visual Studio

5. Compile libMinHook as x64 Release build

6. Open `ChaosMod.sln` in the root folder in Visual Studio

7. Compiling should work now. If there's an error referencing `MsBuildMajorVersion` when building either the ConfigApp or TwitchChatProxy projects, close and open Visual Studio again.