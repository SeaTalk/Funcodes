cc_library(
    name = "http_client_lib",
    srcs = ["http_client.cc",],
    hrds = ["http_client.h",],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//common:mongoose",
    ],
)

cc_binary(
    name = "http_client",
    srcs = ["main.cc",],
    visibility = ["//visibility:public",],
    deps = [":http_client_lib",],
)