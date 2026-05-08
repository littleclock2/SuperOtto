import subprocess, json, os, tempfile, base64

REPO = "littleclock2/SuperOtto"
SOURCE_DIR = r"D:\Desktop\Study\程序设计实践\SuperOtto"

def gh_api(endpoint, method="GET", fields=None):
    cmd = ["gh", "api", endpoint, "-X", method, "--hostname", "github.com"]
    if fields:
        for k, v in fields.items():
            cmd.extend(["-f", f"{k}={v}"])
    result = subprocess.run(cmd, capture_output=True, cwd=SOURCE_DIR, timeout=30)
    stdout = result.stdout.decode("utf-8", errors="replace")
    stderr = result.stderr.decode("utf-8", errors="replace")
    if result.returncode != 0:
        return None, stderr.strip()
    return json.loads(stdout) if stdout.strip() else {}, ""

def gh_api_file(endpoint, method, json_data):
    tmpfile = os.path.join(tempfile.gettempdir(), "gh_payload.json")
    with open(tmpfile, "w", encoding="utf-8") as f:
        f.write(json.dumps(json_data))
    cmd = ["gh", "api", endpoint, "-X", method, "--hostname", "github.com", "--input", tmpfile]
    result = subprocess.run(cmd, capture_output=True, cwd=SOURCE_DIR, timeout=30)
    os.unlink(tmpfile)
    stdout = result.stdout.decode("utf-8", errors="replace")
    stderr = result.stderr.decode("utf-8", errors="replace")
    if result.returncode != 0:
        return None, stderr.strip()
    return json.loads(stdout) if stdout.strip() else {}, ""

def main():
    skip_dirs = {".git", "build", "cmake-build-Debug", "cmake-build-Release", "tests"}

    # Get current SHA
    out, _ = gh_api(f"repos/{REPO}/git/refs/heads/master")
    base_sha = out["object"]["sha"]
    print(f"Current: {base_sha}")

    # Create blobs
    entries = []
    for item in sorted(os.listdir(SOURCE_DIR)):
        full = os.path.join(SOURCE_DIR, item)
        if item in skip_dirs:
            continue
        if os.path.isdir(full):
            for root, dirs, files in os.walk(full):
                dirs[:] = [d for d in dirs if d not in skip_dirs]
                for f in sorted(files):
                    fp = os.path.join(root, f)
                    rel = os.path.relpath(fp, SOURCE_DIR).replace(os.sep, "/")
                    with open(fp, "rb") as fh:
                        b64 = base64.b64encode(fh.read()).decode("ascii")
                    out, err = gh_api(f"repos/{REPO}/git/blobs", "POST", {"content": b64, "encoding": "base64"})
                    if out:
                        entries.append({"path": rel, "mode": "100644", "type": "blob", "sha": out["sha"]})
                        print(f"  {rel}")
        else:
            with open(full, "rb") as fh:
                b64 = base64.b64encode(fh.read()).decode("ascii")
            out, err = gh_api(f"repos/{REPO}/git/blobs", "POST", {"content": b64, "encoding": "base64"})
            if out:
                entries.append({"path": item, "mode": "100644", "type": "blob", "sha": out["sha"]})
                print(f"  {item}")

    print(f"\n{len(entries)} files")

    # Create tree WITHOUT base_tree
    out, err = gh_api_file(f"repos/{REPO}/git/trees", "POST", {"tree": entries})
    if not out:
        print(f"Tree failed: {err}")
        return
    tree_sha = out["sha"]
    print(f"Tree: {tree_sha}")

    # Create commit
    out, err = gh_api_file(f"repos/{REPO}/git/commits", "POST", {
        "message": "v3.2.0: Qt5重构版 - 深度汉化 + 操作优化 + 程序图标 + 默认分辨率1960x1200",
        "tree": tree_sha,
        "parents": [base_sha]
    })
    if not out:
        print(f"Commit failed: {err}")
        return
    commit_sha = out["sha"]
    print(f"Commit: {commit_sha}")

    # Force update ref
    out, err = gh_api_file(f"repos/{REPO}/git/refs/heads/master", "PATCH", {"sha": commit_sha, "force": True})
    if not out:
        print(f"Ref failed: {err}")
        return
    print(f"Done! https://github.com/{REPO}")

if __name__ == "__main__":
    main()
