#!/bin/bash

wget --post-file=tagcreate.json http://127.0.0.1:8080/api/tag -O locate-resp.json
cat locate-resp.json
