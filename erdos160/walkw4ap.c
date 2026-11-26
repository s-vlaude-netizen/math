/* Weighted local search ("breakout"/PAWS-style) for k-colourings of {1..N} with every 4-AP using >= 3 colours.
   usage: walkw4ap k N seconds [seed]     prints a colouring if found (verify with verify.py). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#define MAXN 200
static int K,N,nap; static int ap[60000][4]; static int *apof[MAXN], napof[MAXN];
static int c[MAXN]; static int w[60000]; static int viol[60000], vpos[60000], nviol;
static uint64_t rs=88172645463325252ULL;
static inline uint64_t rnd(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }
static inline int is_bad(int a){ int *q=ap[a]; uint32_t m=(1u<<c[q[0]])|(1u<<c[q[1]])|(1u<<c[q[2]])|(1u<<c[q[3]]); return __builtin_popcount(m)<=2; }
static inline void vadd(int a){ if(vpos[a]<0){ vpos[a]=nviol; viol[nviol++]=a; } }
static inline void vdel(int a){ int i=vpos[a]; if(i>=0){ int l=viol[--nviol]; viol[i]=l; vpos[l]=i; vpos[a]=-1; } }
static void setcol(int p,int col){ c[p]=col; for(int i=0;i<napof[p];i++){ int a=apof[p][i]; if(is_bad(a)) vadd(a); else vdel(a); } }
static int wcost(int p){ int s=0; for(int i=0;i<napof[p];i++){ int a=apof[p][i]; if(is_bad(a)) s+=w[a]; } return s; }
int main(int argc,char**argv){
    K=atoi(argv[1]); N=atoi(argv[2]); double secs=atof(argv[3]); if(argc>4) rs^=(uint64_t)atoll(argv[4])*0x9E3779B97F4A7C15ULL; for(int i=0;i<10;i++) rnd();
    for(int p=1;p<=N;p++){ apof[p]=malloc(sizeof(int)*N*N/2); napof[p]=0; }
    nap=0; for(int d=1;3*d<N;d++) for(int a=1;a+3*d<=N;a++){ for(int i=0;i<4;i++){ ap[nap][i]=a+i*d; apof[a+i*d][napof[a+i*d]++]=nap; } nap++; }
    for(int a=0;a<nap;a++){ vpos[a]=-1; w[a]=1; }
    for(int p=1;p<=N;p++) c[p]=rnd()%K;
    nviol=0; for(int a=0;a<nap;a++) if(is_bad(a)) vadd(a);
    clock_t t0=clock(); long long it=0; int best=1<<30; int flat=0;
    while(1){
        if(nviol==0){ printf("FOUND k=%d N=%d iterations=%lld\n",K,N,it); for(int p=1;p<=N;p++) printf("%d ",c[p]+1); printf("\n"); return 0; }
        if(nviol<best) best=nviol;
        if((it&0x3FFF)==0 && (double)(clock()-t0)/CLOCKS_PER_SEC>secs){ printf("timeout, best violations %d\n",best); return 1; }
        it++;
        /* candidate moves: positions of one random violated AP, every other colour; choose best weighted delta */
        int a=viol[rnd()%nviol]; int bp=-1,bc=-1,bd=1<<30,nb=0;
        for(int i=0;i<4;i++){ int p=ap[a][i]; int old=c[p]; int base=wcost(p);
            for(int col=0;col<K;col++){ if(col==old) continue; c[p]=col; int d=wcost(p)-base; c[p]=old;
                if(d<bd){ bd=d; bp=p; bc=col; nb=1; } else if(d==bd){ nb++; if(rnd()%nb==0){ bp=p; bc=col; } } } }
        if(bd<0 || (bd==0 && rnd()%100<30)){ setcol(bp,bc); }
        else { for(int j=0;j<nviol;j++) w[viol[j]]++; if(rnd()%100<2){ setcol(ap[a][rnd()%4],rnd()%K); } }
        if((it%200000)==0){ /* smooth weights */ for(int x=0;x<nap;x++) if(w[x]>1) w[x]--; }
    }
}
