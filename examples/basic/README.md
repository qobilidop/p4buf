# P4 Buffers Basic Example

```console
$ bazelisk run :main
...
INFO: Running command line: bazel-bin/main
P4 data type spec:
struct {
    struct {
        bit<1> a;
        bit<2> b;
    } s0;
    bit<3> c;
    bit<4> d;
}

Now let's create some buffers and set all fields to 1.

layout: compact
bytewidth: 2
fields:
    /s0/a: { byte_offset: 0, bit_offset: 0, bitwidth: 1 }
    /s0/b: { byte_offset: 0, bit_offset: 1, bitwidth: 2 }
    /c: { byte_offset: 0, bit_offset: 3, bitwidth: 3 }
    /d: { byte_offset: 0, bit_offset: 6, bitwidth: 4 }

layout: byte aligned
bytewidth: 4
fields:
    /s0/a: { byte_offset: 0, bit_offset: 0, bitwidth: 1 }
    /s0/b: { byte_offset: 1, bit_offset: 0, bitwidth: 2 }
    /c: { byte_offset: 2, bit_offset: 0, bitwidth: 3 }
    /d: { byte_offset: 3, bit_offset: 0, bitwidth: 4 }
```
