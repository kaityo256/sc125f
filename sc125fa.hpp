#pragma once

#include <cmath>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

/*
使い方：
sc125f::initTables();
を呼び出してから、


int main(void) {
  const char *str = "000000000000000001001023040000500020002041600070000000004036702060050030800900060";
  sc125f::initTables(); // 定数テーブルの初期化.
  int sc = sc125f::calc(str);
  printf("%d\n", sc);
}
*/

namespace sc125f {

char msk1[81] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1};

int bid[81] = {
    0, 0, 0, 1, 1, 1, 2, 2, 2,
    0, 0, 0, 1, 1, 1, 2, 2, 2,
    0, 0, 0, 1, 1, 1, 2, 2, 2,
    3, 3, 3, 4, 4, 4, 5, 5, 5,
    3, 3, 3, 4, 4, 4, 5, 5, 5,
    3, 3, 3, 4, 4, 4, 5, 5, 5,
    6, 6, 6, 7, 7, 7, 8, 8, 8,
    6, 6, 6, 7, 7, 7, 8, 8, 8,
    6, 6, 6, 7, 7, 7, 8, 8, 8};

char sc125table0[9][9]; // 小ラインの方向テーブル.

void initTables();
int calcSc125(const char *s, int lv1, char verbose);
int getIdx(const char *s);

int calc(const char *s) {
  return calcSc125(s, 0, 0);
}

int st_i = -1, st_j0 = 0, st_j1 = 8;

/*
// テスト用のmain関数.
int main(int argc, const char **argv) {
  int lv0 = 0; // 高速化のための切り捨てライン.
  int sn = 0, sn0 = 0, sc;

  if (argc < 2) {
    fprintf(stderr, "usage>%s 1234...789(len=81) [lv0] [0] [i]\n", argv[0]);
    fprintf(stderr, "usage>%s filename [lv0] [sn0] [i]\n", argv[0]);
    exit(1);
  }

  if (argc >= 3)
    sscanf(argv[2], "%d", &lv0);
  if (argc >= 4)
    sscanf(argv[3], "%d", &sn0);
  if (argc >= 5)
    st_i = getIdx(&argv[4][0]);
  if (argc >= 6)
    st_j1 = st_j0 = argv[5][0] - '1';

  initTables(); // 定数テーブルの初期化.

  if (strlen(argv[1]) == 81 || (strlen(argv[1]) >= 83 && argv[1][81] == ':')) {
    sc = calcSc125(argv[1], lv0, 1);
    printf("%s  sc125=%05d (%03d:%03d) lv1=%03d pms1=%03d\n", argv[1], sc, sc >> 8, sc & 255, sc >> 8, sc & 255);
  } else {
    char s[4096];
    FILE *fp = fopen(argv[1], "rt");
    if (fp == NULL) {
      puts("fopen error");
      return 1;
    }
    while (fgets(s, 4096 - 2, fp) != NULL) {
      if (strlen(s) >= 81) {
        if (sn >= sn0) {
          int l = strlen(s);
          if (s[l - 1] == '\n')
            s[l - 1] = '\0';
          int t0 = clock();
          sc = calcSc125(s, lv0, 0);
          int t1 = clock();
          printf("%s  sc125=%05d lv1=%03d pms1=%03d time=%07d sn=%06d\n", s, sc, sc >> 8, sc & 255, t1 - t0, sn);
          fflush(stdout);
        }
        sn++;
      }
    }
  }
  return 0;
}
*/

static int popcount9[512];
static int xt[81][20];  // 数独排他テーブル.
static int gt[27][9];   // グループテーブル.
static char dt[81][81]; // distanceテーブル.
static int rg[81][3];   // 逆引きグループテーブル(横, 縦, ボックス).

int popcount32(unsigned int i);
void init_xt();
void init_dt();
void init_rg();
void init_sc125table0();

void initTables() {
  int i, j, ii, jj;
  for (i = 0; i < 512; i++)
    popcount9[i] = popcount32(i);

  init_xt();
  init_dt();

  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      gt[i][j] = i * 9 + j;     // 横.
      gt[9 + i][j] = j * 9 + i; // 縦.
    }
  }
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      for (ii = 0; ii < 3; ii++) {
        for (jj = 0; jj < 3; jj++)
          gt[18 + i * 3 + j][ii * 3 + jj] = (i * 3 + ii) * 9 + (j * 3 + jj); // ボックス.
      }
    }
  }

  init_rg();

  init_sc125table0();

  return;
}

