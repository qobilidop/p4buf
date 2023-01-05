workspace(name = "p4buf")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",
    sha256 = "632cf245a06f38a02eefe9764ae5ca581b74bfa106a44067665fcd99fb6babb0",
    strip_prefix = "bazel-compile-commands-extractor-0197fc673a1a6035078ac7790318659d7442e27e",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/0197fc673a1a6035078ac7790318659d7442e27e.tar.gz",
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

# GoogleTest
# http://google.github.io/googletest/quickstart-bazel.html#set-up-a-bazel-workspace
# https://github.com/google/googletest/releases/tag/release-1.12.1
http_archive(
    name = "com_google_googletest",
    sha256 = "24564e3b712d3eb30ac9a85d92f7d720f60cc0173730ac166f27dda7fed76cb2",
    strip_prefix = "googletest-release-1.12.1",
    urls = ["https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip"],
)
