/* gaps3 with 256-bit position sets (g <= 255) and a scan loop.  usage: gaps3w nmin nmax
   Prints a(n) = least even g not attained for primes <= prime(n), scanning g upward from the previous a(n)-... (g starts at 2). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long u64;
typedef struct { u64 w[4]; } B;
static int n, g, P[64], sel[64]; static B cls[64][256]; static B full; static long long nodes;
static inline int pc(B x){ return __builtin_popcountll(x.w[0])+__builtin_popcountll(x.w[1])+__builtin_popcountll(x.w[2])+__builtin_popcountll(x.w[3]); }
static inline B band(B a,B b){ B r; for(int i=0;i<4;i++) r.w[i]=a.w[i]&b.w[i]; return r; }
static inline B bor(B a,B b){ B r; for(int i=0;i<4;i++) r.w[i]=a.w[i]|b.w[i]; return r; }
static inline B bandn(B a,B b){ B r; for(int i=0;i<4;i++) r.w[i]=a.w[i]&~b.w[i]; return r; }
static inline int zero(B a){ return !(a.w[0]|a.w[1]|a.w[2]|a.w[3]); }
static int rec(int k, B cov){
    nodes++;
    B unc=bandn(full,cov); if(zero(unc)){ for(int j=k;j<n;j++) sel[j]=-1; return 1; }
    if(k==n) return 0;
    int sum=0, need=pc(unc);
    for(int q=k;q<n;q++){ int b=0; for(int r=1;r<P[q];r++) if(r!=g%P[q]){ int c=pc(band(cls[q][r],unc)); if(c>b) b=c; } sum+=b; if(sum>=need) break; }
    if(sum<need) return 0;
    for(int r=1;r<P[k];r++){ if(r==g%P[k]) continue; sel[k]=r; if(rec(k+1,bor(cov,cls[k][r]))) return 1; }
    return 0;
}
static int attained(int gg){
    g=gg; memset(&full,0,sizeof full); for(int j=1;j<g;j++) full.w[j>>6]|=1ULL<<(j&63);
    for(int k=0;k<n;k++) for(int r=0;r<P[k];r++){ memset(&cls[k][r],0,sizeof(B)); for(int j=1;j<g;j++) if(j%P[k]==r) cls[k][r].w[j>>6]|=1ULL<<(j&63); }
    B z; memset(&z,0,sizeof z); nodes=0; return rec(0,z);
}
int main(int c,char**v){ int nmin=atoi(v[1]), nmax=atoi(v[2]); int m=0; for(int x=2;m<64;x++){int ok=1;for(int d=2;d*d<=x;d++) if(x%d==0){ok=0;break;} if(ok)P[m++]=x;}
  for(n=nmin;n<=nmax;n++){ long long tot=0; int gg=2; while(gg<256 && attained(gg)){ tot+=nodes; if(getenv("WIT")){ printf("W %d %d",n,gg); for(int k=0;k<n;k++) printf(" %d",sel[k]); printf("\n"); } gg+=2; } tot+=nodes;
     printf("n=%d prime=%d a(n)=%d total_nodes=%lld\n",n,P[n-1],gg,tot); fflush(stdout); } return 0; }
