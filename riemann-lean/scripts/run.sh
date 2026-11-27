#!/bin/bash
# Run a command in a clean environment: no tokens or keys, own HOME and ELAN_HOME; only the proxy and CA settings the sandbox needs.
exec env -i \
  HOME=/home/user/lsernau/home ELAN_HOME=/home/user/lsernau/.elan \
  PATH=/home/user/lsernau/.elan/bin:/usr/local/bin:/usr/bin:/bin TERM=dumb LANG=C.UTF-8 \
  HTTPS_PROXY="$HTTPS_PROXY" https_proxy="$https_proxy" NO_PROXY="localhost,127.0.0.1" no_proxy="localhost,127.0.0.1" \
  SSL_CERT_FILE=/root/.ccr/ca-bundle.crt CURL_CA_BUNDLE=/root/.ccr/ca-bundle.crt GIT_SSL_CAINFO=/root/.ccr/ca-bundle.crt \
  GIT_TERMINAL_PROMPT=0 GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=url.https://github.com/.insteadOf GIT_CONFIG_VALUE_0=git@github.com: \
  "$@"
