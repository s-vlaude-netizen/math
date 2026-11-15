# verify.py DATADIR SECONDS : run every program against the OEIS data (downloaded JSON in DATADIR)
import json, os, sys, time, multiprocessing as mp
from programs import PROGRAMS

def worker(sid, src, idx, q):
    ns = {}
    exec(src, ns)
    f = ns[sid]
    for i in idx:
        q.put((i, f(i)))

def check(sid, datadir, budget):
    r = json.load(open(os.path.join(datadir, sid + ".json")))
    r = (r if isinstance(r, list) else r["results"])[0]
    off = int(r["offset"].split(",")[0])
    data = [int(x) for x in r["data"].split(",")]
    q = mp.Queue()
    p = mp.Process(target=worker, args=(sid, PROGRAMS[sid], range(off, off + len(data)), q))
    t0 = time.time(); p.start()
    got = {}
    while time.time() - t0 < budget and len(got) < len(data):
        try:
            i, v = q.get(timeout=1)
            got[i] = v
        except Exception:
            if not p.is_alive(): break
    p.terminate()
    bad = [(i, got[i], data[i - off]) for i in sorted(got) if got[i] != data[i - off]]
    return len(got), len(data), off, bad, time.time() - t0

if __name__ == "__main__":
    datadir, budget = sys.argv[1], float(sys.argv[2])
    only = sys.argv[3:] or sorted(PROGRAMS)
    for sid in only:
        n, total, off, bad, t = check(sid, datadir, budget)
        status = "OK" if not bad and n else ("MISMATCH " + str(bad[:3]) if bad else "NO TERMS")
        print(f"{sid}: {status}  checked {n} of {total} terms (n = {off}..{off + n - 1}), {t:.0f} s", flush=True)
