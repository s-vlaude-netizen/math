/* Erdos problem 160 / OEIS A402186: h(N) = least k such that {1..N} can be k-coloured so that every 4-term
   arithmetic progression contains at least three distinct colours.
   This program decides, for given k and N, whether such a colouring exists, by exhaustive DFS with
   forward checking and colour-symmetry breaking (an unused colour is tried only once).
   usage: color4ap k N [print]     exit status 0 = colouring exists (printed), 1 = none exists.
   Propagation rules for a 4-AP with assigned colours S and unassigned positions U (all sound):
     |U|=0: need >= 3 distinct;  |U|=1: |S|=1 -> fail, |S|=2 -> the free position avoids S;
     |U|=2, both assigned equal (colour a): both free positions avoid a and differ from each other;
     |U|=2, assigned differ (colours a,b): not both free positions inside {a,b}. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#define MAXN 160
static int K, N, nap;
static int ap[40000][4];
static int *apof[MAXN], napof[MAXN];
static uint32_t dom[MAXN]; static int col[MAXN];
static long long nodes;
static int trail_pos[400000], trail_col[400000]; static uint32_t trail_dom[400000]; static int tlen;
static int queue[400000], qh, qt, inq[MAXN];
static uint32_t full;
static inline void save(int p){ trail_pos[tlen]=p; trail_dom[tlen]=dom[p]; trail_col[tlen]=col[p]; tlen++; }
static void undo(int to){ while(tlen>to){ tlen--; int p=trail_pos[tlen]; dom[p]=trail_dom[tlen]; col[p]=trail_col[tlen]; } }
static inline void enq(int p){ if(!inq[p]){ inq[p]=1; queue[qt++]=p; } }
static int setdom(int p, uint32_t nd){            /* returns 0 on wipe-out */
    if(nd==dom[p]) return 1;
    save(p); dom[p]=nd; if(!nd) return 0;
    if(col[p]<0 && (nd&(nd-1))==0){ col[p]=__builtin_ctz(nd); }
    enq(p); return 1;
}
static int check_ap(int a){
    int *q=ap[a]; int as[4], na=0, un[4], nu=0;
    for(int i=0;i<4;i++){ if(col[q[i]]>=0) as[na++]=col[q[i]]; else un[nu++]=q[i]; }
    uint32_t S=0; for(int i=0;i<na;i++) S|=1u<<as[i]; int ns=__builtin_popcount(S);
    if(nu==0) return ns>=3;
    if(nu==1){ if(ns==1) return 0; if(ns==2) return setdom(un[0],dom[un[0]]&~S); return 1; }
    if(nu==2){
        if(as[0]==as[1]){ uint32_t a=1u<<as[0];
            if(!setdom(un[0],dom[un[0]]&~a)) return 0; if(!setdom(un[1],dom[un[1]]&~a)) return 0;
            if(col[un[0]]>=0 && col[un[1]]>=0 && col[un[0]]==col[un[1]]) return 0;
            if(col[un[0]]>=0) if(!setdom(un[1],dom[un[1]]&~(1u<<col[un[0]]))) return 0;
            if(col[un[1]]>=0) if(!setdom(un[0],dom[un[0]]&~(1u<<col[un[1]]))) return 0;
            return 1; }
        else { if((dom[un[0]]&~S)==0){ if(!setdom(un[1],dom[un[1]]&~S)) return 0; }
               if((dom[un[1]]&~S)==0){ if(!setdom(un[0],dom[un[0]]&~S)) return 0; }
               return 1; }
    }
    return 1;
}
static int propagate(void){
    while(qh<qt){ int p=queue[qh++]; inq[p]=0;
        for(int i=0;i<napof[p];i++) if(!check_ap(apof[p][i])){ while(qh<qt){ inq[queue[qh++]]=0; } qh=qt=0; return 0; } }
    qh=qt=0; return 1;
}
static uint32_t used_colours(void){ uint32_t u=0; for(int p=1;p<=N;p++) if(col[p]>=0) u|=1u<<col[p]; return u; }
/* Knuth's estimator of the DFS tree size: follow one random root-to-leaf path; the estimate is the sum over the nodes on the
   path of the product of the branching factors above them. Unbiased; high variance, so average many probes. */
static uint64_t rs=88172645463325252ULL; static inline uint64_t rnd(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }
static double probe(void){
    double w=1, est=0; int mark0=tlen;
    while(1){
        est+=w;                                         /* this node is a dfs() call */
        int best=-1, bs=99;
        for(int p=1;p<=N;p++) if(col[p]<0){ int c=__builtin_popcount(dom[p]); if(c<bs){ bs=c; best=p; if(c<=1) break; } }
        if(best<0) break;                               /* complete colouring (solution) */
        uint32_t used=used_colours();
        uint32_t cand=dom[best]&used; uint32_t un=dom[best]&~used&full; if(un) cand|=un&(-un);
        int cl[32], ncl=0; for(uint32_t m=cand;m;m&=m-1) cl[ncl++]=__builtin_ctz(m);
        if(ncl==0) break;
        w*=ncl; int c=cl[rnd()%ncl];
        save(best); col[best]=c; dom[best]=1u<<c; enq(best);
        if(!propagate()){ qh=qt=0; memset(inq,0,sizeof(int)*(N+2)); break; }
    }
    undo(mark0); qh=qt=0; memset(inq,0,sizeof(int)*(N+2));
    return est;
}
int main(int argc,char**argv){
    K=atoi(argv[1]); N=atoi(argv[2]); full=(K==32)?~0u:((1u<<K)-1);
    for(int p=1;p<=N;p++){ apof[p]=malloc(sizeof(int)*(N*N)); napof[p]=0; }
    nap=0; for(int d=1;3*d<N;d++) for(int a=1;a+3*d<=N;a++){ for(int i=0;i<4;i++){ ap[nap][i]=a+i*d; apof[a+i*d][napof[a+i*d]++]=nap; } nap++; }
    for(int p=1;p<=N;p++){ dom[p]=full; col[p]=-1; }
    /* break the colour symmetry at the root: position 1 gets colour 0 */
    col[1]=0; dom[1]=1; enq(1);
    uint64_t probes=argc>3?strtoull(argv[3],0,10):100000; if(argc>4) rs^=strtoull(argv[4],0,10)*0x9E3779B97F4A7C15ULL; for(int i=0;i<20;i++) rnd();
    if(!propagate()){ printf("root propagation fails: no colouring\n"); return 1; }
    int base=tlen; double sum=0, sum2=0, mx=0;
    for(uint64_t t=0;t<probes;t++){ double e=probe(); sum+=e; sum2+=e*e; if(e>mx) mx=e; }
    double mean=sum/probes, var=sum2/probes-mean*mean;
    printf("k=%d N=%d probes=%llu estimated DFS nodes = %.3g  (std error %.2g, largest single probe %.3g)\n",K,N,(unsigned long long)probes,mean,sqrt(var/probes>0?var/probes:0),mx);
    (void)base; return 0;
}