int popcount32(unsigned int i) {
  i = i - ((i >> 1) & 0x55555555U);
  i = (i & 0x33333333U) + ((i >> 2) & 0x33333333U);
  i = (i + (i >> 4)) & 0x0f0f0f0fU;
  i = (i * 0x01010101U) >> 24;
  return i;
}

void init_xt() {
  int i, j, k, l;
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      k = 0;
      for (l = 0; l < 9; l++) {
        if (l != j)
          xt[i * 9 + j][k++] = i * 9 + l;
      }
      for (l = 0; l < 9; l++) {
        if (l != i)
          xt[i * 9 + j][k++] = l * 9 + j;
      }
      int i0 = (i / 3) * 3, j0 = (j / 3) * 3, i1, j1;
      for (i1 = 0; i1 < 3; i1++) {
        for (j1 = 0; j1 < 3; j1++) {
          if (i0 + i1 != i && j0 + j1 != j)
            xt[i * 9 + j][k++] = (i0 + i1) * 9 + (j0 + j1);
        }
      }
    }
  }
  return;
}

void init_sc125table0() {
  int i, j;
  for (i = 0; i < 9; i++) {
    for (j = 0; j < 9; j++) {
      sc125table0[i][j] = 0;
      if (i / 3 == j / 3) sc125table0[i][j] |= 1; // 行.
      if (i % 3 == j % 3) sc125table0[i][j] |= 2; // 列.
    }
  }
  return;
}

char isNeighbor1(int i, int j)
/* 第一近傍なら1 */
{
  int *pxt = xt[i], k;
  for (k = 0; k < 20; k++) {
    if (pxt[k] == j)
      return 1;
  }
  return 0;
}

char isNeighbor012(int i, int j)
/* 第二近傍以内なら1 */
{
  int ix = i % 9, iy = i / 9;
  int jx = j % 9, jy = j / 9;
  if (ix / 3 == jx / 3 || iy / 3 == jy / 3) return 1;
  return 0;
}

void init_dt() {
  int i, j;
  for (i = 0; i < 81; i++) {
    for (j = 0; j < 81; j++) {
      dt[i][j] = 2;
      if (isNeighbor012(i, j) == 0)
        dt[i][j] = 3;
      else if (i == j)
        dt[i][j] = 0;
      else if (isNeighbor1(i, j) != 0)
        dt[i][j] = 1;
    }
  }
  return;
}

void init_rg() {
  int i, j, k, p[81];
  for (i = 0; i < 81; i++)
    p[i] = 0;
  for (i = 0; i < 27; i++) {
    for (j = 0; j < 9; j++) {
      k = gt[i][j];
      rg[k][p[k]] = i;
      p[k]++;
    }
  }
  return;
}
void conv_s2f(const char *s, int *f) {
  int i;
  for (i = 0; i < 81; i++) {
    f[i] = 0;
    char c = s[i];
    if ('1' <= c && c <= '9')
      f[i] = 1 << (c - '1');
  }
  return;
}

int log2i(unsigned int i) {
  int c = -1;
  while (i > 0) {
    c++;
    i >>= 1;
  }
  return c;
}

int calcFf(const int *f, int i) {
  int *pxt = xt[i];
  return (f[pxt[0]] | f[pxt[1]] | f[pxt[2]] | f[pxt[3]]) | (f[pxt[4]] | f[pxt[5]] | f[pxt[6]] | f[pxt[7]]) | (f[pxt[8]] | f[pxt[9]] | f[pxt[10]] | f[pxt[11]]) | (f[pxt[12]] | f[pxt[13]] | f[pxt[14]] | f[pxt[15]]) | (f[pxt[16]] | f[pxt[17]] | f[pxt[18]] | f[pxt[19]]);
}

