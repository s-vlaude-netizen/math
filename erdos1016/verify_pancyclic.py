#!/usr/bin/env python3
"""
Independent verification for the Erdos-1016 / A105206 computation.

The C searcher reads the cycle spectrum off the cycle space (XOR of at most
2^(k+1)-1 fundamental cycles).  This script instead enumerates *all* simple
cycles of the graph directly with networkx and compares, so a bug in the
cycle-space argument would show up immediately.

Usage:
    python3 verify_pancyclic.py witness  <n> "(0,2),(0,20),(1,18),(8,20)"
    python3 verify_pancyclic.py crosscheck            # randomised comparison
    python3 verify_pancyclic.py bruteforce <n> <k>    # slow independent search
"""
import sys
import itertools
import random
import networkx as nx


def build(n, chords):
    G = nx.Graph()
    G.add_nodes_from(range(n))
    for i in range(n):
        G.add_edge(i, (i + 1) % n)
    for u, v in chords:
        G.add_edge(u, v)
    return G


def spectrum_bruteforce(G):
    """All cycle lengths, by direct enumeration of simple cycles."""
    return {len(c) for c in nx.simple_cycles(G)}


def spectrum_cyclespace(n, chords):
    """The method the C program uses: XOR over the cycle space."""
    edges = [(i, i + 1) for i in range(n - 1)] + [(0, n - 1)] + list(chords)
    idx = {}
    for i, e in enumerate(edges):
        idx[frozenset(e)] = i
    fund = [frozenset(range(n))]              # the Hamiltonian cycle
    for j, (u, v) in enumerate(chords):
        f = set(range(u, v)) | {n + j}        # path edges [u,v) plus the chord
        fund.append(frozenset(f))
    lens = set()
    for mask in range(1, 1 << len(fund)):
        s = set()
        for b in range(len(fund)):
            if mask >> b & 1:
                s ^= fund[b]
        deg = {}
        for e in s:
            for x in edges[e]:
                deg[x] = deg.get(x, 0) + 1
        if not s or any(d != 2 for d in deg.values()):
            continue
        # connected?  walk it
        adj = {}
        for e in s:
            a, b = edges[e]
            adj.setdefault(a, []).append(b)
            adj.setdefault(b, []).append(a)
        start = next(iter(adj))
        prev, cur, steps = None, start, 0
        while True:
            nxt = adj[cur][0] if adj[cur][0] != prev else adj[cur][1]
            prev, cur = cur, nxt
            steps += 1
            if cur == start:
                break
        if steps == len(s):
            lens.add(steps)
    return lens


def is_pancyclic(n, chords, method="brute"):
    if method == "brute":
        sp = spectrum_bruteforce(build(n, chords))
    else:
        sp = spectrum_cyclespace(n, chords)
    return all(L in sp for L in range(3, n + 1)), sp


def parse(s):
    s = s.strip().strip("[]")
    out = []
    for part in s.split("),"):
        part = part.strip().strip("()")
        if not part:
            continue
        u, v = part.split(",")
        out.append((int(u), int(v)))
    return out


if __name__ == "__main__":
    cmd = sys.argv[1]

    if cmd == "witness":
        n = int(sys.argv[2])
        chords = parse(sys.argv[3]) if len(sys.argv) > 3 else []
        ok, sp = is_pancyclic(n, chords)
        missing = [L for L in range(3, n + 1) if L not in sp]
        print(f"n={n}  edges={n + len(chords)}  chords={chords}")
        print(f"  cycle lengths present : {sorted(sp)}")
        print(f"  missing in 3..{n}      : {missing}")
        print("  PANCYCLIC" if ok else "  NOT pancyclic")
        sys.exit(0 if ok else 1)

    elif cmd == "crosscheck":
        random.seed(20260904)
        bad = 0
        for trial in range(4000):
            n = random.randint(6, 16)
            k = random.randint(1, 5)
            allch = [(u, v) for u in range(n) for v in range(u + 2, n)
                     if not (u == 0 and v == n - 1)]
            if len(allch) < k:
                continue
            chords = random.sample(allch, k)
            a = spectrum_bruteforce(build(n, chords))
            b = spectrum_cyclespace(n, sorted(chords))
            if a != b:
                bad += 1
                print("MISMATCH", n, chords, sorted(a), sorted(b))
        print(f"crosscheck done, {bad} mismatches out of 4000")
        sys.exit(1 if bad else 0)

    elif cmd == "bruteforce":
        # completely independent exhaustive search (slow; small n only)
        n, k = int(sys.argv[2]), int(sys.argv[3])
        allch = [(u, v) for u in range(n) for v in range(u + 2, n)
                 if not (u == 0 and v == n - 1)]
        for chords in itertools.combinations(allch, k):
            ok, _ = is_pancyclic(n, list(chords))
            if ok:
                print(f"n={n} k={k} FOUND {chords}")
                sys.exit(10)
        print(f"n={n} k={k} IMPOSSIBLE (independent brute force)")
        sys.exit(20)
