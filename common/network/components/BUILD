package(default_visibility = ["//visibility:public"])

cc_library(
    name = "broadcaster_shared",
    hdrs = ["broadcaster.h"],
    deps = [
        "//utils:logger_helper",
        "//utils/containers/queues:concurrent_queue_trit_array",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "broadcaster",
    srcs = ["broadcaster.c"],
    deps = [
        ":broadcaster_shared",
        "//ciri:node_shared",
        "//utils:logger_helper",
    ],
)

cc_library(
    name = "processor",
    srcs = ["processor.c"],
    hdrs = ["processor.h"],
    deps = [
        "//utils:logger_helper",
        "//utils/containers/queues:concurrent_queue_packet",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "responder",
    srcs = ["responder.c"],
    hdrs = ["responder.h"],
    deps = [
        "//utils:logger_helper",
        "//utils/containers/queues:concurrent_queue_hash_request",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "receiver_shared",
    hdrs = ["receiver.h"],
    deps = [
        "//common/network:iota_packet",
        "//common/network/services:receiver_shared",
        "//utils/handles:thread",
    ],
)

cc_library(
    name = "receiver",
    srcs = ["receiver.c"],
    deps = [
        ":receiver_shared",
        "//ciri:node_shared",
        "//common/network/services:receiver",
    ],
)

cc_library(
    name = "requester_shared",
    hdrs = ["requester.h"],
    deps = [
        "//utils/containers/lists:concurrent_list_trit_array",
    ],
)

cc_library(
    name = "requester",
    srcs = ["requester.c"],
    deps = [
        ":requester_shared",
        "//ciri:core_shared",
        "//utils:logger_helper",
    ],
)
