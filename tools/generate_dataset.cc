/*
 * Usage: bazel run //tools:generate_dataset -- \
 *          --output '/home/lionell/dev/labs/parallel_prog/data/generated/' \
 *          --page_cnt 100000 \
 *          --name 'test'
 */

#include <bits/stdc++.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "proto/chunk.pb.h"
#include "proto/metadata.pb.h"
#include "lib/benchmark.h"

using namespace std;

DEFINE_string(name, "", "Dataset name");
DEFINE_int32(page_cnt, 10000, "Amount of pages in dataset");
DEFINE_int32(max_in_link_cnt, 1000, "Max in-links per page");
DEFINE_int32(chunk_size, 10000, "Max amount of pages per chunk");
DEFINE_string(output, "", "Where to store dataset");

vector<int> pages;
vector<int> out_link_cnts;

void ShufflePagesPrefix(int k) {
	for (int i = 0; i < k; i++) {
		int j = rand() % (pages.size() - i);
		swap(pages[i], pages[j]);
	}
}

void FillPage(int id, pr::Page *p) {
	p->set_id(id);
	int in_link_cnt = rand() % FLAGS_max_in_link_cnt;
	ShufflePagesPrefix(in_link_cnt);
	for (int i = 0; i < in_link_cnt; i++) {
		p->add_in_links(pages[i]);
		out_link_cnts[pages[i]]++;
	}
}

void Init() {
	pages.reserve(FLAGS_page_cnt);
	out_link_cnts.reserve(FLAGS_page_cnt);
	for (int i = 0; i < FLAGS_page_cnt; i++) {
		pages.push_back(i);
		out_link_cnts.push_back(0);
	}
}

int main(int argc, char *argv[]) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	srand(time(0));
	FLAGS_logtostderr = 1;
	google::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);
	google::InitGoogleLogging(argv[0]);

	if (FLAGS_max_in_link_cnt > FLAGS_page_cnt) {
		LOG(WARNING) << "Using " << FLAGS_page_cnt << " as max_in_link_cnt.";
		FLAGS_max_in_link_cnt = FLAGS_page_cnt;
	}

	Timer chunk_io, chunk_gen;
	Init();
	int chunk_cnt = int(std::ceil(FLAGS_page_cnt / FLAGS_chunk_size));
	int page_id = 0;
	for (int i = 0; i < chunk_cnt; i++) {
		LOG_EVERY_N(INFO, 5) << "Generating " << i << "-th chunk.";

		chunk_gen.Start();
		pr::Chunk chunk;
		for (int j = 0; j < FLAGS_chunk_size; j++) {
			FillPage(page_id, chunk.add_pages());
			page_id++;
			if (page_id == FLAGS_page_cnt) break;
		}
		chunk_gen.Stop();

		ofstream out(FLAGS_output + FLAGS_name + "_" + to_string(i)
				+ ".chnk", ios::out | ios::trunc | ios::binary);
		chunk.SerializeToOstream(&out);
	}
	LOG(INFO) << "Chunks generation finished.";
	chunk_gen.ReportStats("chunk generation");

	LOG(INFO) << "Generating metadata.";
	pr::Metadata meta;
	meta.set_page_cnt(FLAGS_page_cnt);
	meta.set_chunk_size(FLAGS_chunk_size);
	for (int x : out_link_cnts) {
		meta.add_out_link_cnts(x);
	}

	ofstream out(FLAGS_output + FLAGS_name + ".meta", ios::out | ios::trunc
			| ios::binary);
	meta.SerializeToOstream(&out);
	LOG(INFO) << "Metadata generation finished.";

	google::protobuf::ShutdownProtobufLibrary();
	google::ShutDownCommandLineFlags();
	return 0;
}
