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
    @patch("server.web_risk_lookup", return_value={"provider": "google-webrisk", "threats": [{"threatTypes": ["MALWARE"]}]})
    @patch.dict(os.environ, {"WEB_RISK_API_KEY": "test-key"}, clear=True)
    def test_threat_finding_is_malicious(self, *_):
        self.assertEqual(check_url("https://short.example/a").verdict, "malicious")


if __name__ == "__main__": unittest.main()
