import sys, re, os, html
root = sys.argv[1]
KEYS = re.compile(r"(limitation|future work|future direction|open question|open problem|we leave|remains? open|caveat|false positive|rediscover|previously (known|published)|prior literature|verification (bottleneck|burden)|bottleneck|human (expert|verification|review))", re.I)
for aid in sys.argv[2:]:
    p = os.path.join(root, aid, "page.html")
    raw = open(p, encoding="utf-8", errors="replace").read()
    raw = re.sub(r"(?s)<(script|style).*?</\1>", "", raw)
    paras = []
    for m in re.finditer(r"(?s)<p[^>]*>(.*?)</p>", raw):
        t = re.sub(r"(?s)<[^>]+>", "", m.group(1)); t = html.unescape(" ".join(t.split()))
        if 140 <= len(t) <= 1800: paras.append(t)
    n = len(paras)
    hits = [(i, t) for i, t in enumerate(paras) if KEYS.search(t) and i > n * 0.55]   # discussion/limitations live late in a paper
    print(f"\n=== {aid}: {n} paragraphs, {len(hits)} late keyword hits")
    for i, t in hits[:4]:
        print(f"  [{i}/{n}] {t[:520]}{'…' if len(t) > 520 else ''}")
