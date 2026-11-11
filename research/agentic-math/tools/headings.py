import sys, re, os, html
root = sys.argv[1]
for aid in sys.argv[2:]:
    raw = open(os.path.join(root, aid, "page.html"), encoding="utf-8", errors="replace").read()
    hs = []
    for m in re.finditer(r"(?s)<h([23])[^>]*>(.*?)</h\1>", raw):
        t = html.unescape(" ".join(re.sub(r"(?s)<[^>]+>", "", m.group(2)).split()))
        if t and len(t) < 90: hs.append(t)
    print(f"\n=== {aid} headings: " + " | ".join(hs[:40]))