void init_fgt(const int *f, int *fgt) {
  int i, *ip;
  for (i = 0; i < 27; i++) {
    ip = gt[i];
    fgt[i] = f[ip[0]] | f[ip[1]] | f[ip[2]] | f[ip[3]] | f[ip[4]] | f[ip[5]] | f[ip[6]] | f[ip[7]] | f[ip[8]];
  }
  return;
}

struct StrSearchAnsSub {
  int ans1, ans;
  jmp_buf env;
};

void searchAnsSub(int *f, const int *cff, int i0, int i1, struct StrSearchAnsSub *w, const char *msk, int *ia, int *fgt) {
  int i, j, k, l, t, t1, ff[81], *ip, iac = 0, i2 = 0, m = 10, ffip[9];
  for (i = 0; i < 81; i++)
    ff[i] = 0;
  for (i = i0; i <= i1; i++) {
    if (f[i] != 0) continue;
    t = (fgt[rg[i][0]] | fgt[rg[i][1]] | fgt[rg[i][2]]) ^ 0x1ff;
    t &= cff[i];
    if (t == 0) goto fin;
    ff[i] = t;
    if (msk[i] != 0) {
      if ((t & (t - 1)) == 0) {
        ia[iac + 0] = i;
        ia[iac + 1] = t;
        iac += 2;
      } else if (m > popcount9[t]) {
        m = popcount9[t];
        i2 = i;
      }
    }
  }
  for (j = 0; j < 27; j++) {
    t1 = fgt[j];
    if (t1 == 0x1ff) continue; // これはかなり効果がある.
    ip = gt[j];
    ffip[0] = ff[ip[0]];
    ffip[1] = ff[ip[1]];
    ffip[2] = ff[ip[2]];
    ffip[3] = ff[ip[3]];
    ffip[4] = ff[ip[4]];
    ffip[5] = ff[ip[5]];
    ffip[6] = ff[ip[6]];
    ffip[7] = ff[ip[7]];
    ffip[8] = ff[ip[8]];
    for (k = 1; k <= 0x1ff; k <<= 1) {
      if ((t1 & k) != 0) continue;
      t = (ffip[0] & k) + (ffip[1] & k) + (ffip[2] & k) + (ffip[3] & k) + (ffip[4] & k) + (ffip[5] & k) + (ffip[6] & k) + (ffip[7] & k) + (ffip[8] & k);
      if (t > k) continue;
      if (t == 0) goto fin;
      for (l = 0; l < 9; l++) { // t == k
        if ((ffip[l] & k) != 0) break;
      }
      t = ip[l];
      if (msk[t] == 0) continue;
      for (l = 0; l < iac; l += 2) {
        if (ia[l] == t) break;
      }
      if (l >= iac) {
        ia[iac + 0] = t;
        ia[iac + 1] = k;
        iac += 2;
      }
    }
  }
  if (iac > 0) {
    for (i = 0; i < 27; i++)
      ff[i] = fgt[i];
    for (i = 0; i < iac; i += 2) {
      t = ia[i + 0];
      t1 = ia[i + 1];
      f[t] = t1;
      if ((fgt[rg[t][0]] & t1) != 0) goto fin1;
      fgt[rg[t][0]] |= t1;
      if ((fgt[rg[t][1]] & t1) != 0) goto fin1;
      fgt[rg[t][1]] |= t1;
      if ((fgt[rg[t][2]] & t1) != 0) goto fin1;
      fgt[rg[t][2]] |= t1;
    }
    for (; i1 >= 0 && f[i1] != 0; i1--)
      ;
    if (i1 < 0) {
      w->ans++;
      if (w->ans >= w->ans1)
        longjmp(w->env, 1);
    } else {
      for (; f[i0] != 0; i0++)
        ;
      searchAnsSub(f, cff, i0, i1, w, msk, &ia[iac], fgt);
    }
  fin1:
    for (i = 0; i < iac; i += 2)
      f[ia[i + 0]] = 0;
    for (i = 0; i < 27; i++)
      fgt[i] = ff[i];
  } else if (m <= 9) {
    i = i2;
    f[i] = 1; // 0以外のダミー.
    for (; f[i0] != 0; i0++)
      ;
    for (; f[i1] != 0; i1--)
      ;
    for (j = 1; j <= 0x100; j <<= 1) {
      if ((ff[i] & j) == 0) continue;
      f[i] = j;
      fgt[rg[i][0]] |= j;
      fgt[rg[i][1]] |= j;
      fgt[rg[i][2]] |= j;
      searchAnsSub(f, cff, i0, i1, w, msk, ia, fgt);
      fgt[rg[i][0]] ^= j;
      fgt[rg[i][1]] ^= j;
      fgt[rg[i][2]] ^= j;
    }
    f[i] = 0;
  } else {
    w->ans++;
    if (w->ans >= w->ans1)
      longjmp(w->env, 1);
  }
fin:
  return;
}

