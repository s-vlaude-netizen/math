import sys, re, os, html
root, aid = sys.argv[1], sys.argv[2]
raw = open(os.path.join(root, aid, "page.html"), encoding="utf-8", errors="replace").read()
raw = re.sub(r"(?s)<(script|style).*?</\1>", "", raw)
# split into a flat token stream of headings and paragraphs, in document order
toks = []
for m in re.finditer(r"(?s)<h([23])[^>]*>(.*?)</h\1>|<p[^>]*>(.*?)</p>", raw):
    if m.group(1):
        t = html.unescape(" ".join(re.sub(r"(?s)<[^>]+>", "", m.group(2)).split())); toks.append(("h", int(m.group(1)), t))
    else:
        t = html.unescape(" ".join(re.sub(r"(?s)<[^>]+>", "", m.group(3)).split()))
        if t: toks.append(("p", 0, t))
for spec in sys.argv[3:]:
    pat, cap = spec.rsplit("::", 1); cap = int(cap)
    out, on, lvl = [], False, 0
    for k, l, t in toks:
        if k == "h":
            if on and l <= lvl: break
            if not on and re.search(pat, t): on, lvl = True, l; head = t
        elif on: out.append(t)
    body = " ".join(out)
    print(f"\n--- [{aid}] {head if out else pat} ---\n{body[:cap]}{'…' if len(body) > cap else ''}")
