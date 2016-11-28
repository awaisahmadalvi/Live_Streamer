/*
 * srvrJson.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <Server.h>

int exists(const char *fname) {
	FILE *file;
	char n[128];
	sprintf(n, "/home/team/%s", fname);
	if ((file = fopen(n, "rw"))) {
		fclose(file);
		return 1;
	}

	return 0;
}

json_object * newJsonFile(char *file) {
	json_object *tempObj = json_object_new_object();
	/*json_object * newEntry = json_object_new_string("myStream");
	 json_object_object_add(tempObj, "streamId", newEntry);
	 */json_object * newEntry = json_object_new_string("done");
	json_object_object_add(tempObj, "action", newEntry);
	JsonToFile(tempObj, file);
	return json_object_from_file(file);;
}

json_object * JsonFromFile(char *file) {
	if (exists(file))
		return json_object_from_file(file); //"/root/Streamer/$(stream_id)"
	return newJsonFile(file);
}

void JsonToFile(json_object * jobj, char *file) {
	json_object_to_file(file, jobj);
}

void setJsonValue(char * jKey, char *value, char *file) {
	json_object * tempJson = JsonFromFile(file);

	json_object * newEntry = json_object_new_string(value);
	json_object_object_add(tempJson, jKey, newEntry);
	JsonToFile(tempJson, file);
}

char * getJsonValueFromFile(char * jKey, char *file) {
	json_object * tempJson = JsonFromFile(file);
	json_object * jValue;
	if (!json_object_object_get_ex(tempJson, jKey, &jValue)) {
		json_object * newEntry = json_object_new_string("off");
		json_object_object_add(tempJson, "action", newEntry);
		JsonToFile(tempJson, file);
	}
	return json_object_get_string(jValue);
}

char * getJsonValueFromObj(char * jKey, json_object * tempJson) {
	char *value;
	json_object * jValue;
	json_object_object_get_ex(tempJson, jKey, &jValue);
	tempJson = NULL;
	return json_object_get_string(jValue);
}
