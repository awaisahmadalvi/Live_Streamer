/*
 * srvrJson.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <server.h>

int exists(const char *fname) {
	FILE *file;
	if ((file = fopen(fname, "rw"))) {
		fclose(file);
		return 1;
	}
	return 0;
}

json_object * JsonFromFile(char *file) {

	if (exists(file))
		return json_object_from_file(file); //"/root/Streamer/status.json"
	return json_object_new_object();
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
	json_object_object_get_ex(tempJson, jKey, &jValue);
	tempJson = NULL;
	return json_object_get_string(jValue);
}

char * getJsonValueFromObj(json_object * tempJson, char * jKey) {
	char *value;
	json_object * jValue;
	json_object_object_get_ex(tempJson, jKey, &jValue);
	tempJson = NULL;
	return json_object_get_string(jValue);
}

