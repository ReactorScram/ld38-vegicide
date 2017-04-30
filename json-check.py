import json;

with open ("maps/demo.json", "r") as f:
	s = f.read ()
	data = json.loads (s)
	print (data)
