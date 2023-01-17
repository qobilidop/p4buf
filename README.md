# P4 Buffers

Hardware-friendly P4 runtime data representation.

## Example

See the [basic example](examples/basic/):

```cpp
#include <memory>

#include "p4buf/buffer.h"
#include "p4buf/type_spec.h"

int main(int argc, char *argv[]) {
  // Create a buffer of 2 bytes, with all bytes initialized to 0.
  // Internally a buffer is represented as a flat byte array.
  auto buffer = std::make_shared<p4buf::Buffer>(2, 0);

  // Define some bit fields.
  // 00000000 00000000
  // abbbcccc cddddddd
  p4buf::BufferEditor editor;
  editor.field_spec["a"] = {/* offset */ 0, /* width */ 1};
  editor.field_spec["b"] = {1, 3};
  editor.field_spec["c"] = {4, 5};
  editor.field_spec["d"] = {9, 7};

  // Set to edit the buffer.
  editor.Edit(buffer);

  // Set every bit fields to 1.
  editor["a"] = 1;
  editor["b"] = 1;
  editor["c"] = 1;
  editor["d"] = 1;
  // The buffer becomes:
  // 10010000 10000001
  // abbbcccc cddddddd
  buffer->print();

  // Instead of defining bit fields manually, we could define a data type spec
  // instead. For example we can express the following P4 data type:
  //
  // struct s0_t {
  //     bit<1> a;
  //     bit<3> b;
  // }
  //
  // struct s1_t {
  //     s0_t s0;
  //     bit<5> c;
  //     bit<7> d;
  // }
  p4buf::StructTypeSpec s0_t;
  s0_t.add("a", 1);
  s0_t.add("b", 3);
  p4buf::StructTypeSpec s1_t;
  s1_t.add("s0", s0_t);
  s1_t.add("c", 5);
  s1_t.add("d", 7);

  // Then we can create an editor from this type spec.
  p4buf::BufferEditor editor_from_type(&s1_t);

  // Bit fields are already defined and editable.
  editor_from_type.Edit(buffer);
  editor_from_type["/s0/a"] = 0b1;
  editor_from_type["/s0/b"] = 0b10;
  editor_from_type["/c"] = 0b100;
  editor_from_type["/d"] = 0b1000;
  // The buffer becomes:
  // 10100010 00001000
  // abbbcccc cddddddd
  buffer->print();

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
