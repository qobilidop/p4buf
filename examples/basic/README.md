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
```