int searchAns0(const int *cff, int *f, const char *msk)
/* 解の個数が0か1以上かを判定できる */
{
  struct StrSearchAnsSub w;
  int i, j, k, i0, i1, fgt[27], ia[81 * 2], c = 0;
  char flg;
  w.ans = 0;
  w.ans1 = 1;
  for (i = 0; i < 27; i++) {
    k = 0;
    for (j = 0; j < 9; j++)
      k |= cff[gt[i][j]];
    if (k != 0x1ff) goto fin;
  }
  f[-1] = f[81] = 0; // 番兵
  for (i = 0; i < 81; i++) {
    f[i] = 0;
    if (popcount9[cff[i]] == 1)
      f[i] = cff[i];
  }
  do {
    flg = 0;
    for (i = 0; i < 81; i++) {
      int ff = calcFf(f, i) ^ 0x1ff;
      ff &= cff[i];
      if (ff == 0) return 0;
      if (f[i] != ff && popcount9[ff] == 1 && msk[i] != 0) {
        f[i] = ff;
        flg = 1;
      }
    }
  } while (flg != 0);
  c = 1;
  if (setjmp(w.env) == 0) {
    init_fgt(f, fgt);
    for (i0 = 0; i0 < 81 && f[i0] != 0; i0++)
      ;
    for (i1 = 80; i1 >= 0 && f[i1] != 0; i1--)
      ;
    searchAnsSub(f, cff, i0, i1, &w, msk, ia, fgt);
    c = 0;
  }
fin:
  return c;
}

int searchAns(const int *cff, const char *msk)
/* 解の個数が0か1以上かを判定できる */
{
  int _f[83];
  return searchAns0(cff, _f + 1, msk);
}

void initFf(const int *f, int *ff)
/* ヒントだけから、初期のffを構築する */
{
  int i;
  for (i = 0; i < 81; i++) {
    ff[i] = f[i];
    if (f[i] == 0)
      ff[i] = calcFf(f, i) ^ 0x1ff;
  }
  return;
}

int calcPms(const int *ff)
// pencil mark sum.
{
  int i, pms = -81;
  for (i = 0; i < 81; i++)
    pms += popcount9[ff[i]];
  return pms;
}

typedef struct Sc125Work_ {
  unsigned char s2[81], msk[81];
  int tff[81], to;
  const int *a;
} Sc125Work;

void setS2(const int *cff, unsigned char *s2) {
  int i, j, k, jj;
  int c;
  for (i = 0; i < 81; i++) {
    s2[i] = 9;
    if (popcount9[cff[i]] >= 2)
      s2[i] = popcount9[cff[i]];
  }
  for (i = 0; i < 27; i++) {
    for (j = 0; j < 9; j++) {
      jj = 1 << j;
      c = 0;
      for (k = 0; k < 9; k++)
        c += cff[gt[i][k]] & jj;
      c >>= j;
      if (c >= 2) {
        for (k = 0; k < 9; k++) {
          if ((cff[gt[i][k]] & jj) != 0 && s2[gt[i][k]] > c)
            s2[gt[i][k]] = c;
        }
      }
    }
  }
  return;
}

