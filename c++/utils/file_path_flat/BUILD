
cc_binary(
    name = "boost_filesystem_test",
    deps = [":boost_file_test"],
)

cc_library(
    name = "boost_file_test",
    hdrs = ["filesys.h"],
    srcs = ["filesys.cc"],
    deps = ["@third_party_bazel//:boost"],
    visibility = ["//visibility:public",],
)
