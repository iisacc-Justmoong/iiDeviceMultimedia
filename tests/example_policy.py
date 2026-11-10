"""Check that the generated build exposes the example only when enabled."""
import argparse
import re
import subprocess

TARGET = "iiDeviceMultimediaHardwareExample"


def has_target(help_output):
    return bool(re.search(r"(?<![A-Za-z0-9_])" + re.escape(TARGET) + r"(?![A-Za-z0-9_])", help_output))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cmake", required=True)
    parser.add_argument("--build", required=True)
    parser.add_argument("--enabled", choices=("ON", "OFF"), required=True)
    args = parser.parse_args()
    result = subprocess.run([args.cmake, "--build", args.build, "--target", "help"],
                            check=True, capture_output=True, text=True)
    actual = has_target(result.stdout)
    expected = args.enabled == "ON"
    if actual != expected:
        raise SystemExit(f"Example target availability {actual} does not match option {args.enabled}")


if __name__ == "__main__":
    main()
