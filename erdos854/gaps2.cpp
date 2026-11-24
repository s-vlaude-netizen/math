// Erdos 854 / A389839, solver v2: same cover formulation as gaps.cpp, stronger pruning.
//  * branch on the uncovered position with the fewest primes able to cover it (fail first);
//  * bound: every unused prime covers at most max_r |class(p,r) & uncovered| positions; their sum must reach |uncovered|.
// usage: gaps2 nmin nmax [gstart]   -- prints a(n) for nmin <= n <= nmax, and a witness for the largest feasible g below it.
#include <bits/stdc++.h>
using namespace std;
typedef bitset<256> BS;
static int G, M; static vector<int> P; static vector<vector<BS>> cls; static vector<vector<char>> okr;
static long long nodes; static vector<int> chosen; static vector<int> bestW;
static bool dfs(const BS& cov, unsigned long long used, int unc, BS unc_set){
    nodes++;
    if(unc==0) return true;
    // bound + fail-first position
    int sum=0;
    for(int k=0;k<M;k++) if(!(used>>k&1)){
        int p=P[k], b=0;
        for(int r=1;r<p;r++) if(okr[k][r]){ int c=(int)(cls[k][r]&unc_set).count(); if(c>b) b=c; }
        sum+=b;
    }
    if(sum<unc) return false;
    int bj=-1, bc=1000;
    for(int j=1;j<G;j++) if(unc_set[j]){
        int c=0; for(int k=0;k<M;k++) if(!(used>>k&1)){ int p=P[k], r=j%p; if(okr[k][r]) c++; }
        if(c<bc){bc=c;bj=j; if(c==0) return false;}
    }
    struct C{int k,gain;}; vector<C> cand;
    for(int k=0;k<M;k++) if(!(used>>k&1)){ int p=P[k], r=bj%p; if(!okr[k][r]) continue; cand.push_back({k,(int)(cls[k][r]&unc_set).count()}); }
    sort(cand.begin(),cand.end(),[](const C&a,const C&b){return a.gain>b.gain;});
    for(auto&c:cand){
        int p=P[c.k], r=bj%p;
        BS nu=unc_set&~cls[c.k][r];
        chosen[c.k]=r;
        if(dfs(cov|cls[c.k][r],used|(1ULL<<c.k),(int)nu.count(),nu)) return true;
        chosen[c.k]=-1;
    }
    return false;
}
static bool feasible(int g){
    G=g; M=P.size(); cls.assign(M,{}); okr.assign(M,{}); chosen.assign(M,-1);
    for(int k=0;k<M;k++){ int p=P[k]; cls[k].assign(p,BS()); okr[k].assign(p,0);
        for(int r=0;r<p;r++){ okr[k][r]=(r!=0 && r!=g%p); for(int j=1;j<g;j++) if(j%p==r) cls[k][r].set(j);} }
    BS cov, uns; for(int j=1;j<g;j++) uns.set(j); unsigned long long used=0;
    for(int k=0;k<M;k++){ int p=P[k]; vector<int> al; for(int r=0;r<p;r++) if(okr[k][r]) al.push_back(r);
        if(al.empty()) return false; if(al.size()==1){ cov|=cls[k][al[0]]; uns&=~cls[k][al[0]]; used|=1ULL<<k; chosen[k]=al[0]; } }
    return dfs(cov,used,(int)uns.count(),uns);
}
int main(int argc,char**argv){
    int nmin=atoi(argv[1]), nmax=atoi(argv[2]); int g0=argc>3?atoi(argv[3]):2;
    vector<int> pr; for(int x=2;(int)pr.size()<nmax;x++){bool ok=1;for(int d=2;d*d<=x;d++) if(x%d==0){ok=0;break;} if(ok) pr.push_back(x);}
    for(int n=nmin;n<=nmax;n++){
        P.assign(pr.begin(),pr.begin()+n); nodes=0;
        int g=g0; vector<int> w;
        while(feasible(g)){ w=chosen; if(getenv("WIT")){ printf("W %d %d",n,g); for(int k=0;k<n;k++) printf(" %d",w[k]); printf("\n"); } g+=2; }
        printf("n=%d prime=%d a(n)=%d nodes=%lld  witness(g=%d) residues s_p:",n,pr[n-1],g,nodes,g-2);
        for(int k=0;k<n;k++) printf(" %d",w.empty()?-1:w[k]); printf("\n"); fflush(stdout);
    }
}
