"""The installed module grant is Apache-2.0, with the iisacc copyright notice."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]


class LicenseTextTests(unittest.TestCase):
    def test_root_license_is_apache_2(self):
        text = (ROOT / "LICENSE").read_text(encoding="utf-8")
        self.assertIn("Apache License", text)
        self.assertIn("Version 2.0, January 2004", text)
        self.assertIn("http://www.apache.org/licenses/", text)
        self.assertNotIn("MIT License", text)
        self.assertNotIn("Permission is hereby granted, free of charge", text)

    def test_notice_names_iisacc_and_the_product_split(self):
        text = (ROOT / "NOTICE").read_text(encoding="utf-8")
        self.assertIn("Copyright (c) 2026 iisacc", text)
        self.assertIn("Apache License", text)
        self.assertIn("AGPL-3.0-or-later", text)
        self.assertIn("https://github.com/iisacc-Justmoong", text)

    def test_readme_spdx_and_korean_statement(self):
        readme = (ROOT / "README.md").read_text(encoding="utf-8")
        self.assertIn("SPDX-License-Identifier: Apache-2.0", readme)
        self.assertNotIn("SPDX-License-Identifier: MIT", readme)
        self.assertIn("[Apache License, Version 2.0](LICENSE)", readme)
        korean = (ROOT / "docs" / "License.md").read_text(encoding="utf-8")
        self.assertIn("SPDX-License-Identifier: Apache-2.0", korean)
        self.assertIn("Apache License 2.0", korean)
        self.assertIn("AGPL-3.0-or-later", korean)

    def test_package_metadata_exports_spdx_and_installs_license(self):
        cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        config = (ROOT / "cmake" / "iiDeviceMultimediaConfig.cmake.in").read_text(encoding="utf-8")
        self.assertIn('set(iiDeviceMultimedia_SPDX_LICENSE "Apache-2.0")', cmake)
        self.assertIn('set(iiDeviceMultimedia_SPDX_LICENSE "Apache-2.0")', config)
        self.assertIn("install(FILES README.md LICENSE NOTICE", cmake)


if __name__ == "__main__":
    unittest.main()
