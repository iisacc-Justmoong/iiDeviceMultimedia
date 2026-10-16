"""Device/media production code and package must not depend on iiSDKCore."""
import argparse
import json
from pathlib import Path
import re


def verify(source, build=None):
    files = [source / "CMakeLists.txt"]
    files += list((source / "src").rglob("*"))
    files += list((source / "cmake").rglob("*"))
    for path in files:
        if path.is_file() and path.suffix in (".h", ".cpp", ".mm", ".txt", ".cmake", ".in"):
            if re.search(r"iiSDKCore|IISDKCORE|ii::runtime|runtime::", path.read_text()):
                raise RuntimeError("Device module references another SDK: " + str(path))
    if build:
        manifests = list(build.glob("device-boundary-*.txt"))
        if not manifests:
            raise RuntimeError("Device dependency manifest missing")
        for path in manifests:
            # Toolchain/sysroot assets can live under another checkout's build/;
            # their directory name is not a dependency on that SDK's API.
            links = "\n".join(path.read_text().splitlines()[:2])
            if re.search(r"iiSDKCore|IISDKCORE", links, re.I):
                raise RuntimeError("Device target links Core: " + links)
        for path in (build / "include/iiDeviceMultimedia").rglob("*.h"):
            if "iiSDKCore" in path.read_text():
                raise RuntimeError("Generated device header references Core")
        for path in (build / "architecture").glob("config-*.json"):
            for unit in json.loads(path.read_text())["translation_units"]:
                if "iiSDKCore" in unit:
                    raise RuntimeError("Device build compiles Core sources")
    print("Independent device source/package boundary passed")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--build", type=Path)
    args = parser.parse_args()
    verify(args.source.resolve(), args.build.resolve() if args.build else None)
