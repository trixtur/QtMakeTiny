package main

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"html"
	"io"
	"log"
	"net"
	"net/http"
	"net/url"
	"os"
	"regexp"
	"strings"
	"time"
)

const maxRedirects = 5
const maxDigestBytes = 512 * 1024

type Finding struct {
	URL         string   `json:"url"`
	Provider    string   `json:"provider"`
	Matched     bool     `json:"matched"`
	Threat      string   `json:"threat,omitempty"`
	Tags        []string `json:"tags,omitempty"`
	TestFixture bool     `json:"test_fixture,omitempty"`
}

type CheckResult struct {
	Verdict  string      `json:"verdict"`
	Chain    []string    `json:"chain"`
	Findings []Finding   `json:"findings"`
	Errors   []string    `json:"errors"`
	Digest   *SiteDigest `json:"digest,omitempty"`
}

type SiteDigest struct {
	URL         string `json:"url"`
	Title       string `json:"title,omitempty"`
	Description string `json:"description,omitempty"`
	SiteName    string `json:"site_name,omitempty"`
	ContentType string `json:"content_type,omitempty"`
}

type Checker struct {
	Client          *http.Client
	URLhausKey      string
	URLhausEndpoint string
	TestURL         string
}

func publicHTTPURL(raw string) (string, error) {
	u, err := url.Parse(strings.TrimSpace(raw))
	if err != nil || u.Hostname() == "" || (u.Scheme != "http" && u.Scheme != "https") {
		return "", errors.New("only HTTP and HTTPS URLs are allowed")
	}
	if u.User != nil {
		return "", errors.New("URLs containing credentials are not allowed")
	}
	addresses, err := net.LookupIP(u.Hostname())
	if err != nil || len(addresses) == 0 {
		return "", errors.New("URL host cannot be resolved")
	}
	for _, address := range addresses {
		if !address.IsGlobalUnicast() || address.IsPrivate() || address.IsLoopback() ||
			address.IsLinkLocalUnicast() || address.IsUnspecified() || address.IsMulticast() {
			return "", errors.New("private or reserved hosts are not allowed")
		}
	}
	return u.String(), nil
}

func (c *Checker) ResolveRedirects(ctx context.Context, start string) ([]string, error) {
	current, err := publicHTTPURL(start)
	if err != nil {
		return nil, err
	}
	chain := []string{current}
	client := *c.Client
	client.CheckRedirect = func(_ *http.Request, _ []*http.Request) error { return http.ErrUseLastResponse }
	for i := 0; i < maxRedirects; i++ {
		req, err := http.NewRequestWithContext(ctx, http.MethodHead, current, nil)
		if err != nil {
			return nil, err
		}
		req.Header.Set("User-Agent", "QtMakeTiny/1.0")
		resp, err := client.Do(req)
		if err != nil {
			return nil, err
		}
		location := resp.Header.Get("Location")
		resp.Body.Close()
		if resp.StatusCode < 300 || resp.StatusCode >= 400 || location == "" {
			return chain, nil
		}
		next, err := url.Parse(location)
		if err != nil {
			return nil, err
		}
		base, _ := url.Parse(current)
		nextURL := base.ResolveReference(next).String()
		if c.TestURL == nextURL {
			chain = append(chain, nextURL)
			return chain, nil
		}
		current, err = publicHTTPURL(nextURL)
		if err != nil {
			return nil, err
		}
		chain = append(chain, current)
	}
	return nil, errors.New("redirect limit exceeded")
}

func (c *Checker) URLhausLookup(ctx context.Context, rawURL string) (Finding, error) {
	form := url.Values{"url": {rawURL}}
	req, err := http.NewRequestWithContext(ctx, http.MethodPost, c.URLhausEndpoint, strings.NewReader(form.Encode()))
	if err != nil {
		return Finding{}, err
	}
	req.Header.Set("Auth-Key", c.URLhausKey)
	req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	resp, err := c.Client.Do(req)
	if err != nil {
		return Finding{}, err
	}
	defer resp.Body.Close()
	if resp.StatusCode < 200 || resp.StatusCode >= 300 {
		return Finding{}, fmt.Errorf("URLhaus returned HTTP %d", resp.StatusCode)
	}
	var result struct {
		QueryStatus string   `json:"query_status"`
		Threat      string   `json:"threat"`
		Tags        []string `json:"tags"`
	}
	if err := json.NewDecoder(io.LimitReader(resp.Body, 64*1024)).Decode(&result); err != nil {
		return Finding{}, err
	}
	return Finding{Provider: "urlhaus", Matched: result.QueryStatus == "ok", Threat: result.Threat, Tags: result.Tags}, nil
}

func metaValue(document, name string) string {
	tagPattern := regexp.MustCompile(`(?is)<meta\b[^>]*>`)
	attributePattern := regexp.MustCompile(`(?i)([a-z][a-z0-9:_-]*)\s*=\s*["']([^"']*)["']`)
	for _, tag := range tagPattern.FindAllString(document, -1) {
		attrs := map[string]string{}
		for _, match := range attributePattern.FindAllStringSubmatch(tag, -1) {
			attrs[strings.ToLower(match[1])] = html.UnescapeString(strings.TrimSpace(match[2]))
		}
		if strings.EqualFold(attrs["name"], name) || strings.EqualFold(attrs["property"], name) {
			return attrs["content"]
		}
	}
	return ""
}

