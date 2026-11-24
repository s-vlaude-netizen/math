/* Independent infeasibility check for Erdos 854 / A389839: branches over PRIMES in increasing order (gaps2 branches over
   positions).  usage: gaps3 n g   -> prints whether g is attained for primes <= prime(n).  Positions 1..g-1, g <= 127. */
#include <stdio.h>
#include <stdlib.h>
typedef unsigned __int128 u128;
static int n, g, P[32]; static u128 cls[32][128]; static long long nodes; static u128 full;
static int pc(u128 x){ return __builtin_popcountll((unsigned long long)x)+__builtin_popcountll((unsigned long long)(x>>64)); }
static int rec(int k, u128 cov){
    nodes++;
    if(k==n) return cov==full;
    u128 unc=full&~cov; if(!unc) { /* remaining primes pick any allowed residue */ return 1; }
    /* bound: remaining primes' best single-class coverage of uncovered positions */
    int sum=0, need=pc(unc);
    for(int q=k;q<n;q++){ int b=0; for(int r=1;r<P[q];r++) if(r!=g%P[q]){ int c=pc(cls[q][r]&unc); if(c>b) b=c; } sum+=b; }
    if(sum<need) return 0;
    for(int r=0;r<P[k];r++){ if(r==0||r==g%P[k]) continue; if(rec(k+1,cov|cls[k][r])) return 1; }
    return 0;
}
int main(int c,char**v){ n=atoi(v[1]); g=atoi(v[2]); int m=0; for(int x=2;m<n;x++){int ok=1;for(int d=2;d*d<=x;d++) if(x%d==0){ok=0;break;} if(ok)P[m++]=x;}
  full=0; for(int j=1;j<g;j++) full|=(u128)1<<j;
  for(int k=0;k<n;k++) for(int r=0;r<P[k];r++){ cls[k][r]=0; for(int j=1;j<g;j++) if(j%P[k]==r) cls[k][r]|=(u128)1<<j; }
  int f=rec(0,0); printf("n=%d g=%d %s nodes=%lld\n",n,g,f?"ATTAINED":"NOT attained",nodes); return 0; }
