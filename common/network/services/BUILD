package(default_visibility = ["//visibility:public"])

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = ["//utils/containers/queues:concurrent_queue_packet"],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.cc"],
    deps = [
        "//common/network/services:tcp_receiver",
        "//common/network/services:udp_receiver",
    ],
)

cc_library(
    name = "udp_receiver",
    srcs = ["udp_receiver.cc"],
    hdrs = ["udp_receiver.hpp"],
    deps = [
        "//ciri:node_shared",
        "//common/network:neighbor",
        "//common/network/components:receiver_shared",
        "//common/network/services:receiver_shared",
        "//utils:logger_helper",
        "@boost//:asio",
    ],
)

cc_library(
    name = "tcp_receiver",
    srcs = ["tcp_receiver.cc"],
    hdrs = ["tcp_receiver.hpp"],
    deps = [
        "//ciri:node_shared",
        "//common/network:neighbor",
        "//common/network/components:receiver_shared",
        "//common/network/services:receiver_shared",
        "//utils:logger_helper",
        "@boost//:asio",
    ],
)

cc_library(
    name = "tcp_sender",
    srcs = ["tcp_sender.cc"],
    hdrs = ["tcp_sender.hpp"],
    deps = [
        "//common/network:endpoint",
        "//common/trinary:trit_array",
        "//utils:logger_helper",
        "@boost//:asio",
    ],
)

cc_library(
    name = "udp_sender",
    srcs = ["udp_sender.cc"],
    hdrs = ["udp_sender.hpp"],
    deps = [
        "//common/network:endpoint",
        "//common/trinary:trit_array",
        "//utils:logger_helper",
        "@boost//:asio",
    ],
)