func extractDigest(rawURL, contentType, document string) SiteDigest {
	titlePattern := regexp.MustCompile(`(?is)<title\b[^>]*>(.*?)</title>`)
	digest := SiteDigest{URL: rawURL, ContentType: contentType}
	if match := titlePattern.FindStringSubmatch(document); len(match) == 2 {
		digest.Title = strings.TrimSpace(html.UnescapeString(regexp.MustCompile(`<[^>]+>`).ReplaceAllString(match[1], "")))
	}
	digest.Description = metaValue(document, "description")
	digest.SiteName = metaValue(document, "og:site_name")
	if digest.Title == "" {
		digest.Title = metaValue(document, "og:title")
	}
	return digest
}

func (c *Checker) FetchDigest(ctx context.Context, rawURL string) (SiteDigest, error) {
	client := *c.Client
	client.CheckRedirect = func(_ *http.Request, _ []*http.Request) error { return http.ErrUseLastResponse }
	req, err := http.NewRequestWithContext(ctx, http.MethodGet, rawURL, nil)
	if err != nil {
		return SiteDigest{}, err
	}
	req.Header.Set("User-Agent", "QtMakeTiny/1.0")
	req.Header.Set("Range", fmt.Sprintf("bytes=0-%d", maxDigestBytes-1))
	resp, err := client.Do(req)
	if err != nil {
		return SiteDigest{}, err
	}
	defer resp.Body.Close()
	contentType := resp.Header.Get("Content-Type")
	if !strings.Contains(strings.ToLower(contentType), "text/html") {
		return SiteDigest{URL: rawURL, ContentType: contentType}, nil
	}
	body, err := io.ReadAll(io.LimitReader(resp.Body, maxDigestBytes))
	if err != nil {
		return SiteDigest{}, err
	}
	return extractDigest(rawURL, contentType, string(body)), nil
}

func (c *Checker) CheckURL(ctx context.Context, rawURL string) (CheckResult, error) {
	chain, err := c.ResolveRedirects(ctx, rawURL)
	if err != nil {
		return CheckResult{}, err
	}
	result := CheckResult{Verdict: "unknown", Chain: chain, Findings: []Finding{}, Errors: []string{}}
	for _, item := range chain {
		if c.TestURL != "" && item == c.TestURL {
			result.Findings = append(result.Findings, Finding{URL: item, Provider: "urlhaus", Matched: true, Threat: "malware_download", TestFixture: true})
			continue
		}
		if c.URLhausKey == "" {
			continue
		}
		finding, lookupErr := c.URLhausLookup(ctx, item)
		if lookupErr != nil {
			result.Errors = append(result.Errors, "urlhaus: "+lookupErr.Error())
			continue
		}
		if finding.Matched {
			finding.URL = item
			result.Findings = append(result.Findings, finding)
		}
	}
	if len(result.Findings) > 0 {
		result.Verdict = "malicious"
	} else if c.URLhausKey != "" && len(result.Errors) == 0 {
		result.Verdict = "safe"
		if digest, digestErr := c.FetchDigest(ctx, chain[len(chain)-1]); digestErr != nil {
			result.Errors = append(result.Errors, "digest: "+digestErr.Error())
		} else {
			result.Digest = &digest
		}
	}
	log.Printf("event=url_check verdict=%s hops=%d findings=%d errors=%d", result.Verdict, len(chain), len(result.Findings), len(result.Errors))
	return result, nil
}

type handler struct{ checker *Checker }

func (h handler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	if r.Method == http.MethodGet && r.URL.Path == "/healthz" {
		writeJSON(w, http.StatusOK, map[string]string{"status": "ok"})
		return
	}
	if r.Method != http.MethodPost || r.URL.Path != "/api/url-check" {
		http.NotFound(w, r)
		return
	}
	r.Body = http.MaxBytesReader(w, r.Body, 4096)
	var payload struct {
		URL string `json:"url"`
	}
	if err := json.NewDecoder(r.Body).Decode(&payload); err != nil || payload.URL == "" {
		http.Error(w, "invalid request", http.StatusBadRequest)
		return
	}
	result, err := h.checker.CheckURL(r.Context(), payload.URL)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}
	writeJSON(w, http.StatusOK, result)
}

func writeJSON(w http.ResponseWriter, status int, value any) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	_ = json.NewEncoder(w).Encode(value)
}

func main() {
	log.SetFlags(log.LstdFlags | log.Lmicroseconds)
	port := os.Getenv("PORT")
	if port == "" {
		port = "8787"
	}
	checker := &Checker{Client: &http.Client{Timeout: 5 * time.Second}, URLhausKey: os.Getenv("URLHAUS_AUTH_KEY"), URLhausEndpoint: "https://urlhaus-api.abuse.ch/v1/url/", TestURL: os.Getenv("URLHAUS_TEST_URL")}
	server := &http.Server{Addr: "127.0.0.1:" + port, Handler: handler{checker: checker}, ReadHeaderTimeout: 5 * time.Second}
	log.Printf("event=backend_listening address=%s", server.Addr)
	if err := server.ListenAndServe(); err != nil && !errors.Is(err, http.ErrServerClosed) {
		log.Printf("event=backend_stopped error=%v", err)
		os.Exit(1)
	}
}
