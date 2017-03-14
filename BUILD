genrule(
		name = "mpi_compile",
		srcs = ["src/mpi.cc"],
		outs = ["mpi"],
		cmd = "mpic++ -L/usr/lib -lgflags -lglog -lprotobuf \
				-I/home/lionell/dev/projects/pagerank -Ibazel-out/host/genfiles -o $@ $< \
				bazel-out/host/bin/lib/libpagerank.a bazel-out/host/bin/lib/libio.a \
				bazel-out/host/bin/lib/libmath.a bazel-out/host/bin/lib/libbenchmark.a \
				bazel-out/host/bin/proto/libpage_proto.a bazel-out/host/bin/proto/libchunk_proto.a \
				bazel-out/host/bin/proto/libmetadata_proto.a",
		tools = [
				"//proto:page_cc_proto",
				"//proto:chunk_cc_proto",
				"//proto:metadata_cc_proto",
				"//lib:pagerank",
				"//lib:io",
				"//lib:math",
				"//lib:benchmark",
		],
		executable = 1,
		output_to_bindir = 1,
)

genrule(
		name = "mpi_run",
		outs = ["mpi.sh"],
		cmd = "echo '#!/usr/bin/env sh' > $@; \
				echo 'mpirun -np 3 $(location :mpi_compile) $$@' >> $@",
		tools = [
				":mpi_compile",
		],
		executable = 1,
		output_to_bindir = 1,
)

cc_binary(
		name = "omp",
		srcs = ["src/omp.cc"],
		deps = [
				"//lib:pagerank",
				"//lib:io",
				"//lib:math",
				"//lib:benchmark",
		],
		copts = [
				"-fopenmp",
		],
		linkopts = [
				"-fopenmp",
				"-lgflags",
				"-lglog",
		],
)

cc_binary(
		name = "serial",
		srcs = ["src/serial.cc"],
		deps = [
				"//lib:pagerank",
				"//lib:io",
				"//lib:math",
				"//lib:benchmark",
		],
		linkopts = [
				"-lgflags",
				"-lglog",
		],
)
