/*
 * srvrJson.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <server.h>

json_object * JsonFromFile() {

	if (exists("status.json"))
		return json_object_from_file("status.json");
	return json_object_new_object();
}

int exists(const char *fname) {
	FILE *file;
	if (file = fopen(fname, "rw")) {
		fclose(file);
		return 1;
	}
	return 0;
}

void JsonToFile(json_object * jobj) {
	json_object_to_file("status.json", jobj);
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

