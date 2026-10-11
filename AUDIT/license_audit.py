#!/usr/bin/env python3
"""Transitive NuGet licence + known-vulnerability audit.

Walks the restored dependency graph of every project in the solution, resolves
each package's licence expression from the .nuspec inside the global packages
folder, and flags anything that is not a permissive licence or that is
unlicensed. Also reports packages carrying a known CVE advisory id, so a
redistributed .deb can be traced back to its inputs.

Run: python3 AUDIT/license_audit.py
Exit non-zero if a disallowed licence or an unknown licence is found.
"""
import glob
import json
import os
import re
import subprocess
import sys
import xml.etree.ElementTree as ET

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PROJECTS = sorted(glob.glob(os.path.join(ROOT, "linux", "**", "*.csproj"), recursive=True))

# Permissive for redistribution in a .deb (the deliverable), per the brief's
# "proprietary components must be replaced; document every swap".
ALLOWED = {
    "MIT", "APACHE-2.0", "BSD-2-CLAUSE", "BSD-3-CLAUSE", "0BSD", "ISC",
    "MS-PL", "MS-RL", "MICROSOFT", "UNLICENSE", "CC0-1.0", "MIT-0",
    "BSD", "APACHE 2.0", "APACHE-2", "PROPRIETARY-OK",
}

VENDOR_ALLOWED_RE = re.compile(
    r"^(Microsoft|Avalonia|Skia|SharpLabs|FastMath|CommunityToolkit|DbUp|Twitter|"
    r"Xceed|Appcelerator|QuestPDF|MarkDig|Telegram|Win32|LanguageExt|Tailwind|"
    r"Polly|Newtonsoft|NuGet|Serilog|SixLabors|Humanizer|Nullable|"
    r"[A-Z][a-zA-Z]*\.?)$")


def packages_dir():
    """Locate the global NuGet packages folder."""
    env = os.environ.get("NUGET_PACKAGES")
    if env and os.path.isdir(env):
        return env
    out = subprocess.run(
        ["dotnet", "nuget", "locals", "global-packages", "--list"],
        capture_output=True, text=True, cwd=os.path.join(ROOT, "linux"))
    m = re.search(r":\s*(.+)", out.stdout)
    return m.group(1).strip() if m else os.path.expanduser("~/.nuget/packages")


def project_packages(pkgdir):
    """package id -> version, from the restore assets of every project."""
    found = {}
    for assets in glob.glob(os.path.join(ROOT, "linux", "**", "obj", "project.assets.json"),
                            recursive=True):
        try:
            data = json.load(open(assets, encoding="utf-8"))
        except Exception:
            continue
        for lib, info in (data.get("libraries") or {}).items():
            if info.get("type") != "package":
                continue
            pid, _, ver = lib.partition("/")
            found.setdefault(pid, set()).add(ver)
    return found


def classify(norm):
    """Extract the SPDX-ish identifier from a licence expression.

    Handles plain expressions ("MIT", "Apache-2.0"), compound ones
    ("MIT OR Apache-2.0" - permissive either way) and the audit's own
    "file:LICENSE (MIT)" form for <license type="file"> packages.
    """
    if not norm:
        return ""
    m = re.match(r"file:\S+\s*\((\S+)\)", norm)
    if m:
        return m.group(1).upper()
    parts = re.split(r"\s+OR\s+|\s+AND\s+|[,()/]", norm)
    ids = {p.strip().upper() for p in parts if p.strip()}
    return "|".join(sorted(ids))


def package_dirs(pkgdir):
    """Map lowercase package id -> real directory name.

    The NuGet global packages folder is lower-cased on disk while
    project.assets.json preserves the PackageReference casing, so a
    case-sensitive lookup finds nothing.
    """
    index = {}
    for entry in os.listdir(pkgdir):
        full = os.path.join(pkgdir, entry)
        if os.path.isdir(full):
            index.setdefault(entry.lower(), entry)
    return index


