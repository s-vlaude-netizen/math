// Erdos 854 / OEIS A389839: smallest even g that is NOT the difference of two consecutive integers coprime to prime(n)#.
// g is attained iff there are residues s_p (p <= prime(n)) with: s_p != 0 and s_p != g (mod p) [x and x+g stay coprime to p],
// and every i in 1..g-1 satisfies i == s_p (mod p) for some p [x+i is not coprime to the primorial].
// Exact DFS: branch on the smallest uncovered position and on which unused prime covers it.
#include <bits/stdc++.h>
using namespace std;
typedef bitset<1024> BS;
static int G; static vector<int> P; static vector<BS> cls[64]; // cls[pi][r] = positions covered by residue r
static vector<vector<int>> allowed;                            // allowed residues per prime
static long long nodes;
static bool dfs(const BS& covered, unsigned long long used, int unc){
    nodes++;
    if(unc==0) return true;
    // capacity bound
    int cap=0; for(size_t k=0;k<P.size();k++) if(!(used>>k&1)) cap += (G-2)/P[k]+1;
    if(cap<unc) return false;
    int i=1; while(covered[i]) i++;
    // candidate (prime, residue) covering i
    struct C{int k; int gain;}; vector<C> cand;
    for(size_t k=0;k<P.size();k++) if(!(used>>k&1)){
        int p=P[k], r=i%p; if(r==0 || r==G%p) continue;
        BS nc=cls[k][r]&~covered; cand.push_back({(int)k,(int)nc.count()});
    }
    sort(cand.begin(),cand.end(),[](const C&a,const C&b){return a.gain>b.gain;});
    // positions i must be covered by some candidate; try each
    for(auto&c:cand){
        int p=P[c.k], r=i%p;
        BS nc=covered|cls[c.k][r];
        int nu=0; for(int j=1;j<G;j++) if(!nc[j]) nu++;
        // an unused prime that has no usable residue at all would make the instance infeasible; checked up front
        if(dfs(nc,used|(1ULL<<c.k),nu)) return true;
    }
    return false;
}
static bool feasible(int g){
    G=g; int m=P.size();
    for(int k=0;k<m;k++){ int p=P[k]; cls[k].assign(p,BS()); for(int r=0;r<p;r++) for(int j=1;j<g;j++) if(j%p==r) cls[k][r].set(j); }
    // forced primes: those with exactly one allowed residue
    BS cov; unsigned long long used=0;
    for(int k=0;k<m;k++){ int p=P[k]; vector<int> al; for(int r=0;r<p;r++) if(r!=0 && r!=g%p) al.push_back(r);
        if(al.empty()) return false; if(al.size()==1){ cov|=cls[k][al[0]]; used|=1ULL<<k; } }
    int unc=0; for(int j=1;j<g;j++) if(!cov[j]) unc++;
    return dfs(cov,used,unc);
}
int main(int argc,char**argv){
    int nmax=atoi(argv[1]);
    vector<int> pr; for(int x=2;(int)pr.size()<nmax;x++){bool ok=1;for(int d=2;d*d<=x;d++) if(x%d==0){ok=0;break;} if(ok) pr.push_back(x);}
    for(int n=2;n<=nmax;n++){
        P.assign(pr.begin(),pr.begin()+n); nodes=0;
        int g=2; while(feasible(g)) g+=2;
        printf("n=%d prime=%d a(n)=%d nodes=%lld\n",n,pr[n-1],g,nodes); fflush(stdout);
    }
}
