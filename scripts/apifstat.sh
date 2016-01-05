#!/bin/bash

wget --post-data="{\"path\":\"$1\"}" http://127.0.0.1:8080/api/fstat -O fstat-resp.json
cat fstat-resp.json
