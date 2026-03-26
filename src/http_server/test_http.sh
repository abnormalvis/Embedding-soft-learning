#!/usr/bin/env bash
curl --noproxy '*' -i --max-time 3 http://127.0.0.1:8080/ | head -n 20
curl --noproxy '*' -i --max-time 3 http://127.0.0.1:8080/hello | head -n 20
