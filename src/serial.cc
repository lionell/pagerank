/*
 * Usage: bazel run :serial -- \
 *          --dataset $(pwd)/data/generated/100k_10k \
 *          --output $(pwd)/out/serial
 */

#include <iomanip>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "lib/io.h"
#include "lib/math.h"
#include "lib/pagerank.h"
#include "lib/benchmark.h"

DEFINE_double(damping_factor, 0.85, "PageRank main parameter");
DEFINE_double(eps, 1e-7, "Computation precision");

int main(int argc, char *argv[]) {
	FLAGS_logtostderr = 1;
	google::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
	google::InitGoogleLogging(argv[0]);
	Timer timer;

	int page_cnt, chunk_size;
	std::vector<int> out_link_cnts;
	ReadMetadata(&page_cnt, &chunk_size, out_link_cnts);

	std::vector<pr::Page> pages;
	pages.reserve(page_cnt);
	timer.Start();
	ReadPages(chunk_size, 0, page_cnt - 1, pages);
	timer.StopAndReport("Reading pages");

	std::vector<int> dangling_pages = ExploreDanglingPages(out_link_cnts);
	std::vector<long double> pr = InitPr(page_cnt);
	std::vector<long double> old_pr(page_cnt);

	timer.Start();
	bool go_on = true;
	int step = 0;
	while (go_on) {
		std::copy(pr.begin(), pr.end(), old_pr.begin());

		AddPagesPr(pages, out_link_cnts, old_pr, pr);
		AddDanglingPagesPr(dangling_pages, old_pr, pr);
		AddRandomJumpsPr(FLAGS_damping_factor, pr);

		long double err = L1Norm(pr, old_pr);
		LOG(INFO) << "Error " << std::setprecision(10) << std::fixed << err
			<< " at step " << step + 1;
		go_on = err > FLAGS_eps;
		step++;
	}
	timer.StopAndReport("PageRank");

	WritePr(pr);

	google::ShutDownCommandLineFlags();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
