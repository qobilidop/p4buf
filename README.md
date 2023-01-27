# P4 Buffers

## Example

See the [basic example](examples/basic/):

```cpp
#include <bitset>
#include <iostream>

#include "p4buf/p4data.h"

using namespace p4buf;

void PrintBuffer(const p4buf::Buffer& buffer) {
  std::cout << "bytes:";
  for (std::size_t i = 0; i < buffer.size(); ++i) {
    std::cout << " " << std::bitset<8>(std::to_integer<int>(buffer.at(i)));
  }
  std::cout << "\n";
}

int main(int argc, char* argv[]) {
  // Specify a P4 type.
  // clang-format off
  P4Type p4type(P4StructT{  // struct {
    {"s0", P4StructT{       //   struct {
      {"a", P4BitT{1}},     //     bit<1> a;
      {"b", P4BitT{2}},     //     bit<2> b;
    }},                     //   } s0;
    {"c", P4BitT{3}},       //   bit<3> c;
    {"d", P4BitT{4}},       //   bit<4> d;
  });                       // }
  // clang-format on

  // Create a P4 data object with the specified type.
  // Initialize all bytes to 0.
  P4Data p4data(p4type, 0);

  // Edit named fields.
  p4data["/s0/a"] = uint8_t{1};
  p4data["/s0/b"] = uint8_t{1};
  p4data["/c"] = uint8_t{1};
  p4data["/d"] = p4data["/c"];

  // Now the underlying buffer becomes:
  // 10100100 01000000
  // abbcccdd dd
  PrintBuffer(*p4data.buffer());

  return 0;
}
```

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
