/*
 * clientJson.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <client.h>

json_object * JsonFromFile() {
	return json_object_from_file("/root/Streamer/properties.json");
}

void JsonToFile(json_object * jobj) {
	json_object_to_file("/root/Streamer/properties.json", jobj);
}

void setJsonValue(char * jKey, char *value) {
	json_object * tempJson = JsonFromFile();

	json_object * newEntry = json_object_new_string(value);
	json_object_object_add(tempJson, jKey, newEntry);
	JsonToFile(tempJson);
}

char * getJsonValueFromFile(char * jKey) {
	char *value;
	json_object * tempJson = JsonFromFile();

	json_object * jValue;
	json_object_object_get_ex(tempJson, jKey, &jValue);
	tempJson = NULL;
	value = json_object_get_string(jValue);
	return value;
}

char * getJsonValueFromObj(json_object * tempJson, char * jKey) {
	char *value;
	json_object * jValue;
	json_object_object_get_ex(tempJson, jKey, &jValue);
	tempJson = NULL;
	value = json_object_get_string(jValue);
	return value;
}

