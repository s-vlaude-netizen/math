import sys, re, os
import xml.etree.ElementTree as ET
ns={"a":"http://www.w3.org/2005/Atom"}
root=ET.parse(os.path.join(sys.argv[1],"abs.xml")).getroot()
for e in root.findall("a:entry",ns):
    aid=re.sub(r"v\d+$","",e.find("a:id",ns).text.rsplit("/",1)[-1])
    au=[x.find("a:name",ns).text for x in e.findall("a:author",ns)]
    t=" ".join(e.find("a:title",ns).text.split())
    s=" ".join(e.find("a:summary",ns).text.split())
    print(f"\n[{aid}] {t}\n  {au[0]}{' et al.' if len(au)>1 else ''} ({len(au)} authors), {e.find('a:published',ns).text[:10]}\n  {s[:640]}{'…' if len(s)>640 else ''}")
