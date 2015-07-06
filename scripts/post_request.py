#!/usr/bin/env python3.4

import json
import requests

payload = { \
    "class": "json",
    "key1" : "value1",
    "key2" : "value2",
}

url = "http://localhost:8080/"

response = requests.get(url)
print("GET status:", response.status_code);

response = requests.post(url, data=json.dumps(payload))
print("POST status:", response.status_code);
print("POST content:", response.text);
