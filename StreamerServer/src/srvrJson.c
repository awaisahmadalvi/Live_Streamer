/*
 * srvrJson.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <Server.h>

char* jsonFileWithAddr(char *file) {
	//char *fn;
	char *fn = (char *) malloc(sizeof(char) * 64);
	snprintf(fn, 64, "status/%s.json", file);
	return fn;
}

json_object * newJsonFile(char *file) {
	json_object *tempObj = json_object_new_object();
	json_object * newEntry = json_object_new_string(file);
	json_object_object_add(tempObj, "streamId", newEntry);
	newEntry = json_object_new_string("off");
	json_object_object_add(tempObj, "status", newEntry);
	newEntry = json_object_new_string("live");
	json_object_object_add(tempObj, "action", newEntry);
	JsonToFile(tempObj, file);
	return json_object_from_file(jsonFileWithAddr(file));
}

json_object * JsonFromFile(char *file) {
	json_object* temp = json_object_from_file(jsonFileWithAddr(file));
	if (temp == NULL) {
		printf(" JSON NOT FOUND \n\n");
		return newJsonFile(file);
	}
	return temp;
}

void JsonToFile(json_object * jobj, char *file) {
	json_object_to_file(jsonFileWithAddr(file), jobj);
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
