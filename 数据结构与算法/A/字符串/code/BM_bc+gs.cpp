#include <vector>
#include <string>

using namespace std;

vector<int> buildBC(string P){
    vector<int> bc(256, -1);
    for(size_t m = P.size(), j = 0; j < m; j++)
        bc[ P[j] ] = j;
    return bc;
}

vector<int> buildSS ( string P ) { //构造最大匹配后缀长度表：O(m)
    int m = P.size(); 
    vector<int> ss(m, 0); //Suffix Size表
    ss[m - 1]  =  m; //对最后一个字符而言，与之匹配的最长后缀就是整个P串
// 以下，从倒数第二个字符起自右向左扫描P，依次计算出ss[]其余各项
    for ( int lo = m - 1, hi = m - 1, j = lo - 1; j >= 0; j -- )
        if ( ( lo < j ) && ( ss[m - hi + j - 1] <= j - lo ) ) //情况一：该情况处于最大匹配后缀后的字符，例如，RICE中的R,I,C.
            ss[j] =  ss[m - hi + j - 1]; //直接利用此前已计算出的ss[]
        else { //情况二: 遇到匹配项，依次递减进行匹配
            hi = j; lo = min ( lo, hi );
            while ( ( 0 <= lo ) && ( P[lo] == P[m - hi + lo - 1] ) ) 
                lo--; //逐个对比处于(lo, hi]前端的字符
            ss[j] = hi - lo; // 高位减去递减后的低位，得到最长匹配长度
        }
    return ss;
}

vector<int> buildGS ( string P ) { //构造好后缀位移量表：O(m)
   vector<int> ss = buildSS ( P ); //Suffix Size表
   size_t m = P.size(); 
   vector<int> gs(m, m); //Good Suffix shift table
   for ( size_t i = 0, j = m - 1; j < UINT_MAX; j -- ) //逆向逐一扫描各字符P[j]
      if ( j + 1 == ss[j] ) //若P[0, j] = P[m - j - 1, m)，则
         while ( i < m - j - 1 ) //对于P[m - j - 1]左侧的每个字符P[i]而言
            gs[i++] = m - j - 1; //m - j - 1都是gs[i]的一种选择
   for ( size_t j = 0; j < m - 1; j ++ ) //正向扫描P[]各字符，gs[j]不断递减，直至最小
      gs[m - ss[j] - 1] = m - j - 1; //m - j - 1必是其gs[m - ss[j] - 1]值的一种选择
   return gs;
}

int match ( string P, string T ) { //Boyer-Morre算法（完全版，兼顾Bad Character与Good Suffix）
   vector<int> bc = buildBC ( P ), gs = buildGS ( P ); //构造BC表和GS表
   size_t i = 0; //模式串相对于文本串的起始位置（初始时与文本串左对齐）
   while ( T.size() >= i + P.size() ) { //不断右移（距离可能不止一个字符）模式串
      int j = P.size() - 1; //从模式串最末尾的字符开始
      while ( P[j] == T[i + j] ) //自右向左比对
         if ( 0 > --j ) break; 
      if ( 0 > j ) //若极大匹配后缀 == 整个模式串（说明已经完全匹配）
         break; //返回匹配位置
      else //否则，适当地移动模式串
         i += max ( gs[j], j - bc[ T[i + j] ] ); //位移量根据BC表和GS表选择大者
   }
   return i;
}