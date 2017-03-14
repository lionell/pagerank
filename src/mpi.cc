/*
 * Usage: bazel run :mpi -- \
 *          --dataset $(pwd)/data/generated/100k_10k \
 *          --output $(pwd)/out/mpi
 */

#include <iomanip>

#include <mpi.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "lib/io.h"
#include "lib/pagerank.h"
#include "lib/math.h"
#include "lib/benchmark.h"

DEFINE_double(damping_factor, 0.85, "PageRank main parameter");
DEFINE_double(eps, 1e-7, "Computation precision");

int world_rank;

std::string GetProc() {
	return "[" + std::to_string(world_rank) + "]: ";
}

int main(int argc, char *argv[]) {
	FLAGS_logtostderr = 1;
	google::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
	google::InitGoogleLogging(argv[0]);
	MPI_Init(nullptr, nullptr);
	Timer timer;

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	if (world_rank == 0) {
		LOG(INFO) << "World size: " << world_size;
	}

	int global_page_cnt, chunk_size;
	std::vector<int> out_link_cnts, dangling_pages;
	if (world_rank == 0) {
		ReadMetadata(&global_page_cnt, &chunk_size, out_link_cnts);
		dangling_pages = ExploreDanglingPages(out_link_cnts);
	}
	MPI_Bcast(&global_page_cnt, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (world_rank > 0) {
		out_link_cnts.resize(global_page_cnt);
	}
	MPI_Bcast(out_link_cnts.data(), global_page_cnt, MPI_INT, 0, MPI_COMM_WORLD);

	// Calculate data range to process on particular processor.
	int pages_per_proc = global_page_cnt / world_size;
	int begin = pages_per_proc * world_rank;
	int end = begin + pages_per_proc - 1;
	LOG(INFO) << GetProc() << "Handling pages " << begin << "..." << end << ".";

	std::vector<pr::Page> pages;
	pages.reserve(pages_per_proc);
	timer.Start();
	ReadPages(chunk_size, begin, end, pages);
	timer.StopAndReport(GetProc() + "Reading pages");

	// If global_page_cnt % world_size != 0 then there are some pages remaining.
	// We are going to process them at root.
	int reminder_cnt = global_page_cnt % world_size;
	int reminder_begin = global_page_cnt - reminder_cnt;
	int reminder_end = global_page_cnt - 1;
	if (world_rank == 0) {
		if (reminder_cnt > 0) {
			LOG(INFO) << "Reminder " << reminder_begin << "..." << reminder_end
				<< ".";

			timer.Start();
			ReadPages(chunk_size, reminder_begin, reminder_end, pages);
			timer.StopAndReport("Reading reminder pages");
		}
	}

	std::vector<long double> global_pr(global_page_cnt);
	std::vector<long double> pr(pages.size());
	std::vector<long double> old_pr;
	if (world_rank == 0) {
		global_pr = InitPr(global_page_cnt);
		old_pr.resize(global_page_cnt);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	timer.Start();
	bool go_on = true;
	int step = 0;
	while (go_on) {
		MPI_Bcast(global_pr.data(), global_page_cnt, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
		if (world_rank == 0) {
			std::copy(global_pr.begin(), global_pr.end(), old_pr.begin());
		}

		AddPagesPr(pages, out_link_cnts, global_pr, pr);
		// NOTE! We use pages_per_proc instead of page.size()
		// to gather PRs, because size should remaind the same for
		// all the processes. Some part of PRs evaluated at root are not
		// going to be visible for MPI_Gather. But, it's OK until they
		// remain on host. We will take them into account.
		MPI_Gather(pr.data(), pages_per_proc, MPI_LONG_DOUBLE,
				global_pr.data(), pages_per_proc, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);

		if (world_rank == 0) {
			std::copy(pr.begin() + pages_per_proc, pr.end(),
					global_pr.begin() + reminder_begin);

			AddDanglingPagesPr(dangling_pages, old_pr, global_pr);
			AddRandomJumpsPr(FLAGS_damping_factor, global_pr);

			long double err = L1Norm(global_pr, old_pr);
			LOG(INFO) << "Error " << std::setprecision(10) << std::fixed << err
				<< " at step " << step + 1;
			go_on = err > FLAGS_eps;
		}

		MPI_Bcast(&go_on, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
		step++;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	timer.StopAndReport("PageRank");

	if (world_rank == 0) {
		WritePr(global_pr);
	}

	MPI_Finalize();
	google::ShutDownCommandLineFlags();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