int calcSc125Sub1(Sc125Work *w, int ii, int i, int d, int bmf)
// d >= 1
{
  int c, bk[3], i0, i1, i2, bi;
  for (; i < 54; i++) {
    if (i < 27) {
      i0 = i * 3;
      i1 = i0 + 1;
      i2 = i0 + 2;
    } else {
      i0 = ((i - 27) / 9) * 27 + ((i - 27) % 9);
      i1 = i0 + 9;
      i2 = i0 + 18;
    }
    bi = bid[i0];
    if ((bmf & (1 << bi)) != 0) continue;
    if ((sc125table0[bid[ii]][bi] & 1) != 0 && i >= 27) continue;
    if ((sc125table0[bid[ii]][bi] & 2) != 0 && i < 27) continue;
    c = (w->msk[i0] == 0) + (w->msk[i1] == 0) + (w->msk[i2] == 0);
    if (c == 0) continue;
    if (c == 1 && i >= 27 && sc125table0[bid[ii]][bi] == 0) continue;
    bk[0] = w->msk[i0];
    bk[1] = w->msk[i1];
    bk[2] = w->msk[i2];
    if (w->msk[i0] == 0) w->msk[i0] = 3;
    if (w->msk[i1] == 0) w->msk[i1] = 3;
    if (w->msk[i2] == 0) w->msk[i2] = 3;
    if (d > 1) {
      c = calcSc125Sub1(w, ii, i + 1, d - 1, bmf | 1 << bi);
      if (c <= 0) return c;
    } else {
      c = searchAns(w->tff, (const char *)w->msk);
      if (c <= 0) return c;
    }
    w->msk[i0] = bk[0];
    w->msk[i1] = bk[1];
    w->msk[i2] = bk[2];
  }
  return 1;
}

int calcSc125Sub0(Sc125Work *w, const int *cff, int ii, int b, int llv) {
  int c, i, j, k;

  for (i = 0; i < 81; i++) {
    w->msk[i] = 0;
    w->tff[i] = cff[i];
    if (popcount9[cff[i]] <= 1)
      w->msk[i] = 4;
  }
  w->tff[ii] = b; // 第0近傍のみ(01).
  w->msk[ii] = 4;

  if (llv >= 3) { // 第一近傍1つ+2択全部(03).
    for (i = 0; i < 81; i++) {
      if (w->s2[i] <= 2 && w->msk[i] == 0)
        w->msk[i] = 2;
    }
  }

  if (2 <= llv && llv <= 3) { // 第一近傍1つ(02).
    for (k = 0; k < 3; k++) {
      for (j = 0; j < 9; j++) {
        i = gt[rg[ii][k]][j];
        if (w->msk[i] == 0)
          w->msk[i] = 1;
      }
      c = searchAns(w->tff, (const char *)w->msk);
      if (c == 0) return llv;
      for (i = 0; i < 81; i++) {
        if (w->msk[i] == 1)
          w->msk[i] = 0;
      }
    }
    return 99;
  }

  if (llv >= 4) { // 第一近傍全部(04).
    for (i = 0; i < 81; i++) {
      if (dt[ii][i] == 1 && w->msk[i] == 0)
        w->msk[i] = 1;
    }
  }
  if (llv <= 4) {
    c = searchAns(w->tff, (const char *)w->msk);
    if (c == 0) return llv;
    return 99;
  }

  c = calcSc125Sub1(w, ii, 0, llv - 4, 0);
  if (c == 0) return llv;
  return 99;
}

int getIdx(const char *s) {
  int i;
  if ('0' <= s[0] && s[0] <= '9')
    i = (s[0] - '0') * 10 + (s[1] - '0');
  else {
    i = ((s[0] - 1) & 0xf) * 9 + (s[1] - '1');
    if (i >= 81) i -= 9;
  }
  return i;
}

