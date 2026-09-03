package main

import (
	"context"
	"net/http"
	"net/http/httptest"
	"strings"
	"testing"
)

func TestPublicHTTPURLRejectsUnsafeInputs(t *testing.T) {
	for _, value := range []string{"ftp://example.com/file", "https://user:pass@example.com/", "http://127.0.0.1/"} {
		if _, err := publicHTTPURL(value); err == nil {
			t.Errorf("expected %q to be rejected", value)
		}
	}
}

func TestURLhausLookupParsesFinding(t *testing.T) {
	server := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		if r.Header.Get("Auth-Key") != "test-key" {
			t.Error("missing auth key")
		}
		if err := r.ParseForm(); err != nil || r.Form.Get("url") != "https://example.invalid/test" {
			t.Error("unexpected request form")
		}
		w.Header().Set("Content-Type", "application/json")
		_, _ = w.Write([]byte(`{"query_status":"ok","threat":"malware_download","tags":["test"]}`))
	}))
	defer server.Close()
	checker := &Checker{Client: server.Client(), URLhausKey: "test-key", URLhausEndpoint: server.URL}
	finding, err := checker.URLhausLookup(context.Background(), "https://example.invalid/test")
	if err != nil || !finding.Matched || finding.Threat != "malware_download" {
		t.Fatalf("unexpected finding: %#v, %v", finding, err)
	}
}

func TestHandlerHealthAndBadRequest(t *testing.T) {
	h := handler{checker: &Checker{Client: http.DefaultClient, URLhausEndpoint: ""}}
	request := httptest.NewRequest(http.MethodGet, "/healthz", nil)
	response := httptest.NewRecorder()
	h.ServeHTTP(response, request)
	if response.Code != http.StatusOK || !strings.Contains(response.Body.String(), "ok") {
		t.Fatalf("unexpected health response: %d %s", response.Code, response.Body.String())
	}
}
