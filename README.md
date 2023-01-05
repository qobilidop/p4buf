# P4 Buffers

## Dependencies

User:
- [Bazel](https://bazel.build) through [Bazelisk](https://bazel.build/install/bazelisk)
    - Bazel handles other dependencies automatically, including the development ones below.

Development:
- [GoogleTest](https://google.github.io/googletest)

## Usage

TODO

## Development

```bash
bazelisk build //src:p4buf
bazelisk test //src:p4buf_test
```

### Hedron's Compile Commands Extractor for Bazel

[This tool](https://github.com/hedronvision/bazel-compile-commands-extractor) (optional) integrates Bazel with common editors to enable [autocomplete and other great editor features](https://bazel.build/install/ide#c_language_family_c_c_objective-c_and_objective-c).

To keep tooling information up to date, run this command after every Bazel file change:

```bash
bazelisk run @hedron_compile_commands//:refresh_all
```

VS Code settings are already recorded in this repo. So it shall work automatically. For other editors, follow instructions [here](https://github.com/hedronvision/bazel-compile-commands-extractor#other-editors).
