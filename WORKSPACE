workspace(name = "p4buf")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Abseil
# https://abseil.io/docs/cpp/quickstart.html
# https://github.com/abseil/abseil-cpp (use the latest commit)
http_archive(
    name = "com_google_absl",
    sha256 = "bc4d3d9a82a5ce56c0837f406e60fdbc7139ed4032217e61ccec4dcae8c7ecd5",
    strip_prefix = "abseil-cpp-4b34e19765c8c03e4066f2226f7fb28d68604a1d",
    urls = ["https://github.com/abseil/abseil-cpp/archive/4b34e19765c8c03e4066f2226f7fb28d68604a1d.tar.gz"],
)

# Skylib
# https://github.com/bazelbuild/bazel-skylib/releases (use the latest release)
http_archive(
    name = "bazel_skylib",
    sha256 = "74d544d96f4a5bb630d465ca8bbcfe231e3594e5aae57e1edbf17a6eb3ca2506",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# GoogleTest
# http://google.github.io/googletest/quickstart-bazel.html#set-up-a-bazel-workspace
# https://github.com/google/googletest/releases (use the latest release)
http_archive(
    name = "com_google_googletest",
    sha256 = "81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2",
    strip_prefix = "googletest-release-1.12.1",
    urls = ["https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz"],
)

# fmt
# https://github.com/fmtlib/fmt/tree/master/support/bazel
git_repository(
    name = "fmt",
    branch = "master",
    patch_cmds = [
        "mv support/bazel/.bazelrc .bazelrc",
        "mv support/bazel/.bazelversion .bazelversion",
        "mv support/bazel/BUILD.bazel BUILD.bazel",
        "mv support/bazel/WORKSPACE.bazel WORKSPACE.bazel",
    ],
    patch_cmds_win = [
        "Move-Item -Path support/bazel/.bazelrc -Destination .bazelrc",
        "Move-Item -Path support/bazel/.bazelversion -Destination .bazelversion",
        "Move-Item -Path support/bazel/BUILD.bazel -Destination BUILD.bazel",
        "Move-Item -Path support/bazel/WORKSPACE.bazel -Destination WORKSPACE.bazel",
    ],
    remote = "https://github.com/fmtlib/fmt",
)

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor (use the latest commit)
http_archive(
    name = "hedron_compile_commands",
    sha256 = "632cf245a06f38a02eefe9764ae5ca581b74bfa106a44067665fcd99fb6babb0",
    strip_prefix = "bazel-compile-commands-extractor-0197fc673a1a6035078ac7790318659d7442e27e",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/0197fc673a1a6035078ac7790318659d7442e27e.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()
