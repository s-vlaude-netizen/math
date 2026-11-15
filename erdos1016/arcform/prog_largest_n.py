from itertools import combinations
def pancyclic(n, c):
    adj = [{(v-1) % n, (v+1) % n} for v in range(n)]
    for i, j in c: adj[i].add(j); adj[j].add(i)
    L = set()
    def dfs(s, v, seen):
        for w in adj[v]:
            if w == s and len(seen) > 2: L.add(len(seen))
            elif w > s and w not in seen: dfs(s, w, seen | {w})
    for s in range(n): dfs(s, s, {s})
    return L >= set(range(3, n+1))
def a(k):  # largest n with a pancyclic graph on n vertices and n+k edges (n <= 2^(k+1)+1 by Bondy)
    for n in range(2**(k+1) + 1, 2, -1):
        ch = [(i, j) for i in range(n) for j in range(i+2, n) if (i, j) != (0, n-1)]
        if any(pancyclic(n, c) for c in combinations(ch, k)): return n
print([a(k) for k in range(0, 3)])
