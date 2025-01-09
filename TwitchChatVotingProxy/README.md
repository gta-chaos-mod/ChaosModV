# ChaosMod TwitchChatVotingProxy Project

This is where the source code for the voting process resides in. Contrary to the name it supports multiple platforms now: Twitch and Discord.

## Building

Either through Visual Studio or using the `dotnet` CLI tool (`dotnet build` or `dotnet publish`).

## Coding conventions

- This project provides a custom .editorconfig file. Please make sure to format your source code before contributing, there should be support for just about every IDE / editor out there - either officially or using an extension. Alternatively you can run `dotnet format`.
- Always use brackets for if, for and while expressions.
- Naming:
    - Private member variables: `m_Foo`
    - Static global variables specific to file: `ms_Foo`
    - Global member: `Foo`