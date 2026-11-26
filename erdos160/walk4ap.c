/* Local search (WalkSAT-style) for k-colourings of {1..N} in which every 4-term AP has >= 3 distinct colours.
   usage: walk4ap k N seconds [seed]   prints a colouring if one is found (then verify with verify.py). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#define MAXN 200
static int K,N,nap; static int ap[60000][4]; static int *apof[MAXN], napof[MAXN];
static int c[MAXN]; static int viol[60000], vpos[60000], nviol;   /* list of violated APs */
static uint64_t rs=88172645463325252ULL;
static inline uint64_t rnd(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }
static inline int is_bad(int a){ int *q=ap[a]; int x=c[q[0]],y=c[q[1]],z=c[q[2]],w=c[q[3]];
    uint32_t m=(1u<<x)|(1u<<y)|(1u<<z)|(1u<<w); return __builtin_popcount(m)<=2; }
static inline void vadd(int a){ if(vpos[a]<0){ vpos[a]=nviol; viol[nviol++]=a; } }
static inline void vdel(int a){ int i=vpos[a]; if(i>=0){ int l=viol[--nviol]; viol[i]=l; vpos[l]=i; vpos[a]=-1; } }
static void setcol(int p,int col){ c[p]=col; for(int i=0;i<napof[p];i++){ int a=apof[p][i]; if(is_bad(a)) vadd(a); else vdel(a); } }
static int cost_if(int p,int col){ int old=c[p]; c[p]=col; int n=0; for(int i=0;i<napof[p];i++) n+=is_bad(apof[p][i]); c[p]=old; return n; }
int main(int argc,char**argv){
    K=atoi(argv[1]); N=atoi(argv[2]); double secs=atof(argv[3]); if(argc>4) rs^=(uint64_t)atoll(argv[4])*0x9E3779B97F4A7C15ULL; for(int i=0;i<10;i++) rnd();
    for(int p=1;p<=N;p++){ apof[p]=malloc(sizeof(int)*N*N/2); napof[p]=0; }
    nap=0; for(int d=1;3*d<N;d++) for(int a=1;a+3*d<=N;a++){ for(int i=0;i<4;i++){ ap[nap][i]=a+i*d; apof[a+i*d][napof[a+i*d]++]=nap; } nap++; }
    for(int a=0;a<nap;a++) vpos[a]=-1;
    clock_t t0=clock(); long long it=0; int best=1<<30;
    while(1){
        for(int p=1;p<=N;p++) c[p]=rnd()%K;
        nviol=0; for(int a=0;a<nap;a++){ vpos[a]=-1; if(is_bad(a)) vadd(a); }
        for(long long step=0; step<20000000; step++,it++){
            if(nviol==0){ printf("FOUND k=%d N=%d\n",K,N); for(int p=1;p<=N;p++) printf("%d ",c[p]+1); printf("\n"); return 0; }
            if(nviol<best){ best=nviol; }
            if((it&0xFFFF)==0 && (double)(clock()-t0)/CLOCKS_PER_SEC>secs){ printf("timeout, best violations %d\n",best); return 1; }
            int a=viol[rnd()%nviol]; int bp=-1,bc=-1,bcost=1<<30;
            if(rnd()%100<8){ bp=ap[a][rnd()%4]; bc=rnd()%K; }
            else for(int i=0;i<4;i++){ int p=ap[a][i]; int base=0; for(int j=0;j<napof[p];j++) base+=is_bad(apof[p][j]);
                for(int col=0;col<K;col++){ if(col==c[p]) continue; int d=cost_if(p,col)-base; if(d<bcost || (d==bcost && (rnd()&1))){ bcost=d; bp=p; bc=col; } } }
            setcol(bp,bc);
        }
    }
}
