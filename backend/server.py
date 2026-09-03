#!/usr/bin/env python3
"""Small defensive URL reputation backend for QtMakeTiny."""
import ipaddress
import json
import logging
import os
import socket
import urllib.error
import urllib.parse
import urllib.request
from dataclasses import dataclass
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

LOG = logging.getLogger("maketiny.backend")
MAX_REDIRECTS = 5
TIMEOUT_SECONDS = 5


def public_http_url(value: str) -> str:
    parsed = urllib.parse.urlparse(value.strip())
    if parsed.scheme.lower() not in {"http", "https"} or not parsed.hostname:
        raise ValueError("only HTTP and HTTPS URLs are allowed")
    if parsed.username or parsed.password:
        raise ValueError("URLs containing credentials are not allowed")
    host = parsed.hostname
    try:
        addresses = {item[4][0] for item in socket.getaddrinfo(host, parsed.port, type=socket.SOCK_STREAM)}
    except (OSError, ValueError):
        raise ValueError("URL host cannot be resolved")
    for address in addresses:
        ip = ipaddress.ip_address(address)
        if any((ip.is_private, ip.is_loopback, ip.is_link_local, ip.is_multicast,
                ip.is_reserved, ip.is_unspecified)):
            raise ValueError("private or reserved hosts are not allowed")
    return parsed.geturl()


class NoRedirect(urllib.request.HTTPRedirectHandler):
    def http_error_301(self, req, fp, code, msg, headers): return fp
    def http_error_302(self, req, fp, code, msg, headers): return fp
    def http_error_303(self, req, fp, code, msg, headers): return fp
    def http_error_307(self, req, fp, code, msg, headers): return fp
    def http_error_308(self, req, fp, code, msg, headers): return fp


def resolve_redirects(start: str) -> list[str]:
    current = public_http_url(start)
    chain = [current]
    opener = urllib.request.build_opener(NoRedirect)
    for _ in range(MAX_REDIRECTS):
        request = urllib.request.Request(current, headers={"User-Agent": "QtMakeTiny/1.0"}, method="HEAD")
        try:
            response = opener.open(request, timeout=TIMEOUT_SECONDS)
        except urllib.error.HTTPError as error:
            response = error
        location = response.headers.get("Location")
        if response.status < 300 or response.status >= 400 or not location:
            return chain
        current = public_http_url(urllib.parse.urljoin(current, location))
        chain.append(current)
    raise ValueError("redirect limit exceeded")


def urlhaus_lookup(url: str, auth_key: str) -> dict:
    body = urllib.parse.urlencode({"url": url}).encode()
    request = urllib.request.Request("https://urlhaus-api.abuse.ch/v1/url/", data=body,
                                     headers={"Auth-Key": auth_key,
                                              "Content-Type": "application/x-www-form-urlencoded"})
    with urllib.request.urlopen(request, timeout=TIMEOUT_SECONDS) as response:
        result = json.loads(response.read() or b"{}")
    return {"provider": "urlhaus", "matched": result.get("query_status") == "ok",
            "threat": result.get("threat"), "tags": result.get("tags", [])}


@dataclass
class CheckResult:
    verdict: str
    chain: list[str]
    findings: list[dict]
    errors: list[str]

    def as_dict(self) -> dict:
        return {"verdict": self.verdict, "chain": self.chain,
                "findings": self.findings, "errors": self.errors}


def check_url(url: str) -> CheckResult:
    chain = resolve_redirects(url)
    findings, errors = [], []
    urlhaus_key = os.environ.get("URLHAUS_AUTH_KEY")
    for item in chain:
        if urlhaus_key:
            try:
                result = urlhaus_lookup(item, urlhaus_key)
                if result["matched"]: findings.append({"url": item, **result})
            except (OSError, ValueError, json.JSONDecodeError) as error:
                errors.append(f"urlhaus: {error}")
    configured = bool(urlhaus_key)
    verdict = "malicious" if findings else ("unknown" if errors or not configured else "safe")
    LOG.info("url check verdict=%s hops=%d findings=%d errors=%d", verdict, len(chain), len(findings), len(errors))
    return CheckResult(verdict, chain, findings, errors)


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/healthz":
            self.send_response(200); self.send_header("Content-Type", "application/json"); self.end_headers()
            self.wfile.write(b'{"status":"ok"}')
            return
        self.send_error(404)

    def do_POST(self):
        if self.path != "/api/url-check":
            self.send_error(404); return
        try:
            size = int(self.headers.get("Content-Length", "0"))
            if size > 4096: raise ValueError("request is too large")
            payload = json.loads(self.rfile.read(size))
            result = check_url(payload["url"])
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(result.as_dict()).encode())
        except (ValueError, KeyError, json.JSONDecodeError) as error:
            self.send_error(400, str(error))
        except Exception:
            LOG.exception("URL check failed")
            self.send_error(502, "URL check unavailable")

    def log_message(self, format, *args):
        LOG.info("%s - %s", self.address_string(), format % args)


def main():
    logging.basicConfig(level=os.environ.get("LOG_LEVEL", "INFO"),
                        format="%(asctime)s %(levelname)s %(name)s %(message)s")
    host, port = os.environ.get("BIND_HOST", "127.0.0.1"), int(os.environ.get("PORT", "8787"))
    LOG.info("backend listening on %s:%d", host, port)
    ThreadingHTTPServer((host, port), Handler).serve_forever()


if __name__ == "__main__": main()
