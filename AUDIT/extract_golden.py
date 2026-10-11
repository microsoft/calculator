#!/usr/bin/env python3
"""Extract the reference's engine golden vectors into JSON.

Source of truth: src/CalculatorUnitTests/CalculatorManagerTest.cpp (verbatim,
unmodified reference). Each TestDriver::Test(expectedPrimary, expectedExpression,
commands[], cleanup=true, isScientific=false) is replayed against the port's
engine as an assertion.

Emits: AUDIT/evidence/golden_engine_vectors.json
"""
import json
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src/CalculatorUnitTests/CalculatorManagerTest.cpp")
CMD_H = os.path.join(ROOT, "src/CalcManager/Command.h")
OUT = os.path.join(ROOT, "AUDIT/evidence/golden_engine_vectors.json")

# Command name -> numeric id, straight out of the engine's own header.
cmd_ids = {}
_h = open(CMD_H, encoding="utf-8", errors="replace").read()
for m in re.finditer(r"\b((?:Command|Mode)\w+)\s*=\s*(?:CMD_\w+\s*=\s*)?(\d+)", _h):
    cmd_ids.setdefault(m.group(1), int(m.group(2)))

text = open(SRC, encoding="utf-8", errors="replace").read()

# Split into TEST_METHOD bodies so every vector keeps its origin.
methods = {}
for m in re.finditer(r"void\s+CalculatorManagerTest::(\w+)\(\)\s*\{", text):
    name = m.group(1)
    start = m.end()
    depth, i = 1, start
    while i < len(text) and depth:
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
        i += 1
    methods[name] = text[start:i - 1]

# `TestDriver::Test(L"primary", L"expr", commandsN, cleanup?, isScientific?);`
call = re.compile(
    r"TestDriver::Test\(\s*L\"((?:[^\"\\]|\\.)*)\"\s*,\s*L\"((?:[^\"\\]|\\.)*)\"\s*,\s*(\w+)\s*"
    r"(?:,\s*(true|false)\s*)?(?:,\s*(true|false)\s*)?\)",
    re.S)

unescape = lambda s: (s.replace(r"\)", ")").replace(r"\\", "\\")
                       .replace(r"\"", '"').replace(r"\x00D7", "×")
                       .replace(r"\x00F7", "÷").replace(r"\x221A", "√"))


def decode_cpp_wstring(raw):
    """Turn a C++ wide-literal body into the characters the engine produces."""
    out, i = [], 0
    while i < len(raw):
        if raw[i] == "\\" and i + 1 < len(raw):
            nxt = raw[i + 1]
            if nxt == "x" and i + 5 < len(raw):
                # C++ \x escapes are an unbounded run of hex digits; the
                # reference always writes exactly four (\x00D7, \x221A, ...).
                j = i + 2
                while j < len(raw) and raw[j] in "0123456789abcdefABCDEF":
                    j += 1
                out.append(chr(int(raw[i + 2:j], 16)))
                i = j
                continue
            mapping = {"n": "\n", "t": "\t", "r": "\r", "\\": "\\", '"': '"'}
            if nxt in mapping:
                out.append(mapping[nxt])
                i += 2
                continue
        out.append(raw[i])
        i += 1
    return "".join(out)


vectors, unresolved = [], set()
for method, body in methods.items():
    arrays = dict(re.findall(r"Command\s+(\w+)\s*\[\]\s*=\s*\{(.*?)\}", body, re.S))
    for m in call.finditer(body):
        primary_raw, expr_raw, arr, cleanup, sci = m.groups()
        arr_body = arrays.get(arr)
        if arr_body is None:
            unresolved.add(f"{method}:{arr}")
            continue
        names = re.findall(r"Command::(\w+)", arr_body)
        ids, bad = [], False
        for n in names:
            if n not in cmd_ids:
                bad = True
                break
            ids.append(cmd_ids[n])
        if bad:
            unresolved.add(f"{method}:{arr}:cmd")
            continue
        vectors.append({
            "origin": f"src/CalculatorUnitTests/CalculatorManagerTest.cpp::{method}",
            "commands": ids,
            "expectedPrimary": decode_cpp_wstring(primary_raw),
            "expectedExpression": None if expr_raw == "N/A" else decode_cpp_wstring(expr_raw),
            "cleanup": (cleanup != "false"),
            "scientific": (sci == "true"),
        })

meta = {
    "source": "src/CalculatorUnitTests/CalculatorManagerTest.cpp",
    "commit": "d125246a4e19842ce1332e6c7839cf0e110027d8",
    "contract": ("TestDriver::Test(expectedPrimary, expectedExpression, commands[], "
                 "cleanup=true, isScientific=false): Reset(); if isScientific -> ModeScientific; "
                 "send commands until NULL; assert primary, and expression unless 'N/A'"),
    "count": len(vectors),
    "unresolved": sorted(unresolved),
    "vectors": vectors,
}
os.makedirs(os.path.dirname(OUT), exist_ok=True)
with open(OUT, "w", encoding="utf-8") as fh:
    json.dump(meta, fh, indent=1, ensure_ascii=False)

print(f"extracted {len(vectors)} golden vectors from {len(methods)} reference test methods")
if unresolved:
    print(f"unresolved ({len(unresolved)}): {sorted(unresolved)[:10]}")
print(f"-> {OUT}")
sci = sum(1 for v in vectors if v["scientific"])
print(f"scientific-mode vectors: {sci}, standard/programmer: {len(vectors)-sci}")