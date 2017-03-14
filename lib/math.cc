#include "math.h"

#include <cmath>

long double L1Norm(
		const std::vector<long double> a,
		const std::vector<long double> b) {
	long double sum = 0;
	for (int i = 0; i < a.size(); i++) {
		sum += std::abs(a[i] - b[i]);
	}
	return sum;
}
