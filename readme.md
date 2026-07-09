# Jerv Minecraft Bedrock Server Software

A Minecraft Bedrock Server Sofware (WIP), basics like chunk sending already work.

This is the third iteration of the project.

## Current State

Im not very happy with the code as their still some code snippets which are AI code.
I am not the biggest AI hater, I've used it before, but I don't want to use it for any projects which are important to me.
I will be rewriting quite a bit again, probably with an similar structure tho, as the structure and quite a bit of the code is from me (80%).

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
