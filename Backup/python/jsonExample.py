import sys, json;

with open('status.json', "r") as data_file:
        src = json.load(data_file)
        with open('properties.json', "r") as data_file:    
                snk = json.load(data_file)
                snk["streamId"] = src["streamId"]
                with open('properties.json', "w") as data_file:
                        data_file.write(json.dumps(snk))
                        print "Stream ID set to " ,snk["streamId"]