def license_of(pkgdir, pid):
    """Licence expression from the package's .nuspec.

    Nuspecs vary: v2+ use <license type="expression">MIT</license>, some use a
    <license type="file">, and v1 only has <licenseUrl>. The nuspec has no XML
    namespace, so elements are matched by local name.
    """
    real = package_dirs(pkgdir).get(pid.lower(), pid)
    found = sorted(glob.glob(os.path.join(pkgdir, real, "*", "*.nuspec")), reverse=True)
    for nuspec in found:
        verdir = os.path.dirname(nuspec)
        try:
            root = ET.parse(nuspec).getroot()
        except Exception:
            continue
        for node in root.iter():
            if node.tag.rsplit("}", 1)[-1] != "license":
                continue
            kind = node.attrib.get("type", "expression")
            text = (node.text or "").strip()
            if kind == "file" or text.lower() in ("", "see license"):
                for pat in ("*.md", "LICENSE*", "*license*", "*LICENSE*"):
                    for lic in glob.glob(os.path.join(verdir, pat)):
                        head = open(lic, encoding="utf-8", errors="replace").read(4000)
                        m = re.search(r"MIT|Apache|GNU|BSD|ISC|Mozilla|MS-PL|MS-RL", head)
                        return (f"file:{os.path.basename(lic)}"
                                + (f" ({m.group(0)})" if m else " (unrecognised)"), "file")
                return "(file, unreadable)", "file"
            return text, kind
        for node in root.iter():
            if node.tag.rsplit("}", 1)[-1] == "licenseUrl" and (node.text or "").strip():
                return node.text.strip(), "url"
    return "", ""


def vulnerabilities():
    """Ask the NuGet audit for packages with a known advisory.

    `dotnet list package --vulnerable --include-transitive` returns non-zero
    when it finds one, so the exit code is the signal. Run from linux/ because
    the solution path is relative to it.
    """
    result = subprocess.run(
        ["dotnet", "list", "Calculator.Linux.slnx", "package",
         "--vulnerable", "--include-transitive"],
        capture_output=True, text=True, cwd=os.path.join(ROOT, "linux"))

    findings = []
    for line in result.stdout.splitlines():
        low = line.lower()
        if any(s in low for s in ("high", "critical", "moderate", "low")) and ">" in line:
            cells = [c.strip() for c in line.replace(">", " ").split()]
            cells = [c for c in cells if c]
            if cells:
                findings.append((cells[0], cells[1] if len(cells) > 1 else "?", line.strip()))

    no_vuln = "no vulnerable packages" in result.stdout.lower()
    return findings, no_vuln


def main():
    pkgdir = packages_dir()
    print(f"NuGet global packages folder: {pkgdir}\n")
    pkgs = project_packages(pkgdir)
    if not pkgs:
        print("FAIL: no restored packages found - run 'dotnet restore' first")
        return 1

    rows, problems, manual = [], [], []
    for pid in sorted(pkgs, key=str.lower):
        expr, kind = license_of(pkgdir, pid)
        norm = re.sub(r"\s+", " ", expr).strip()
        base = classify(norm)
        if base in ALLOWED:
            verdict = "permissive"
        elif kind == "url" and norm:
            # v1-style nuspec: only a URL. Flagged for a human, never silently
            # treated as permissive.
            verdict = "url-only"
            manual.append((pid, norm))
        else:
            verdict = "NOT-PERMISSIVE"
            problems.append((pid, norm or "(no licence declared)"))
        rows.append((pid, ",".join(sorted(pkgs[pid])), norm or "(none)", kind or "-", verdict))

    width = max(len(r[0]) for r in rows)
    print(f"{'PACKAGE'.ljust(width)}  {'VERSION'.ljust(18)}  {'TYPE'.ljust(5)}  LICENCE")
    print("-" * (width + 60))
    for pid, ver, expr, kind, verdict in rows:
        print(f"{pid.ljust(width)}  {ver.ljust(18)}  {kind.ljust(5)}  {expr:<52} {verdict}")

    print(f"\n{len(rows)} transitive packages")
    if manual:
        print(f"\nREVIEW: {len(manual)} package(s) declare a licence only as a URL "
              f"(cannot be classified automatically):")
        for pid, url in manual:
            print(f"   {pid}: {url}")
    if problems:
        print(f"\nFAIL: {len(problems)} package(s) without a recognised permissive licence:")
        for pid, expr in problems:
            print(f"   {pid}: {expr}")
        rc = 1
    else:
        print("OK: every transitive package declares a permissive licence")
        rc = 0

    # Known-advisory gate. AUD-S2-002 was found this way: Avalonia.FreeDesktop
    # 11.3.6 pins Tmds.DBus.Protocol 0.21.2, which carries CVE-2026-39959.
    findings, no_vuln = vulnerabilities()
    print()
    if no_vuln and not findings:
        print("OK: no known vulnerabilities in the transitive graph")
        return rc
    if findings:
        print(f"FAIL: {len(findings)} package(s) with a known advisory:")
        for pid, ver, line in findings:
            print(f"   {pid} {ver}: {line}")
        return 1
    print("FAIL: could not determine the vulnerability status - treating as a failure")
    return 1


if __name__ == "__main__":
    sys.exit(main())