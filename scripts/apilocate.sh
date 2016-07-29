#!/bin/bash

wget --post-file=locate.json http://127.0.0.1:8080/api/locate -O locate-resp.json
cat locate-resp.json
