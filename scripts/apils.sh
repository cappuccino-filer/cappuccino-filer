#!/bin/bash

wget --post-data="{\"path\":\"$1\"}" http://127.0.0.1:8080/api/ls -O ls-resp.json
cat ls-resp.json
