import subprocess, sys, os, time
root = sys.argv[1]
for aid in sys.argv[2:]:
    d = os.path.join(root, aid); os.makedirs(d, exist_ok=True)
    r = subprocess.run(["curl","-sSL","--max-time","60","-A","research-notes/0.1","-o",os.path.join(d,"page.html"),
                        "-w","%{http_code}","https://arxiv.org/html/"+aid], capture_output=True, text=True)
    print(aid, "HTTP", r.stdout, os.path.getsize(os.path.join(d,"page.html")), "bytes")
    time.sleep(2)
