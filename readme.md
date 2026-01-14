# Jerv Minecraft Bedrock Server Software

A little Server Software im working on to just play around and see how things work. The future will tell how far it will go, but im aiming for good perfomance. The current state of the project is definetly messy in some places and needs refactoring, main reason for it being that I didn't care that much and just wanted to see stuff happen ingame.

This is the second iteration of the project, the first got thrown into the trash.

## How to build

I personally use Clion and g++ and it works pretty well, just make sure you select your own local version g++ under Settings/Build, Execution, Deployment/Toolchain.

If you want to build via terminal use:

```sh
cmake --preset default

cmake --build --preset <debug | release>
```

## Note

As this is a very buggy server software, probably violating alot of stuff, it crashes if you run it on linux. It also crashes when running with the Debugger, but Windows being Windows, it handles this buggy code with no problem and just runs it. Also do not try joining with multiple people, in its current state, the world gen will blow up. This is a single player server software.

### Todo

Check out the [Todo.md](Todo.md) for a list of things that still need to be done.

## Thanks

Thank you to [Carolina](https://github.com/bedrock-apis/carolina), which this projects Raknet and structure is heavily based on and thanks to [Serenity](https://github.com/SerenityJS/serenity) and [Cloudburst](https://github.com/CloudburstMC/Protocol) for lots of infos.
