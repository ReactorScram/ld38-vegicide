import json;

with open ("scene_ecs.json", "r") as f:
	s = f.read ()
	data = json.loads (s)
	print (data)
