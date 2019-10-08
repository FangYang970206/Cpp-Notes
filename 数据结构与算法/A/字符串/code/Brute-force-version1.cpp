#include <string>

using namespace std;


int match(string P, string T) {
	size_t n = T.size(), i = 0;
	size_t m = P.size(), j = 0;
	while (i < n && j < m)
		if ( T[i] == P[j]) { i++; j++;}
		else { i -= j - 1; j = 0;}
	return i - j;
}