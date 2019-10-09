#include <vector>
#include <string>

using namespace std;

vector<int> buildNext(string P) { //构造模式串P的next表
    size_t m = P.size(), j = 0;   //“主”串指针
    vector<int> N(m, 0);          //next表
    int t = N[0] = -1;			  //模式串指针（通配符*）
    while ( j  < m - 1 )          //j是不会减小的，j会在循环内变为m-1，此时退出
        if ( 0 > t || P[j] == P[t] ) { //当出现通配符也就是0>t, 当前j自加1，next表对应j为0。
            						   //当不是通配符时，比较是否相等，相等则next表对应j自加1
            j++; t++;
            N[j] = t;
        }
    	else
            t = N[t];  //失配，根据前面得到的next，来看应该从哪里开始比较，比如下面的匹配等于4的时候，
					   // e不等于c，查表知e所在的位置为0，也就是没有相同的前后缀，所以从0开始继续匹配，
					   // 如果大于0，说明有共同前后缀，此时应该不从0开始，因为有共同前后缀，可以避开节省时间。
    return N;
}

int match(string P, string T) {
    vector<int> next = buildNext(P);
    size_t n = T.size(), i = 0;
    size_t m = P.size(), j = 0;
    while (j < m && i < n)
        if (0 > j || T[i] == P[j]) { i++; j++;}
    	else j = next[j];
    return i - j;
}

