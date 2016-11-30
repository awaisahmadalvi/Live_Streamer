/*
 * srvrJson.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <Server.h>

int exists(const char *fname) {
	FILE *file;
	char fn[64];
	snprintf(fn, 64, "status/%s.json", fname);
	if ((file = fopen(fn, "rw"))) {
		fclose(file);
		return 1;
	}
	return 0;
}

json_object * newJsonFile(char *file) {
	json_object *tempObj = json_object_new_object();
	json_object * newEntry = json_object_new_string(file);
	json_object_object_add(tempObj, "streamId", newEntry);
	JsonToFile(tempObj, file);
	newEntry = json_object_new_string("off");
	json_object_object_add(tempObj, "status", newEntry);
	JsonToFile(tempObj, file);
	newEntry = json_object_new_string("live");
	json_object_object_add(tempObj, "action", newEntry);
	JsonToFile(tempObj, file);
	char fn[64];
	snprintf(fn, 64, "status/%s.json", file);
	return json_object_from_file(fn);
}

json_object * JsonFromFile(char *file) {
	if (exists(file)) {
		char fn[64];
		snprintf(fn, 64, "status/%s.json", file);
		return json_object_from_file(fn);
	} //"/root/Streamer/$(stream_id)"
	return newJsonFile(file);
}

void JsonToFile(json_object * jobj, char *file) {
	char fn[64];
	snprintf(fn, 64, "status/%s.json", file);
	json_object_to_file(fn, jobj);
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
