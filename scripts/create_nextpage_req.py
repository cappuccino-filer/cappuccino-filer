#! /usr/bin/env python3

import json
import sys

with open(sys.argv[1]) as jf:
	current_page = json.load(jf)

with open(sys.argv[2]) as jf:
	current_resp = json.load(jf)

current_page['cache_cookie'] = current_resp['cache_cookie']
current_page['start'] += current_page['number']
print(json.dumps(current_page))
