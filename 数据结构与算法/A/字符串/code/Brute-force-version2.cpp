#include <string>

using namespace std;


int match(string P, string T) {
	size_t n = T.size(), i;
	size_t m = P.size(), j;
	for ( i = 0; i < n - m + 1; i++) {
		for ( j = 0; j < m; j++)
			if ( T[i+j] != P[j]) break;
		if ( m <= j) break;
	}
	return i;
}