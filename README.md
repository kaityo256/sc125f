# sc125f

## 概要

川合さんによる数独難易度判定プログラムを渡辺がラップしたもの。

`sc125f::calc`に`const char*`で数独の問題を渡すと、難易度をintで返す。

```cpp
#include "sc125fa.hpp"
int main(void) {
  const char *str = "000000000000000001001023040000500020002041600070000000004036702060050030800900060";
  int sc = sc125f::calc(str);
  printf("%d\n", sc); // => 16826
}
```

## ライセンス

このプログラムは川合堂ライセンス-01 ver.1.0で公開する。
詳細は[LICENSE](LICENSE)ファイルを参照のこと。