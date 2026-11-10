import importlib.util
from pathlib import Path
import unittest

spec = importlib.util.spec_from_file_location("example_policy", Path(__file__).with_name("example_policy.py"))
policy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(policy)

class ExamplePolicyTests(unittest.TestCase):
    def test_target_detection_uses_complete_target_name(self):
        self.assertTrue(policy.has_target("iiDeviceMultimediaHardwareExample: phony\n"))
        self.assertFalse(policy.has_target("iiDeviceMultimediaHardwareExampleTests: phony\n"))
        self.assertFalse(policy.has_target("iiDeviceMultimediaHardwareTests: phony\n"))
