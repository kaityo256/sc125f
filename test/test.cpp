#include "../sc125fa.hpp"
int main(void) {
  const char *str = "000000000000000001001023040000500020002041600070000000004036702060050030800900060";
  sc125f::initTables(); // 定数テーブルの初期化.
  int sc = sc125f::calc(str);
  printf("%d\n", sc);
}
