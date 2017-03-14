#ifndef PAGERANK_H_
#define PAGERANK_H_

#include <string>
#include <vector>

#include "proto/page.pb.h"

std::vector<int> ExploreDanglingPages(const std::vector<int> &out_link_cnts);
std::vector<long double> InitPr(int page_cnt);
void AddPagesPr(
		const std::vector<pr::Page> &pages,
		const std::vector<int> &out_link_cnts,
		const std::vector<long double> &old_pr,
		std::vector<long double> &new_pr);
void AddDanglingPagesPr(
		const std::vector<int> &dangling_pages,
		const std::vector<long double> &old_pr,
		std::vector<long double> &new_pr);
void AddRandomJumpsPr(
		long double damping_factor,
		std::vector<long double> &new_pr);

#endif  // PAGERANK_H_
