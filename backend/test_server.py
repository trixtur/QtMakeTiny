import os
import unittest
from unittest.mock import patch

from server import CheckResult, check_url, public_http_url


class UrlSafetyTests(unittest.TestCase):
    @patch("server.socket.getaddrinfo", return_value=[(None, None, None, None, ("93.184.216.34", 0))])
    def test_accepts_public_http_urls(self, _):
        self.assertEqual(public_http_url(" https://example.com/path "), "https://example.com/path")

    @patch("server.socket.getaddrinfo", return_value=[(None, None, None, None, ("127.0.0.1", 0))])
    def test_rejects_private_hosts(self, _):
        with self.assertRaises(ValueError): public_http_url("http://localhost/")

    def test_rejects_credentials_and_non_http(self):
        with patch("server.socket.getaddrinfo", return_value=[(None, None, None, None, ("93.184.216.34", 0))]):
            with self.assertRaises(ValueError): public_http_url("https://user:pass@example.com/")
            with self.assertRaises(ValueError): public_http_url("ftp://example.com/file")

    @patch("server.resolve_redirects", return_value=["https://short.example/a", "https://safe.example/final"])
    @patch.dict(os.environ, {}, clear=True)
    def test_missing_provider_is_unknown(self, *_):
        result = check_url("https://short.example/a")
        self.assertEqual(result.verdict, "unknown")
        self.assertEqual(len(result.chain), 2)

    @patch("server.resolve_redirects", return_value=["https://short.example/a", "https://safe.example/final"])
    @patch("server.urlhaus_lookup", return_value={"provider": "urlhaus", "matched": True, "threat": "malware_download", "tags": []})
    @patch.dict(os.environ, {"URLHAUS_AUTH_KEY": "test-key"}, clear=True)
    def test_threat_finding_is_malicious(self, *_):
        self.assertEqual(check_url("https://short.example/a").verdict, "malicious")

    @patch("server.resolve_redirects", return_value=["https://tinyurl.com/test", "https://example.invalid/urlhaus-test"])
    @patch.dict(os.environ, {"URLHAUS_TEST_URL": "https://example.invalid/urlhaus-test"}, clear=True)
    def test_local_fixture_is_malicious(self, *_):
        result = check_url("https://tinyurl.com/test")
        self.assertEqual(result.verdict, "malicious")
        self.assertTrue(result.findings[0]["test_fixture"])


if __name__ == "__main__": unittest.main()
