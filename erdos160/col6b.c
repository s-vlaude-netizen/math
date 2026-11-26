/* Exact, parallel, checkpointed solver for Erdos 160 / A402186: is there a k-colouring of {1..N} in which every 4-term AP has >= 3 colours?
   Same search as color4ap.c (fail-first DFS, forward checking, colour symmetry broken by trying one unused colour only), made
   deterministic so that the tree can be split: the nodes at decision depth D are "tasks", worker r of m takes tasks with index % m == r,
   and finished tasks are appended to a checkpoint file, so an interrupted run resumes.
   usage: col6 k N D m r [ckpt_file]       exit 0 = colouring found (printed), 1 = this worker's share is exhausted (no colouring there). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define MAXN 64
static int K, N, nap, D, M, R;
static uint8_t ap[2000][4];
static uint16_t apof[MAXN][128]; static int napof[MAXN];
static uint32_t dom[MAXN]; static int col[MAXN];
static long long nodes;
static int trail_pos[200000]; static uint32_t trail_dom[200000]; static int trail_col[200000]; static int tlen;
static int queue[200000], qh, qt; static char inq[MAXN];
static uint32_t full;
static FILE *ck; static char *taskdone; static long long taskidx, ntasks_seen, done_skipped, done_now;
static inline void save(int p){ trail_pos[tlen]=p; trail_dom[tlen]=dom[p]; trail_col[tlen]=col[p]; tlen++; }
static void undo(int to){ while(tlen>to){ tlen--; int p=trail_pos[tlen]; dom[p]=trail_dom[tlen]; col[p]=trail_col[tlen]; } }
static inline void enq(int p){ if(!inq[p]){ inq[p]=1; queue[qt++]=p; } }
static inline int setdom(int p, uint32_t nd){
    if(nd==dom[p]) return 1;
    save(p); dom[p]=nd; if(!nd) return 0;
    if(col[p]<0 && (nd&(nd-1))==0) col[p]=__builtin_ctz(nd);
    /* a domain shrink of an unassigned position can only matter to a rule when it has <= 2 colours left
       (rule 3 needs dom(u) inside a 2-set; a singleton is assigned above); larger domains need no re-check */
    if(col[p]>=0 || __builtin_popcount(nd)<=2) enq(p);
    return 1;
}
static int check_ap(int a){
    const uint8_t *q=ap[a]; int as[4], na=0, un[4], nu=0;
    for(int i=0;i<4;i++){ int c=col[q[i]]; if(c>=0) as[na++]=c; else un[nu++]=q[i]; }
    uint32_t S=0; for(int i=0;i<na;i++) S|=1u<<as[i]; int ns=__builtin_popcount(S);
    if(nu==0) return ns>=3;
    if(nu==1){ if(ns==1) return 0; if(ns==2) return setdom(un[0],dom[un[0]]&~S); return 1; }
    if(nu==2){
        if(as[0]==as[1]){ uint32_t a1=1u<<as[0];
            if(!setdom(un[0],dom[un[0]]&~a1)) return 0; if(!setdom(un[1],dom[un[1]]&~a1)) return 0;
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
        for(int i=0;i<napof[p];i++) if(!check_ap(apof[p][i])){ while(qh<qt) inq[queue[qh++]]=0; qh=qt=0; return 0; } }
    qh=qt=0; return 1;
}
static int dfs(int depth){
    nodes++;
    int best=-1, bs=99; uint32_t used=0;
    for(int p=0;p<N;p++){ if(col[p]>=0) used|=1u<<col[p]; else { int c=__builtin_popcount(dom[p]); if(c<bs){ bs=c; best=p; } } }
    if(best<0) return 1;
    if(bs>1){ /* the original scans left to right and stops at the first domain of size <= 1; here bs > 1 so the min is the first minimal one */ }
    else { for(int p=0;p<N;p++) if(col[p]<0 && __builtin_popcount(dom[p])<=1){ best=p; break; } }
    uint32_t cand=dom[best]&used; uint32_t un=dom[best]&~used&full; if(un) cand|=un&(-un);
    for(uint32_t m=cand;m;m&=m-1){ int c=__builtin_ctz(m);
        long long my=-1;
        if(depth==D){ my=taskidx++; ntasks_seen++; if(my%M!=R){ continue; } if(taskdone && my<(1LL<<30) && taskdone[my]){ done_skipped++; continue; } }
        int mark=tlen;
        save(best); col[best]=c; dom[best]=1u<<c; enq(best);
        int ok=propagate();
        int found=0;
        if(ok){ if(depth+1==D){ /* next nodes are tasks: handled by the loop above at depth==D */ }
                found=dfs(depth+1); }
        if(found) return 1;
        undo(mark); qh=qt=0; memset(inq,0,sizeof inq);
        if(depth==D && my>=0){ done_now++; if(ck){ fprintf(ck,"%lld\n",my); fflush(ck); } }
    }
    return 0;
}
int main(int argc,char**argv){
    if(argc<6){ fprintf(stderr,"usage: col6 k N D m r [ckpt]\n"); return 2; }
    K=atoi(argv[1]); N=atoi(argv[2]); D=atoi(argv[3]); M=atoi(argv[4]); R=atoi(argv[5]); full=(1u<<K)-1;
    nap=0; for(int d=1;3*d<N;d++) for(int a=0;a+3*d<N;a++){ for(int i=0;i<4;i++){ int p=a+i*d; ap[nap][i]=p; apof[p][napof[p]++]=nap; } nap++; }
    for(int p=0;p<N;p++){ dom[p]=full; col[p]=-1; }
    if(argc>6){ taskdone=calloc(1<<30>>3<<3?1:1,1); free(taskdone); taskdone=calloc((size_t)1<<24,1);
        FILE *f=fopen(argv[6],"r"); if(f){ long long t; while(fscanf(f,"%lld",&t)==1) if(t>=0 && t<(1<<24)) taskdone[t]=1; fclose(f); }
        ck=fopen(argv[6],"a"); }
    col[0]=0; dom[0]=1; enq(0);
    int ok=propagate() && dfs(0);
    if(ok){ printf("FOUND k=%d N=%d (nodes=%lld)\n",K,N,nodes); for(int p=0;p<N;p++) printf("%d ",col[p]+1); printf("\n"); return 0; }
    printf("DONE k=%d N=%d D=%d worker %d/%d: no colouring in this share; nodes=%lld tasks_seen=%lld done_before=%lld done_now=%lld\n",K,N,D,R,M,nodes,ntasks_seen,done_skipped,done_now);
    return 1;
}
