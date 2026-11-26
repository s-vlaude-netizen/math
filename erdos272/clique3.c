/* Erdos 272 / A401861: max family of subsets of {1..n} whose pairwise intersections are nonempty APs.
   Exact max clique (bit-parallel BBMC, greedy colouring) on the compatibility graph of the 2^n-1 nonempty sets.
   usage: clique n [lower_bound_to_beat]  -- prints the maximum clique size >= lb (or "none >= lb") and the sets. */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
typedef uint64_t u64;
static int only0=0, no0=0; static int split_m=1, split_r=0; static char *rootdone; static FILE *ckpt; static int rootcount=0; static double t0; static int *ordpool,*colpool; static int *perm; static int n, V, W, best, *bestset, cur[16384];
static u64 *adj; static long long nodes;
static int isap(unsigned m){            /* m nonzero bitmask: is the set an arithmetic progression (size<=2 always) */
    int c=__builtin_popcount(m); if(c<=2) return 1;
    int a=__builtin_ctz(m); unsigned r=m&(m-1); int b=__builtin_ctz(r); int d=b-a;
    unsigned x=m; int cnt=0; for(int p=a;p<n&&((m>>p)&1);p+=d) cnt++; return cnt==c;
}
static void expand(u64 *P,int depth){
    nodes++;
    /* order vertices by greedy colouring */
    int *order=ordpool+(size_t)depth*V, *col=colpool+(size_t)depth*V, cnt=0;
    u64 U[W>0?W:1]; memcpy(U,P,W*8);
    u64 Q[W]; int k=0;
    int nonempty=1;
    while(nonempty){
        k++; memcpy(Q,U,W*8);
        nonempty=0;
        for(int w=0;w<W;w++) while(Q[w]){
            int b=__builtin_ctzll(Q[w]); int v=w*64+b;
            U[w]&=~(1ULL<<b); Q[w]&=~(1ULL<<b);
            u64 *a=adj+(size_t)v*W;
            for(int j=w;j<W;j++) Q[j]&=~a[j];
            order[cnt]=v; col[cnt]=k; cnt++;
        }
        for(int w=0;w<W;w++) if(U[w]){nonempty=1;break;}
    }
    for(int i=cnt-1;i>=0;i--){
        if(depth+col[i]<=best) return;
        int v=order[i]; cur[depth]=v;
        if(depth==0 && ((cnt-1-i)%split_m)!=split_r){ P[v>>6]&=~(1ULL<<(v&63)); continue; }
        if(depth==0 && only0 && (cnt-1-i)!=0){ P[v>>6]&=~(1ULL<<(v&63)); continue; }
        if(depth==0 && no0 && (cnt-1-i)==0){ P[v>>6]&=~(1ULL<<(v&63)); continue; }
        if(depth==0 && rootdone && rootdone[cnt-1-i]){ P[v>>6]&=~(1ULL<<(v&63)); continue; }
        u64 NP[W]; int any=0; u64 *a=adj+(size_t)v*W;
        for(int w=0;w<W;w++){NP[w]=P[w]&a[w]; if(NP[w]) any=1;}
        if(any) expand(NP,depth+1);
        else if(depth+1>best){best=depth+1; memcpy(bestset,cur,(depth+1)*sizeof(int)); fprintf(stderr,"best=%d\n",best);}
        P[v>>6]&=~(1ULL<<(v&63));
        if(depth==0){ rootcount++; if(ckpt){ fprintf(ckpt,"%d\n",cnt-1-i); fflush(ckpt);} fprintf(stderr,"root %d/%d done, nodes=%lld, t=%.0fs\n",cnt-1-i,cnt,nodes,(double)clock()/CLOCKS_PER_SEC); }
    }
}
int main(int argc,char**argv){
    n=atoi(argv[1]); int lb=argc>2?atoi(argv[2]):0; if(argc>4){split_m=atoi(argv[3]); split_r=atoi(argv[4]);} V=(1<<n)-1; W=(V+63)/64;
    ordpool=malloc((size_t)512*V*sizeof(int)); colpool=malloc((size_t)512*V*sizeof(int)); adj=calloc((size_t)V*W,8); bestset=malloc(16384*sizeof(int));
    /* vertex order: decreasing degree; vertex v represents set mask perm[v] */
    int *deg=calloc(V,sizeof(int)); perm=malloc(V*sizeof(int));
    for(int i=0;i<V;i++) for(int j=0;j<V;j++) if(i!=j){ unsigned m=(i+1)&(j+1); if(m&&isap(m)) deg[i]++; }
    for(int i=0;i<V;i++) perm[i]=i;
    for(int i=1;i<0;i++){int t=perm[i],j=i-1; while(j>=0&&deg[perm[j]]<deg[t]){perm[j+1]=perm[j];j--;} perm[j+1]=t;}
    for(int a=0;a<V;a++) for(int b=0;b<V;b++) if(a!=b){ unsigned m=(perm[a]+1)&(perm[b]+1); if(m&&isap(m)) adj[(size_t)a*W+(b>>6)]|=1ULL<<(b&63);}
    { const char*ck=getenv("CKPT"); if(ck){ rootdone=calloc(V+1,1); FILE*f=fopen(ck,"r"); if(f){int k; while(fscanf(f,"%d",&k)==1) if(k>=0&&k<=V) rootdone[k]=1; fclose(f);} ckpt=fopen(ck,"a"); } }
    only0=getenv("ONLY0")!=NULL; no0=getenv("NO0")!=NULL;
    best=lb-1; u64 P[W]; memset(P,0,W*8); for(int i=0;i<V;i++) P[i>>6]|=1ULL<<(i&63);
    expand(P,0);
    if(best<lb){printf("n=%d none >= %d nodes=%lld\n",n,lb,nodes);return 0;}
    printf("n=%d max=%d nodes=%lld\n",n,best,nodes);
    for(int i=0;i<best;i++){printf("{");int f=1;for(int p=0;p<n;p++) if(((perm[bestset[i]]+1)>>p)&1){printf(f?"%d":",%d",p+1);f=0;}printf("} ");}
    printf("\n");return 0;}
