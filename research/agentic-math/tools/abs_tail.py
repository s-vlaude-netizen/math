import sys, re, os
import xml.etree.ElementTree as ET
ns={"a":"http://www.w3.org/2005/Atom"}
want=set(sys.argv[2:])
for e in ET.parse(os.path.join(sys.argv[1],"abs.xml")).getroot().findall("a:entry",ns):
    aid=re.sub(r"v\d+$","",e.find("a:id",ns).text.rsplit("/",1)[-1])
    if aid in want:
        s=" ".join(e.find("a:summary",ns).text.split())
        print(f"\n[{aid}] …{s[600:1500]}")
