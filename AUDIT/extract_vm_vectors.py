#!/usr/bin/env python3
"""Extract the reference's VM-level golden sequences into JSON.

Source of truth: src/Calculator.Tests/StandardCalculatorViewModelTests.cs
(verbatim, unmodified reference @ d125246a).

Each test builds a `TestItem[]` of (command, expected display, expected
expression) and hands it to ValidateViewModelByCommands(viewModel, items,
doReset), which optionally resets (Clear, ClearEntry, ClearMemory) and then
asserts viewModel.DisplayValue after every command. Those sequences replay
exactly against the port's StandardCalculatorViewModel, so they are emitted as
data.

The reference builds some expectations by concatenation, e.g. "13" +
_decimalSeparator. Those become the token {DEC} and are resolved at replay time
from the active localization, so the vectors stay locale-independent.

Methods that need bespoke view-model setup (mode switches, radix changes,
memory preloading) are reported separately rather than replayed with the wrong
preconditions.

Emits: AUDIT/evidence/golden_vm_vectors.json
"""
import json
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src/Calculator.Tests/StandardCalculatorViewModelTests.cs")
OUT = os.path.join(ROOT, "AUDIT/evidence/golden_vm_vectors.json")

text = open(SRC, encoding="utf-8", errors="replace").read()

methods = {}
for m in re.finditer(r"public\s+(?:async\s+)?(?:void|Task)\s+(\w+)\s*\(\s*\)\s*\{", text):
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

# The whole `var items = new TestItem[] { ... };` declaration, any name.
ARRAY = re.compile(r"(?:var|TestItem\[\])\s*(\w+)\s*=\s*new\s+TestItem\[\]\s*\{.*?\};", re.S)

# One element, allowing string concatenation with _decimalSeparator.
ITEM = re.compile(
    r"new\(\s*NumbersAndOperatorsEnum\.(\w+)\s*,\s*(.+?)\s*,\s*(.+?)\s*\)\s*(?:,|\n)")


def literal(expr, token="{DEC}"):
    """Turn a C# string expression into a literal, or None if not constant."""
    expr = expr.strip().rstrip(",").strip()
    parts, rest = [], expr
    while rest:
        m = re.match(r'^\s*"((?:[^"\\]|\\.)*)"\s*\+\s*', rest)
        if m:
            parts.append(m.group(1))
            rest = rest[m.end():]
            continue
        m = re.match(r'^\s*"((?:[^"\\]|\\.)*)"\s*$', rest)
        if m:
            parts.append(m.group(1))
            rest = ""
            continue
        m = re.match(r"^\s*_decimalSeparator\s*\+\s*", rest)
        if m:
            parts.append(token)
            rest = rest[m.end():]
            continue
        m = re.match(r"^\s*\+\s*", rest)
        if m:
            rest = rest[m.end():]
            continue
        return None
    return parts and "".join(parts).replace('\\"', '"')


vectors, skipped = [], []
for name, body in methods.items():
    arrays = {m.group(1): m.group(0) for m in ARRAY.finditer(body)}
    if not arrays:
        continue
    calls = re.findall(
        r"ValidateViewModelByCommands\s*\(\s*\w+\s*,\s*(\w+)\s*(?:,\s*(true|false)\s*)?\)", body)
    if not calls:
        continue

    # Parse every array, in source order.
    parsed = {}
    for arr_name, arr_src in arrays.items():
        inner = arr_src[arr_src.index("{") + 1: arr_src.rindex("}")]
        steps = []
        ok = True
        for cmd, disp, _expr in ITEM.findall(inner):
            d = literal(disp)
            if d is None:
                ok = False
                break
            steps.append({"command": cmd, "expected": d})
        if ok:
            parsed[arr_name] = steps

    used = {a for a, _ in calls if a in parsed}
    if not used:
        skipped.append({"method": name, "reason": "no constant-only TestItem array"})
        continue

    # Everything outside the arrays and the Validate calls is extra setup.
    leftover = ARRAY.sub("", body)
    leftover = re.sub(r"ValidateViewModelByCommands[^;]*;", "", leftover)
    leftover = re.sub(r"//[^\n]*", "", leftover)
    leftover = " ".join(leftover.split())
    if leftover:
        skipped.append({"method": name, "reason": f"extra setup: {leftover[:160]}"})
        continue

    for arr_name, reset in calls:
        steps = parsed[arr_name]
        vectors.append({
            "origin": f"src/Calculator.Tests/StandardCalculatorViewModelTests.cs::{name}",
            "doReset": reset != "false",
            "steps": steps,
        })

meta = {
    "source": "src/Calculator.Tests/StandardCalculatorViewModelTests.cs",
    "commit": "d125246a4e19842ce1332e6c7839cf0e110027d8",
    "contract": ("ValidateViewModelByCommands(vm, items, doReset): if doReset -> "
                 "ButtonPressed(Clear), ButtonPressed(ClearEntry), ClearMemoryCommand(null); "
                 "then for each item: ButtonPressed(command) and assert DisplayValue. "
                 "'{DEC}' is resolved from the active localization's decimal separator."),
    "count": len(vectors),
    "assertions": sum(len(v["steps"]) for v in vectors),
    "skipped_needing_setup": skipped,
    "vectors": vectors,
}
with open(OUT, "w", encoding="utf-8") as fh:
    json.dump(meta, fh, indent=1, ensure_ascii=False)

print(f"extracted {len(vectors)} sequences / {meta['assertions']} assertions from {len(methods)} methods")
print(f"skipped (need bespoke setup): {len(skipped)}")
print(f"-> {OUT}")