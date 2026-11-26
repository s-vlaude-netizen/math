/* A small CDCL SAT solver: two watched literals, first-UIP learning with local minimisation, VSIDS (linear scan,
   fine for a few hundred variables), phase saving, Luby restarts, LBD-based deletion of learnt clauses.
   usage: sat file.cnf [conflict_limit]   prints "s SATISFIABLE"/"s UNSATISFIABLE"/"s UNKNOWN" and a "v ..." model line.
   Not tuned for large instances; written to settle small combinatorial problems (colourings, covers). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
typedef struct Clause { int sz; int learnt; int deleted; int lbd; float act; int lits[1]; } Clause;
typedef struct { Clause *c; int blocker; } Watch;
typedef struct { Watch *w; int n, cap; } WList;
static int nv, ncl;
static int8_t *val;                 /* per literal: 1 true, -1 false, 0 undef */
static int *level, *trail, *trail_lim, tsz, nlim, qhead;
static Clause **reason; static double *act; static int8_t *phase; static char *seen;
static WList *watches; static Clause **learnts; static int nlearnt, caplearnt;
static double var_inc=1.0, cla_inc=1.0;
static long long conflicts=0, decisions=0, props=0;
#define LIT(v,neg) (2*(v)+(neg))
#define VAR(l) ((l)>>1)
#define NEG(l) ((l)^1)
static void wpush(int lit, Clause *c, int blocker){ WList *w=&watches[lit]; if(w->n==w->cap){ w->cap=w->cap?w->cap*2:4; w->w=realloc(w->w,sizeof(Watch)*w->cap);} w->w[w->n].c=c; w->w[w->n].blocker=blocker; w->n++; }
static inline void enqueue(int lit, Clause *from){ val[lit]=1; val[NEG(lit)]=-1; level[VAR(lit)]=nlim; reason[VAR(lit)]=from; trail[tsz++]=lit; }
static Clause *newclause(int *lits,int n,int learnt){ Clause *c=malloc(sizeof(Clause)+sizeof(int)*n); c->sz=n; c->learnt=learnt; c->deleted=0; c->lbd=0; c->act=0; memcpy(c->lits,lits,sizeof(int)*n); return c; }
static void attach(Clause *c){ wpush(c->lits[0],c,c->lits[1]); wpush(c->lits[1],c,c->lits[0]); }
static Clause *propagate(void){
    while(qhead<tsz){
        int p=trail[qhead++]; int fl=NEG(p); WList *ws=&watches[fl]; props++;
        int i=0,j=0;
        while(i<ws->n){
            Watch w=ws->w[i++];
            if(w.c->deleted) continue;
            if(val[w.blocker]==1){ ws->w[j++]=w; continue; }
            Clause *c=w.c; int *L=c->lits;
            if(L[0]==fl){ L[0]=L[1]; L[1]=fl; }
            int first=L[0];
            if(val[first]==1){ ws->w[j].c=c; ws->w[j].blocker=first; j++; continue; }
            int found=0;
            for(int k=2;k<c->sz;k++) if(val[L[k]]!=-1){ L[1]=L[k]; L[k]=fl; wpush(L[1],c,first); found=1; break; }
            if(found) continue;
            ws->w[j].c=c; ws->w[j].blocker=first; j++;
            if(val[first]==-1){ while(i<ws->n) ws->w[j++]=ws->w[i++]; ws->n=j; qhead=tsz; return c; }
            enqueue(first,c);
        }
        ws->n=j;
    }
    return NULL;
}
static void cancel_until(int lv){
    if(nlim<=lv) return;
    for(int i=tsz-1;i>=trail_lim[lv];i--){ int l=trail[i]; int v=VAR(l); phase[v]=(l&1)?0:1; val[l]=0; val[NEG(l)]=0; reason[v]=NULL; }
    tsz=trail_lim[lv]; qhead=tsz; nlim=lv;
}
static void bump_var(int v){ act[v]+=var_inc; if(act[v]>1e100){ for(int i=1;i<=nv;i++) act[i]*=1e-100; var_inc*=1e-100; } }
static void bump_cla(Clause *c){ c->act+=cla_inc; if(c->act>1e20){ for(int i=0;i<nlearnt;i++) learnts[i]->act*=1e-20; cla_inc*=1e-20; } }
static int *learnt_buf, nlb; static int *stackbuf;
static int lit_redundant(int p){      /* local minimisation: p is redundant if every other literal of its reason is seen */
    Clause *r=reason[VAR(p)]; if(!r) return 0;
    for(int k=0;k<r->sz;k++){ int q=r->lits[k]; if(VAR(q)==VAR(p)) continue; if(!seen[VAR(q)] && level[VAR(q)]>0) return 0; }
    return 1;
}
static int analyze(Clause *confl, int *btlevel){
    int pathC=0, p=-1, idx=tsz-1; nlb=1;
    Clause *c=confl;
    do{
        if(c->learnt) bump_cla(c);
        for(int k=(p==-1?0:1);k<c->sz;k++){ int q=c->lits[k]; int v=VAR(q);
            if(!seen[v] && level[v]>0){ bump_var(v); seen[v]=1; if(level[v]>=nlim) pathC++; else learnt_buf[nlb++]=q; } }
        while(!seen[VAR(trail[idx])]) idx--;
        p=trail[idx]; idx--; c=reason[VAR(p)]; seen[VAR(p)]=0; pathC--;
        if(pathC>0 && c==NULL){ fprintf(stderr,"internal error: missing reason\n"); exit(3); }
    } while(pathC>0);
    learnt_buf[0]=NEG(p);
    /* minimise */
    int norig=nlb; memcpy(stackbuf,learnt_buf,sizeof(int)*norig);
    int n2=1; for(int i=1;i<norig;i++){ if(!lit_redundant(stackbuf[i])) learnt_buf[n2++]=stackbuf[i]; }
    for(int i=1;i<norig;i++) seen[VAR(stackbuf[i])]=0;    /* clear the marks of every original literal, kept or removed */
    nlb=n2;
    int bt=0, bi=1; for(int i=1;i<nlb;i++) if(level[VAR(learnt_buf[i])]>bt){ bt=level[VAR(learnt_buf[i])]; bi=i; }
    if(nlb>1){ int t=learnt_buf[1]; learnt_buf[1]=learnt_buf[bi]; learnt_buf[bi]=t; }
    *btlevel=bt; return nlb;
}
static int compute_lbd(int *L,int n){ static int *mark=NULL; static int stamp=0; if(!mark) mark=calloc(nv+2,sizeof(int)); stamp++; int k=0; for(int i=0;i<n;i++){ int lv=level[VAR(L[i])]; if(mark[lv]!=stamp){ mark[lv]=stamp; k++; } } return k; }
static int cmp_cl(const void *a,const void *b){ Clause *x=*(Clause**)a,*y=*(Clause**)b; if(x->lbd!=y->lbd) return y->lbd-x->lbd; if(x->act<y->act) return 1; if(x->act>y->act) return -1; return 0; }
static void reduce_db(void){
    qsort(learnts,nlearnt,sizeof(Clause*),cmp_cl);       /* worst first: high lbd, low activity */
    int half=nlearnt/2, kept=0;
    for(int i=0;i<nlearnt;i++){ Clause *c=learnts[i]; int locked=(val[c->lits[0]]==1 && reason[VAR(c->lits[0])]==c);
        if(i<half && c->lbd>2 && !locked && c->sz>2) c->deleted=1; else learnts[kept++]=c; }
    /* purge watchers of deleted clauses, then free */
    for(int l=0;l<2*nv+2;l++){ WList *w=&watches[l]; int j=0; for(int i=0;i<w->n;i++) if(!w->w[i].c->deleted) w->w[j++]=w->w[i]; w->n=j; }
    /* free: clauses flagged deleted were removed from learnts array above only if kept==..; find them by scanning old array region */
    /* (deleted pointers are no longer in learnts[0..kept); free them via a side list) */
    nlearnt=kept;
}
static double luby(double y,int x){ int size,seq; for(size=1,seq=0;size<x+1;seq++,size=2*size+1); while(size-1!=x){ size=(size-1)>>1; seq--; x=x%size; } return pow(y,seq); }
int main(int argc,char **argv){
    FILE *f=fopen(argv[1],"r"); long long climit=argc>2?atoll(argv[2]):-1; if(!f){ perror("open"); return 2; }
    char line[1<<16]; int *tmp=NULL, tcap=0, tn=0; int hdr=0; Clause **orig=NULL; int norig=0, caporig=0; int trivially_unsat=0;
    int *unit_lits=NULL, nunit=0, capunit=0;
    while(fgets(line,sizeof line,f)){
        if(line[0]=='c') continue;
        if(line[0]=='p'){ sscanf(line,"p cnf %d %d",&nv,&ncl); hdr=1;
            val=calloc(2*nv+2,1); level=calloc(nv+2,sizeof(int)); trail=calloc(nv+2,sizeof(int)); trail_lim=calloc(nv+2,sizeof(int));
            reason=calloc(nv+2,sizeof(Clause*)); act=calloc(nv+2,sizeof(double)); phase=calloc(nv+2,1); seen=calloc(nv+2,1);
            watches=calloc(2*nv+2,sizeof(WList)); learnt_buf=malloc(sizeof(int)*(nv+2)); stackbuf=malloc(sizeof(int)*(nv+2)); continue; }
        char *s=line; int x; int nread;
        while(sscanf(s,"%d%n",&x,&nread)==1){ s+=nread;
            if(x==0){ /* end of clause */
                /* remove duplicates / tautologies */
                int m=0; int taut=0; for(int i=0;i<tn;i++){ int dup=0; for(int j=0;j<m;j++){ if(tmp[j]==tmp[i]) dup=1; if(tmp[j]==NEG(tmp[i])) taut=1; } if(!dup) tmp[m++]=tmp[i]; }
                if(!taut){ if(m==0) trivially_unsat=1; else if(m==1){ if(nunit==capunit){ capunit=capunit?capunit*2:16; unit_lits=realloc(unit_lits,sizeof(int)*capunit);} unit_lits[nunit++]=tmp[0]; }
                    else { Clause *c=newclause(tmp,m,0); attach(c); } }
                tn=0; }
            else { if(tn==tcap){ tcap=tcap?tcap*2:64; tmp=realloc(tmp,sizeof(int)*tcap);} tmp[tn++]=LIT(abs(x),x<0); } }
    }
    (void)hdr; (void)orig; (void)norig; (void)caporig;
    if(trivially_unsat){ printf("s UNSATISFIABLE\n"); return 20; }
    for(int i=0;i<nunit;i++){ int l=unit_lits[i]; if(val[l]==-1){ printf("s UNSATISFIABLE\n"); return 20; } if(val[l]==0) enqueue(l,NULL); }
    if(propagate()){ printf("s UNSATISFIABLE\n"); return 20; }
    int restart_idx=0; long long next_restart=0; long long max_learnts_conf=3000; long long next_reduce=max_learnts_conf;
    learnts=NULL; nlearnt=0; caplearnt=0;
    while(1){
        long long budget=(long long)(luby(2,restart_idx++)*100); long long start=conflicts;
        while(1){
            Clause *confl=propagate();
            if(confl){
                conflicts++;
                if(nlim==0){ printf("s UNSATISFIABLE\nc conflicts %lld decisions %lld\n",conflicts,decisions); return 20; }
                int bt; int n=analyze(confl,&bt);
                cancel_until(bt);
                if(n==1){ enqueue(learnt_buf[0],NULL); }
                else { Clause *c=newclause(learnt_buf,n,1); c->lbd=compute_lbd(c->lits,n); c->act=0; bump_cla(c); attach(c);
                    if(nlearnt==caplearnt){ caplearnt=caplearnt?caplearnt*2:1024; learnts=realloc(learnts,sizeof(Clause*)*caplearnt);} learnts[nlearnt++]=c; enqueue(learnt_buf[0],c); }
                var_inc/=0.95; cla_inc/=0.999;
                if(climit>0 && conflicts>=climit){ printf("s UNKNOWN\nc conflicts %lld\n",conflicts); return 0; }
                if(conflicts%100000==0) fprintf(stderr,"c conflicts %lld learnts %d decisions %lld\n",conflicts,nlearnt,decisions);
            } else {
                if(conflicts-start>=budget){ cancel_until(0); break; }
                if(conflicts>=next_reduce){ reduce_db(); max_learnts_conf=(long long)(max_learnts_conf*1.1)+300; next_reduce=conflicts+max_learnts_conf; }
                int best=0; double ba=-1; for(int v=1;v<=nv;v++) if(val[2*v]==0 && act[v]>ba){ ba=act[v]; best=v; }
                if(!best){ printf("s SATISFIABLE\nv"); for(int v=1;v<=nv;v++) printf(" %d",val[2*v]==1?v:-v); printf(" 0\nc conflicts %lld decisions %lld\n",conflicts,decisions); return 10; }
                decisions++; trail_lim[nlim++]=tsz; enqueue(LIT(best,phase[best]?0:1),NULL);
            }
        }
    }
}
