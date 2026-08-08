# Jerv Minecraft Bedrock Server Software

A Minecraft Bedrock Server Sofware (WIP), basics like chunk sending already work.

This is the third iteration of the project.

The code is currently non existent, im working on the rewrite mentioned below. For the old code check the [legacy branch](https://github.com/jeanmajid/Jerv/tree/legacy)

## How to build

I personally use Clion and g++ and it works pretty well, just make sure you select your own local version g++ under Settings/Build, Execution, Deployment/Toolchain.

If you want to build via terminal use:

```sh
cmake --preset default

cmake --build --preset <debug | release>
```

### Todo

Check out the [todo.md](todo.md) for a list of things that still need to be done.
