#ifndef IO_H_
#define IO_H_

#include <gflags/gflags.h>

#include "proto/page.pb.h"

DECLARE_string(dataset);
DECLARE_string(output);

void ReadPages(int chunk_size, int begin, int end,
		std::vector<pr::Page> &pages);
void ReadMetadata(int *page_cnt_ptr, int *chunk_size_ptr,
		std::vector<int> &out_link_cnts);
void WritePr(const std::vector<long double> pr);

#endif  // IO_H_
