package(default_visibility = ["//visibility:public"])

cc_library(
		name = "pagerank",
		srcs = ["pagerank.cc"],
		hdrs = ["pagerank.h"],
		deps = [
				":math",
				"//proto:page_cc_proto",
		],
)

cc_library(
		name = "io",
		srcs = ["io.cc"],
		hdrs = ["io.h"],
		deps = [
				"//proto:chunk_cc_proto",
				"//proto:metadata_cc_proto",
		],
)

cc_library(
		name = "math",
		srcs = ["math.cc"],
		hdrs = ["math.h"],
)

cc_library(
		name = "benchmark",
		srcs = ["benchmark.cc"],
		hdrs = ["benchmark.h"],
)
