package(default_visibility = ["//visibility:public"])

py_binary(
		name = "generate_dataset_old",
		srcs = ["generate_dataset_old.py"],
)

cc_binary(
		name = "generate_dataset",
		srcs = ["generate_dataset.cc"],
		deps = [
				"//proto:chunk_cc_proto",
				"//proto:metadata_cc_proto",
				"//lib:benchmark",
		],
		linkopts = [
				"-lgflags",
				"-lglog",
		],
)
