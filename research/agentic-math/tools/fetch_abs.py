import subprocess, sys, os
out = sys.argv[1]
ids = "2602.10177,2601.22401,2606.24902,2608.19009,2611.12943,2606.13566,2603.15617,2610.17184,2610.33259,2608.23691,2609.15983,2610.05367,2610.20573,2603.18813,2609.32264,2606.10799,2604.03789,2602.13695,2610.09769,2607.09474".split(",")
url = "https://export.arxiv.org/api/query?max_results=40&id_list=" + ",".join(ids)
subprocess.run(["curl","-sSL","--max-time","60","-A","research-notes/0.1","-o",os.path.join(out,"abs.xml"),url],check=True)
