#!/bin/bash

wget --post-file=pagelocate.json http://127.0.0.1:8080/api/locate -O pagelocate-resp.json
cat pagelocate-resp.json
./create_nextpage_req.py pagelocate.json pagelocate-resp.json > nextpagelocate-from-resp.json
wget --post-file=nextpagelocate-from-resp.json http://127.0.0.1:8080/api/locate -O nextpagelocate-resp.json
cat nextpagelocate-resp.json
