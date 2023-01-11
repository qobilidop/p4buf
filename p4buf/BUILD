cc_library(
    name = "p4buf",
    srcs = ["p4buf.cc"],
    hdrs = ["p4buf.h"],
    visibility = ["//visibility:public"],
    deps = ["@fmt"],
)

cc_test(
    name = "p4buf_test",
    size = "small",
    srcs = ["p4buf_test.cc"],
    deps = [
        ":p4buf",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "buffer",
    srcs = ["buffer.cc"],
    hdrs = ["buffer.h"],
    visibility = ["//visibility:public"],
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