int calcSc125(const char *s, int lv1, char verbose) {
  int _f[83], *f = _f + 1, _a[83], *a = _a + 1, pms0 = 54 * 0 + 1;
  int ff[81], llv;
  int i, j, k, jj, min, pms, slvd = 0;
  int pms1 = 0, slvd1 = 0, sc1 = 0;
  int c, cc, c1 = 16, scb[81][9], c2, c3;
  char ss[81], msb[81][9][81];
  conv_s2f(s, f);
  initFf(f, ff);
  searchAns0(ff, a, msk1);
  if (lv1 == 0) c1 = 1;

  for (k = 81; s[k] == ':'; k += 4) { // これはテスト用の隠し機能.
    i = getIdx(&s[k + 1]);
    j = s[k + 3] - '1';
    if (verbose != 0) printf("off: i=%02d(%c%d) j=%d a=%d\n", i, i / 9 + 'A', (i % 9) + 1, j + 1, log2i(a[i]) + 1);
    ff[i] &= ~(1 << j);
  }

  for (i = 0; i < 81; i++) {
    ss[i] = 0;
    if (s[i] != '0')
      ss[i] = 1;
  }

  for (;;) {
    min = 0;
    for (i = 0; i < 81; i++) {
      if (ss[i] == 0 && popcount9[ff[i]] == 1) {
        ss[i] = 1;
        if (verbose != 0) printf("solved[%02d]: i=%02d(%c%d) j=%d\n", slvd, i, i / 9 + 'A', (i % 9) + 1, log2i(ff[i]) + 1);
        slvd++;
        for (j = 0; j < 20; j++)
          ff[xt[i][j]] &= ~ff[i];
        min = 1;
      }
    }
    if (min > 0) continue;

    if (verbose != 0) puts("--");
    pms = calcPms(ff);
    if (verbose != 0) {
      printf("rest-pm=%03d (%03d:%03d)  ", pms, lv1 * 16 + (1 - c1), pms1);
      for (i = 0; i < 81; i++) {
        if (popcount9[ff[i]] == 1)
          putchar(log2i(ff[i]) + '1');
        else
          putchar('0');
      }
      putchar('\n');
    }
    if (pms < pms0) break;

    Sc125Work w;
    setS2(ff, w.s2);

    min = 99;
    for (llv = 1; llv <= 13; llv++) {
      for (i = 0; i < 81; i++) {
        if (st_i >= 0 && st_i != i) continue;
        if (verbose != 0 && llv >= 1) printf("    i=%02d(%d)\r", i, llv);
        for (jj = st_j0; jj <= st_j1; jj++) {
          j = 1 << jj;
          if ((ff[i] & j) == 0) continue;
          if (j == a[i]) continue;
          scb[i][jj] = calcSc125Sub0(&w, ff, i, j, llv);
          for (k = 0; k < 81; k++)
            msb[i][jj][k] = w.msk[k];
          if (min > scb[i][jj])
            min = scb[i][jj];
        }
      }
      if (min < 99) break;
    }

    c = c3 = 0;
    for (i = 0; i < 81; i++) {
      cc = 0;
      if (st_i >= 0 && st_i != i) continue;
      if (ff[i] == a[i]) continue;
      for (jj = st_j0; jj <= st_j1; jj++) {
        j = 1 << jj;
        if ((ff[i] & j) == 0) continue;
        if (j == a[i]) continue;
        if (scb[i][jj] == min)
          cc = 1;
      }
      c += cc;
      if (cc == 0) c3++;
    }
    c2 = c;
    if (c2 > 16 || min == 13 || c3 < c2 * 2)
      c = 16;
    if (c == 16 && min < 13 && c3 >= c2 * 2)
      c = 15; // 	1/3以下にまではした.

    if (lv1 * 16 + (1 - c1) < min * 16 + (1 - c)) {
      lv1 = min;
      c1 = c;
      pms1 = pms;
      slvd1 = slvd;
      sc1 = (lv1 * 16 + (1 - c1)) << 8 | pms;
    }

    for (i = 0; i < 81; i++) {
      if (st_i >= 0 && st_i != i) continue;
      for (jj = st_j0; jj <= st_j1; jj++) {
        j = 1 << jj;
        if ((ff[i] & j) == 0) continue;
        if (j == a[i]) continue;
        if (scb[i][jj] <= lv1) {
          ff[i] ^= j;
          if (verbose != 0) {
            printf("level(%02d): i=%02d(%c%d) j=%d a=%d (%02d:%02d)  ",
                   scb[i][jj], i, i / 9 + 'A', (i % 9) + 1, jj + 1, log2i(a[i]) + 1, c2, c3);
            for (k = 0; k < 81; k++)
              printf("%d", msb[i][jj][k]);
            putchar('\n');
          }
        }
      }
    }

    if (st_i >= 0) break;
  }

  if (verbose != 0) printf("add_hints(max)=%02d  sc125=%05d (%02d:%02d:%03d)  time=%07.3f[sec]\n", slvd1, sc1, lv1, c1, pms1, clock() / (double)CLOCKS_PER_SEC);
  return sc1;
}
} // namespace sc125f