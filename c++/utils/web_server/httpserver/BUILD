cc_library (
    name = "http_server_lib",
    srcs = ["http_server.cc",],
    hdrs = ["http_server.h",],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//common:mongoose",
    ],
)

cc_binary(
    name = "http_server",
    srcs = ["main.cc",],
    visibility = [
        "//visibility:public",
    ],
    deps = [":http_server_lib",],
)