cc_library(
    name = "bit",
    srcs = ["bit.cc"],
    hdrs = ["bit.h"],
)

cc_library(
    name = "buffer",
    srcs = ["buffer.cc"],
    hdrs = ["buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":bit",
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
    name = "p4data",
    srcs = ["p4data.cc"],
    hdrs = ["p4data.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":buffer",
        ":utility",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "p4data_test",
    size = "small",
    srcs = ["p4data_test.cc"],
    deps = [
        ":p4data",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "utility",
    hdrs = ["utility.h"],
)
