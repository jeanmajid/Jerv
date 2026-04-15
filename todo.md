# Todo

Priority ranked from top to bottom, high to low

## Ongoing Tasks

### fixes

- Check all the Varint calls and do 32 or 64 properly
- Use ReadInt

### Rewrites

- Move the stale connection checker to the main thread to not cause unnecessary issues
- Rewrite world gen from scratch without the stupid multithreading shenanigans (almost done)

### Api
- Command Registry
- Blocks
- Inventory
- Entities
- Design a real api interface
- Add scripting language support and c++ plugins

### Other

- Add client stuff to run a bedrock client
- Proxy capabilities
- Try out Nethernet stuff

## Completed Tasks

- Rewrite the Raknet from scratch
- Make the handlers runtime functions and just manually place themselves into the array via some function
- Make code cleaner and simpler