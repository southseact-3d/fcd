#!/usr/bin/python
# SPDX-License-Identifier: LGPL-2.1-or-later

print("Fetching download statistics from github...")

import json
import urllib.request

response = urllib.request.urlopen(
    "https://api.github.com/repos/FreeCAD/FreeCAD/releases"
)
myobj = json.loads(response.read())
for p in myobj:
    if "assets" in p:
        for asset in p["assets"]:
            print((asset["name"] + ": " + str(asset["download_count"]) + " downloads"))
