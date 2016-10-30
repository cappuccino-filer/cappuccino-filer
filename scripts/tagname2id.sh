#!/bin/bash

wget --post-file=tagname2id.json http://127.0.0.1:8080/api/tag -O resp.json
cat resp.json
