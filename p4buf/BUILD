cc_library(
    name = "buffer",
    srcs = ["buffer.cc"],
    hdrs = ["buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":type_spec",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "buffer_test",
    size = "small",
    srcs = ["buffer_test.cc"],
    deps = [
        ":buffer",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "type_spec",
    srcs = ["type_spec.cc"],
    hdrs = ["type_spec.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":utility",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "type_spec_test",
    size = "small",
    srcs = ["type_spec_test.cc"],
    deps = [
        ":type_spec",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "utility",
    hdrs = ["utility.h"],
)
