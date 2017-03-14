#include "io.h"

#include <sstream>
#include <fstream>
#include <iomanip>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "proto/metadata.pb.h"
#include "proto/chunk.pb.h"

DEFINE_string(dataset, "", "Input dataset");
DEFINE_string(output, "", "Path to file where to store calculated PageRank");

/*
 * NOTE! It's possible that begin >= chunk_begin, or end < chunk_end.
 * The real bounds are intersection of [begin; end] and [chunk_begin; chunk_end].
 */
void ReadPagesFromChunk(int chunk_size, int chunk_id,
		int begin, int end, std::vector<pr::Page> &pages) {
	int chunk_begin = chunk_id * chunk_size;
	int chunk_end = chunk_begin + chunk_size;
	if (begin < chunk_begin) begin = chunk_begin;
	if (end >= chunk_end) end = chunk_end - 1;

	pr::Chunk chunk;
	std::string filename = FLAGS_dataset  + "_" + std::to_string(chunk_id) +
		".chnk";
	std::ifstream in(filename);
	if (!in)
		LOG(FATAL) << "Error opening chunk file " + filename + " for reading.";
	if (!chunk.ParseFromIstream(&in))
		LOG(FATAL) << "Error parsing chunk from file " + filename + ".";

	int shift = begin - chunk_begin;
	std::move(chunk.mutable_pages()->begin() + shift,
			chunk.mutable_pages()->end(),
			std::back_inserter(pages));
}

/*
 * Read pages and push them to the back of output vector.
 */
void ReadPages(int chunk_size, int begin, int end,
		std::vector<pr::Page> &pages) {
	int begin_chunk = begin / chunk_size;
	int end_chunk = end / chunk_size;

	for (int chunk = begin_chunk; chunk <= end_chunk; chunk++) {
		ReadPagesFromChunk(chunk_size, chunk, begin, end, pages);
	}
}

void ReadMetadata(int *page_cnt_ptr, int *chunk_size_ptr,
		std::vector<int> &out_link_cnts) {
	pr::Metadata meta;

	std::string filename = FLAGS_dataset + ".meta";
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in)
		LOG(FATAL) << "Error opening metadata file " + filename + " for reading.";
	if (!meta.ParseFromIstream(&in))
		LOG(FATAL) << "Error parsing metadata from file " + filename + ".";

	*page_cnt_ptr = meta.page_cnt();
	*chunk_size_ptr = meta.chunk_size();

	out_link_cnts.clear();
	out_link_cnts.reserve(meta.page_cnt());
	std::move(meta.mutable_out_link_cnts()->begin(),
			meta.mutable_out_link_cnts()->end(),
			std::back_inserter(out_link_cnts));
}

void WritePr(const std::vector<long double> pr) {
	std::ofstream out(FLAGS_output);
	if (!out) {
		LOG(WARNING) << "Error opening output file '" + FLAGS_output +
			"' for writing. Results are not saved.";
		return;
	}
	for (int i = 0; i < pr.size(); i++) {
		out << i << " " << std::setprecision(10) << std::fixed << pr[i]
			<< std::endl;
	}
}
