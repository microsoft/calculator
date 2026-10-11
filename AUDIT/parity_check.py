#!/usr/bin/env python3
"""AUDIT parity check: the port's hand-transported DATA vs the unmodified reference.

Two independent transcriptions are checked here, because both are large enough
that a single typo changes a number a user sees:

  1. Unit converter data (src/Calculator.ViewModels/DataLoaders/UnitConverterDataLoader.cs
     -> linux/src/Calculator.Linux/DataLoaders/UnitConverterDataLoader.cs)
  2. Resource strings (Resources.resw + CEngineStrings.resw
     -> linux/src/Calculator.Linux/Services/GeneratedResources.cs)

Exit code 0 only if everything matches exactly.
"""
import os
import re
import sys
from collections import Counter
import xml.etree.ElementTree as ET

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REF_VM = os.path.join(ROOT, "src/Calculator.ViewModels/DataLoaders/UnitConverterDataLoader.cs")
PORT_VM = os.path.join(ROOT, "linux/src/Calculator.Linux/DataLoaders/UnitConverterDataLoader.cs")
REF_RES = os.path.join(ROOT, "src/Calculator/Resources/en-US/Resources.resw")
REF_CE = os.path.join(ROOT, "src/Calculator/Resources/en-US/CEngineStrings.resw")
PORT_GEN = os.path.join(ROOT, "linux/src/Calculator.Linux/Services/GeneratedResources.cs")

failures = []


def read(p):
    with open(p, encoding="utf-8", errors="replace") as fh:
        return fh.read().replace("\r", "")


def check(label, cond, detail=""):
    print(("PASS  " if cond else "FAIL  ") + label + ((" :: " + detail) if detail else ""))
    if not cond:
        failures.append(label)


# ---------------------------------------------------------------- converter
ref, port = read(REF_VM), read(PORT_VM)
consts = lambda s: {m.group(1): int(m.group(2)) for m in
                    re.finditer(r"private const int (\w+)\s*=\s*(\d+)\s*;", s)}
rc, pc = consts(ref), consts(port)
shared = set(rc) & set(pc)
check("converter: unit-id constant count equal", len(rc) == len(pc), f"{len(rc)} vs {len(pc)}")
check("converter: no missing unit ids", not (set(rc) - set(pc)), str(sorted(set(rc) - set(pc))[:8]))
check("converter: no extra unit ids", not (set(pc) - set(rc)), str(sorted(set(pc) - set(rc))[:8]))
bad = {k: (rc[k], pc[k]) for k in shared if rc[k] != pc[k]}
check("converter: unit-id values identical", not bad, str(bad))

addunit = lambda s: [(m.group(1), m.group(2), m.group(3), m.group(4), int(m.group(5)))
                     for m in re.finditer(
                         r'AddUnit\(\s*ViewMode\.(\w+)\s*,\s*(\w+)\s*,\s*"([^"]*)"\s*,'
                         r'\s*"([^"]*)"\s*,\s*(\d+)\s*\)', s)]
ra, pa = addunit(ref), addunit(port)
check("converter: AddUnit calls identical", set(ra) == set(pa),
      f"ref={len(ra)} port={len(pa)}")

pair = re.compile(r"\{\s*(\w+)\s*,\s*(-?\d+(?:\.\d+)?(?:[eE][-+]?\d+)?)\s*\}")
rp, pp = pair.findall(ref), pair.findall(port)
check("converter: conversion factors identical", set(rp) == set(pp),
      f"ref={len(rp)} port={len(pp)}")

# --------------------------------------------------------------- resources
def unesc(s):
    """Undo C# string-literal escaping before comparing."""
    return (s.replace('\\"', '"').replace("\\\\", "\\")
             .replace("\\n", "\n").replace("\\'", "'"))


resw = {d.get("name"): d.findtext("value") for d in ET.parse(REF_RES).getroot().iter("data")}
ceng = {d.get("name"): d.findtext("value") for d in ET.parse(REF_CE).getroot().iter("data")}

lines = read(PORT_GEN).split("\n")
strings_start = next(i for i, l in enumerate(lines) if "Strings = new" in l)
ceng_start = next(i for i, l in enumerate(lines) if "CEngineStrings = new" in l)


def table(a, b):
    out = []
    for l in lines[a:b]:
        m = re.match(r'\s*\{\s*"((?:[^"\\]|\\.)*)"\s*,\s*"((?:[^"\\]|\\.)*)"\s*\}', l)
        if m:
            out.append((m.group(1), unesc(m.group(2))))
    return out


ts, tc = table(strings_start, ceng_start), table(ceng_start, len(lines))
ps, pcx = dict(ts), dict(tc)
miss = [k for k in resw if k not in ps]
mis = {k: (resw[k], ps[k]) for k in resw if k in ps and resw[k] != ps[k]}
check("resources: all Resources.resw keys present", not miss, f"{len(resw)} keys, missing {len(miss)}")
check("resources: Resources.resw values identical", not mis, str(list(mis.items())[:5]))
mis2 = {k: (ceng[k], pcx[k]) for k in ceng if k in pcx and ceng[k] != pcx[k]}
check("resources: all CEngineStrings keys present",
      not [k for k in ceng if k not in pcx], f"{len(ceng)} keys")
check("resources: CEngineStrings values identical", not mis2, str(list(mis2.items())[:5]))
dup = sum(1 for _, c in Counter(ts).items() if c > 1) + sum(1 for _, c in Counter(tc).items() if c > 1)
check("resources: no duplicate keys inside either table", dup == 0, f"{dup} duplicates")

print()
if failures:
    print(f"{len(failures)} PARITY CHECK(S) FAILED")
    sys.exit(1)
print("ALL PARITY CHECKS PASSED")