cc_library(
    name = "p4buf",
    srcs = ["p4buf.cc"],
    hdrs = ["p4buf.h"],
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
