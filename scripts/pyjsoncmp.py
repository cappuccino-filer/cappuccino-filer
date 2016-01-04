#! /usr/bin/env python3

import json
import sys
from pprint import pprint

with open(sys.argv[1]) as jf:
	json1 = json.load(jf)

with open(sys.argv[2]) as jf:
	json2 = json.load(jf)

print(json1 == json2)

pprint(json1)
pprint(json2)
