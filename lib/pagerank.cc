#include "pagerank.h"

#include "math.h"

/*
 * Dangling pages - are pages with no out-links. So their PR is divided equally
 * between all pages in graph. It's not efficiently to handle them as ordinary
 * page and add in-links to all pages. We are going to process them differently
 * so we need information about all of them.
 */
std::vector<int> ExploreDanglingPages(const std::vector<int> &out_link_cnts) {
	std::vector<int> dangling_pages;
	for (int i = 0; i < out_link_cnts.size(); i++) {
		if (out_link_cnts[i] == 0) {
			dangling_pages.push_back(i);
		}
	}
	return dangling_pages;
}

std::vector<long double> InitPr(int page_cnt) {
	std::vector<long double> pr;
	pr.reserve(page_cnt);
	for (int i = 0; i < page_cnt; i++) {
		pr.push_back(1.0 / page_cnt);
	}
	return pr;
}

/*
 * Sum PR from all in-links. This is the botleneck part of PR evaluation.
 *
 * We are going to multiply sum by damping_factor in AddDanglingPages.
 */
void AddPagesPr(
		const std::vector<pr::Page> &pages,
		const std::vector<int> &out_link_cnts,
		const std::vector<long double> &old_pr,
		std::vector<long double> &new_pr) {
	for (int i = 0; i < pages.size(); i++) {
		long double sum = 0;
		for (int from_page : pages[i].in_links()) {
			sum += old_pr[from_page] / out_link_cnts[from_page];
		}
		new_pr[i] = sum;
	}
}

void AddDanglingPagesPr(
		const std::vector<int> &dangling_pages,
		const std::vector<long double> &old_pr,
		std::vector<long double> &new_pr) {
	long double sum = 0;
	for (int page : dangling_pages) {
		sum += old_pr[page];
	}
	for (long double &pr : new_pr) {
		pr += sum / new_pr.size();
	}
}

void AddRandomJumpsPr(
		long double damping_factor,
		std::vector<long double> &new_pr) {
	for (long double &pr : new_pr) {
		pr = pr * damping_factor + (1 - damping_factor) / new_pr.size();
	}
}
