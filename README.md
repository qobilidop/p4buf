# P4 Buffers

Hardware-friendly P4 runtime data representation.

## Usage

TODO

## Development

### Dependencies

- [Bazel](https://bazel.build) through [Bazelisk](https://bazel.build/install/bazelisk)
    - Bazel handles other dependencies automatically.
- [GoogleTest](https://google.github.io/googletest)

#### VS Code

If you use VS Code, we recommend also using [dev container](https://code.visualstudio.com/docs/devcontainers/containers) for a convenient reproducible development environment. All the dependecies and other useful tools are already [configured to be installed](.devcontainer/Dockerfile) in the container.

To build and enter the container for the first time, use the following command through the [Command Palette](https://code.visualstudio.com/docs/getstarted/userinterface#_command-palette):

```
Dev Containers: Rebuild and Reopen in Container
```

Use other [commands](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers#available-commands) as you see fit.

### Bazel Commands

```bash
bazelisk build //...  # Build all targets
bazelisk test //...  # Test all targets
```

### Hedron's Compile Commands Extractor for Bazel

[This tool](https://github.com/hedronvision/bazel-compile-commands-extractor) integrates Bazel with common editors to enable [autocomplete and other useful editor features](https://bazel.build/install/ide#c_language_family_c_c_objective-c_and_objective-c). If you use VS Code, we have already included the required [workspace settings](.vscode/settings.json) in this repo. Otherwise, follow [these instructions](https://github.com/hedronvision/bazel-compile-commands-extractor#other-editors) to set up your editor for this integration.

To keep tooling information up to date, run this command after any Bazel file change:

```bash
bazelisk run @hedron_compile_commands//:refresh_all
```
