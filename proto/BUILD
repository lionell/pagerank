package(default_visibility = ["//visibility:public"])

cc_proto_library(
		name = "chunk_cc_proto",
		deps = [":chunk_proto"],
)

cc_proto_library(
		name = "page_cc_proto",
		deps = [":page_proto"],
)

cc_proto_library(
		name = "metadata_cc_proto",
		deps = [":metadata_proto"],
)

proto_library(
		name = "chunk_proto",
		srcs = ["chunk.proto"],
		deps = [":page_proto"],
)

proto_library(
		name = "page_proto",
		srcs = ["page.proto"],
)

proto_library(
		name = "metadata_proto",
		srcs = ["metadata.proto"],
)
