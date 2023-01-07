cc_library(
    name = "buffer",
    srcs = ["buffer.cc"],
    hdrs = [
        "buffer.h",
        "common.h",
    ],
)

cc_test(
    name = "buffer_test",
    size = "small",
    srcs = ["buffer_test.cc"],
    deps = [
        ":buffer",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "type_spec",
    srcs = ["type_spec.cc"],
    hdrs = [
        "common.h",
        "type_spec.h",
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
