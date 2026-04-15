# Jerv Minecraft Bedrock Server Software

A Minecraft Bedrock Server Sofware (WIP), basics like chunk sending already work.

This is the third iteration of the project.

## How to build

I personally use Clion and g++ and it works pretty well, just make sure you select your own local version g++ under Settings/Build, Execution, Deployment/Toolchain.

If you want to build via terminal use:

```sh
cmake --preset default

cmake --build --preset <debug | release>
```

### Todo

Check out the [todo.md](todo.md) for a list of things that still need to be done.

## Thanks

Thank you to [Carolina](https://github.com/bedrock-apis/carolina), which this projects Raknet and structure is heavily based on and thanks to [Serenity](https://github.com/SerenityJS/serenity) and [Cloudburst](https://github.com/CloudburstMC/Protocol) for lots of infos.
