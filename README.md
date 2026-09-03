# QtMakeTiny

A Qt 6 desktop application for creating TinyURL links and resolving short URLs.

Before using the desktop app, start the local safety backend:

```sh
python3 backend/server.py
```

The backend checks each URL and redirect hop with URLhaus when
`URLHAUS_AUTH_KEY` is set. The desktop app fails closed if the backend is
unavailable or cannot produce a verdict. Set `MAKETINY_BACKEND_URL` to use a
deployed backend.

## Build and run

```sh
qmake6 QtMakeTiny.pro
make
./QtMakeTiny.app/Contents/MacOS/QtMakeTiny
```

## Tests

```sh
cd tests
qmake6 urltools_test.pro
make
./urltools_test.app/Contents/MacOS/urltools_test
```

The app validates HTTP/HTTPS input, follows redirects for reverse lookup, reports
network failures in the status bar, and emits structured logs under the
`maketiny.network` category. Enable them with:

```sh
QT_LOGGING_RULES='maketiny.network.debug=true' ./QtMakeTiny.app/Contents/MacOS/QtMakeTiny
```
