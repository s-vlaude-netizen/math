import subprocess, sys, time, urllib.parse, os
# usage: fetch_arxiv.py OUTDIR  (queries are listed below; one raw Atom file per query)
out = sys.argv[1]
queries = {
 "q1_agents_math_research": 'all:"mathematical research" AND (all:agent OR all:agentic) AND (all:LLM OR all:"language model")',
 "q2_erdos": 'all:Erdos AND all:problems AND (all:LLM OR all:"language model" OR all:AI)',
 "q3_alphaevolve": 'all:AlphaEvolve',
 "q4_open_problem_bench": 'all:"open problems" AND all:benchmark AND all:mathematics AND (all:LLM OR all:"language model")',
 "q5_autoformalization": 'all:autoformalization AND (all:agent OR all:fidelity OR all:faithful)',
 "q6_survey": 'all:"mathematics" AND all:"AI" AND (all:survey OR all:perspective OR all:roadmap) AND (all:"research" OR all:discovery)',
 "q7_novelty": 'all:mathematics AND all:"literature search" AND (all:LLM OR all:"language model")',
 "q8_verification": 'all:"research-level" AND all:mathematics AND all:proof AND all:verification AND (all:LLM OR all:"language model")',
}
for name, q in queries.items():
    url = "https://export.arxiv.org/api/query?" + urllib.parse.urlencode(
        {"search_query": q, "start": 0, "max_results": 25, "sortBy": "submittedDate", "sortOrder": "descending"})
    r = subprocess.run(["curl", "-sSL", "--max-time", "40", "-A", "research-notes/0.1", "-o",
                        os.path.join(out, name + ".xml"), url], capture_output=True, text=True)
    print(name, "rc", r.returncode, os.path.getsize(os.path.join(out, name + ".xml")), "bytes")
    time.sleep(3.5)
