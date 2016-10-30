#!/bin/bash

wget --post-file=sample.json http://127.0.0.1:8080/api/demo04 -O resp.json
./pyjsoncmp.py sample.json resp.json
