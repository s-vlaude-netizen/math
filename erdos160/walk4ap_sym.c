/* Local search (WalkSAT-style) for k-colourings of {1..N} in which every 4-term AP has >= 3 distinct colours.
   usage: walk4ap k N seconds [seed]   prints a colouring if one is found (then verify with verify.py). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#define MAXN 200
static int K,N,nap; static int perm[16]; static int mark[60000], stamp=0; static int ap[60000][4]; static int *apof[MAXN], napof[MAXN];
static int c[MAXN]; static int viol[60000], vpos[60000], nviol;   /* list of violated APs */
static uint64_t rs=88172645463325252ULL;
static inline uint64_t rnd(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }
static inline int is_bad(int a){ int *q=ap[a]; int x=c[q[0]],y=c[q[1]],z=c[q[2]],w=c[q[3]];
    uint32_t m=(1u<<x)|(1u<<y)|(1u<<z)|(1u<<w); return __builtin_popcount(m)<=2; }
static inline void vadd(int a){ if(vpos[a]<0){ vpos[a]=nviol; viol[nviol++]=a; } }
static inline void vdel(int a){ int i=vpos[a]; if(i>=0){ int l=viol[--nviol]; viol[i]=l; vpos[l]=i; vpos[a]=-1; } }
static void refresh(int p){ for(int i=0;i<napof[p];i++){ int a=apof[p][i]; if(is_bad(a)) vadd(a); else vdel(a); } }
static void setcol(int p,int col){ int m=N+1-p; c[p]=col; if(m!=p){ c[m]=perm[col]; } refresh(p); if(m!=p) refresh(m); }
static int cost_if(int p,int col){ int m=N+1-p; int op=c[p], om=c[m]; c[p]=col; if(m!=p) c[m]=perm[col]; stamp++; int n=0;
    for(int i=0;i<napof[p];i++){ int a=apof[p][i]; if(mark[a]!=stamp){ mark[a]=stamp; n+=is_bad(a);} }
    if(m!=p) for(int i=0;i<napof[m];i++){ int a=apof[m][i]; if(mark[a]!=stamp){ mark[a]=stamp; n+=is_bad(a);} }
    c[p]=op; if(m!=p) c[m]=om; return n; }
static int base_cost(int p){ int m=N+1-p; stamp++; int n=0;
    for(int i=0;i<napof[p];i++){ int a=apof[p][i]; if(mark[a]!=stamp){ mark[a]=stamp; n+=is_bad(a);} }
    if(m!=p) for(int i=0;i<napof[m];i++){ int a=apof[m][i]; if(mark[a]!=stamp){ mark[a]=stamp; n+=is_bad(a);} }
    return n; }
int main(int argc,char**argv){
    K=atoi(argv[1]); N=atoi(argv[2]); for(int i=0;i<K;i++) perm[i]=(argc>5+i)?atoi(argv[5+i])-1:i; double secs=atof(argv[3]); if(argc>4) rs^=(uint64_t)atoll(argv[4])*0x9E3779B97F4A7C15ULL; for(int i=0;i<10;i++) rnd();
    for(int p=1;p<=N;p++){ apof[p]=malloc(sizeof(int)*N*N/2); napof[p]=0; }
    nap=0; for(int d=1;3*d<N;d++) for(int a=1;a+3*d<=N;a++){ for(int i=0;i<4;i++){ ap[nap][i]=a+i*d; apof[a+i*d][napof[a+i*d]++]=nap; } nap++; }
    for(int a=0;a<nap;a++) vpos[a]=-1;
    clock_t t0=clock(); long long it=0; int best=1<<30;
    while(1){
        for(int p=1;p<=(N+1)/2;p++){ int col=rnd()%K; if(N%2==1 && p==(N+1)/2){ while(perm[col]!=col) col=rnd()%K; } c[p]=col; c[N+1-p]=perm[col]; }
        nviol=0; for(int a=0;a<nap;a++){ vpos[a]=-1; if(is_bad(a)) vadd(a); }
        for(long long step=0; step<20000000; step++,it++){
            if(nviol==0){ printf("FOUND k=%d N=%d\n",K,N); for(int p=1;p<=N;p++) printf("%d ",c[p]+1); printf("\n"); return 0; }
            if(nviol<best){ best=nviol; }
            if((it&0xFFFF)==0 && (double)(clock()-t0)/CLOCKS_PER_SEC>secs){ printf("timeout, best violations %d\n",best); return 1; }
            int a=viol[rnd()%nviol]; int bp=-1,bc=-1,bcost=1<<30;
            if(rnd()%100<8){ bp=ap[a][rnd()%4]; bc=rnd()%K; if(bp==N+1-bp && perm[bc]!=bc) bc=c[bp]; }
            else for(int i=0;i<4;i++){ int p=ap[a][i]; int base=base_cost(p);
                for(int col=0;col<K;col++){ if(col==c[p]) continue; if(p==N+1-p && perm[col]!=col) continue; int d=cost_if(p,col)-base; if(d<bcost || (d==bcost && (rnd()&1))){ bcost=d; bp=p; bc=col; } } }
            setcol(bp,bc);
        }
    }
}
