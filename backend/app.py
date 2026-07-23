from __future__ import annotations

import os
import subprocess
import tempfile
from pathlib import Path

from flask import Flask, jsonify, request


BASE_DIR = Path(__file__).resolve().parent
SOURCE_FILE = BASE_DIR / "main2.cpp"
EXECUTABLE_NAME = "a.exe" if os.name == "nt" else "hinglish_compiler"
EXECUTABLE_FILE = BASE_DIR / EXECUTABLE_NAME
BUILD_TIMEOUT_SECONDS = 120
RUN_TIMEOUT_SECONDS = 30

app = Flask(__name__)


@app.after_request
def apply_cors(response):
    response.headers["Access-Control-Allow-Origin"] = "*"
    response.headers["Access-Control-Allow-Headers"] = "Content-Type"
    response.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS, GET"
    return response


def compile_compiler() -> tuple[bool, str]:
    if EXECUTABLE_FILE.exists() and EXECUTABLE_FILE.stat().st_mtime >= SOURCE_FILE.stat().st_mtime:
        return True, ""

    compile_command = [
        "g++",
        "-std=c++17",
        str(SOURCE_FILE),
        "-o",
        str(EXECUTABLE_FILE),
    ]

    result = subprocess.run(
        compile_command,
        cwd=str(BASE_DIR),
        capture_output=True,
        text=True,
        timeout=BUILD_TIMEOUT_SECONDS,
    )

    if result.returncode != 0:
        error_text = result.stderr.strip() or result.stdout.strip() or "Failed to compile compiler backend."
        return False, error_text

    return True, ""


def run_compiler(code: str) -> dict[str, str | int]:
    ok, error_text = compile_compiler()
    if not ok:
        return {"error": error_text}

    with tempfile.TemporaryDirectory(prefix="compiler-ide-", dir=str(BASE_DIR)) as temp_dir:
        temp_file = Path(temp_dir) / "input.li"
        temp_file.write_text(code, encoding="utf-8")

        result = subprocess.run(
            [str(EXECUTABLE_FILE), str(temp_file)],
            cwd=str(BASE_DIR),
            capture_output=True,
            text=True,
            timeout=RUN_TIMEOUT_SECONDS,
        )

        stdout = (result.stdout or "").strip()
        stderr = (result.stderr or "").strip()

        if result.returncode != 0:
            return {"error": stderr or stdout or "Compiler execution failed.", "stdout": stdout, "stderr": stderr}

        if "Compilation errors:" in stdout or stdout.startswith("Error:") or "Error:" in stdout:
            return {"error": stdout, "stdout": stdout, "stderr": stderr}

        return {"stdout": stdout, "stderr": stderr, "output": stdout}


@app.get("/health")
def health():
    return jsonify({"ok": True})


@app.post("/run")
def run_code():
    payload = request.get_json(silent=True) or {}
    code = payload.get("code", "")

    if not isinstance(code, str) or not code.strip():
        return jsonify({"error": "Missing code in request body."}), 400

    language = payload.get("language", "hinglish")
    if language != "hinglish":
        return jsonify({"error": "This backend only runs Hinglish (.li) code."}), 400

    try:
        return jsonify(run_compiler(code))
    except subprocess.TimeoutExpired:
        return jsonify({"error": "Execution timed out.", "stderr": "Process exceeded the time limit."}), 408
    except FileNotFoundError:
        return jsonify({"error": "g++ or the compiler executable was not found."}), 500
    except Exception as exc:  # pragma: no cover - last-resort safety
        return jsonify({"error": str(exc)}), 500


@app.route("/run", methods=["OPTIONS"])
def run_options():
    return ("", 204)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)