# Agentic LLMs for mathematical research, as a research field

This directory starts a second line of work next to the Erdős-frontier
computations. The Erdős work asked: *can an agent push the exact data on open
problems further?* This line asks about the field that question belongs to:
what is known about agents doing mathematical research, what is still open
there, and can small experiments move any of it.

| File | What it is |
|---|---|
| `handoff.md` | What I was working on before, what is unfinished, how to resume it |
| `literature-round1.md` | First reading round: about 25 papers, by theme, with a note on how much of each I read |
| `open-problems.md` | Seven open problems drawn from that reading, first ideas, and a suggested order |
| `tools/` | The scripts used for the literature pass |

## Where things stand (2026-11-11)

* Round 1 is literature only. No experiment has been run for this line yet.
* The previous work is unchanged, except for one pending decision: whether a
  pancyclic graph on 41 vertices with 5 extra edges exists (`handoff.md`).
* Every idea in `open-problems.md` is untested and its novelty is unchecked.

## Reproducing the literature pass

The scripts download files from the internet, and those files are untrusted data.
Keep each download in its own empty directory, keep the scripts somewhere else,
pass paths as arguments, and run Python with `-I`:

```
mkdir raw && python3 -I tools/fetch_arxiv.py raw && python3 -I tools/parse_arxiv.py raw
python3 -I tools/fetch_html.py html 2602.10177 && python3 -I tools/sections.py html 2602.10177 "5\.3 Weaknesses::2300"
```

`fetch_abs.py` has its list of paper ids written into it; edit that line.
