import sys, glob, os, re
import xml.etree.ElementTree as ET
ns = {"a": "http://www.w3.org/2005/Atom"}
seen = {}
for f in sorted(glob.glob(os.path.join(sys.argv[1], "*.xml"))):
    name = os.path.basename(f)[:-4]
    try:
        root = ET.parse(f).getroot()
    except Exception as e:
        print(name, "PARSE ERROR", e); continue
    n = 0
    print("\n##", name)
    for e in root.findall("a:entry", ns):
        aid = re.sub(r"v\d+$", "", e.find("a:id", ns).text.rsplit("/", 1)[-1])
        date = e.find("a:published", ns).text[:10]
        title = " ".join(e.find("a:title", ns).text.split())
        if date < "2025-01-01": continue
        tag = "" if aid not in seen else " (dup)"
        seen.setdefault(aid, (date, title))
        print(f"  {aid} {date} {title[:105]}{tag}")
        n += 1
        if n >= 14: break
