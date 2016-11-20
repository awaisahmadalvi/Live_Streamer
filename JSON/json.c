#include <json/json.h>
#include <stdio.h>
#include <string.h>

void JsonParse(json_object * jobj) {

	enum json_type type;
	json_object_object_foreach(jobj, key, val)
	{
		printf("%s : ", key);
		type = json_object_get_type(val);
		switch (type) {
		case json_type_boolean:
			printf("type: json_type_boolean, ");
			printf("value: %s\n",
					json_object_get_boolean(val) ? "true" : "false");
			break;
		case json_type_int:
			printf("type: json_type_int, ");
			printf("value: %d\n", json_object_get_int(val));
			break;
		case json_type_double:
			printf("type: json_type_double, ");
			printf("value: %f\n", json_object_get_double(val));
			break;
		case json_type_string:
			printf("type: json_type_string, ");
			printf("value: %s\n", json_object_get_string(val));
			break;
		case json_type_array:
			printf("type: json_type_array, ");
			printf("value: %s\n", json_object_get_string(val));
			break;
		case json_type_object:
			printf("type: json_type_object, ");
			printf("value: %s\n", json_object_get_string(val));
			break;
		}
	}
}
json_object * JsonFromFile() {

	return json_object_from_file("test.json");
}
void JsonToFile(json_object * jobj) {

	json_object_to_file("test.json", jobj);
}
void input2JsonFile() {

	json_object * jobj = json_object_new_object();
	while (1) {
		char key[20];
		char flag[5] = "-1";
		printf("Enter Key: ");
		scanf("%s", &key);
		if (strcmp(key, flag) == 0)
			break;
		char value[255];
		printf("Enter value for %s: ", key);
		scanf("%s", &value);
		json_object * newEntry = json_object_new_string(value);
		json_object_object_add(jobj, key, newEntry);
	}
	JsonToFile(jobj);
}
void NewAddDel() {

	json_object * jobj = json_object_new_object();
	json_object * newAdmin = json_object_new_int(32);
	json_object_object_add(jobj, "admin", newAdmin);

	json_object_object_del(jobj, "admin");

	json_object * jAdmin;
	json_object_object_get_ex(jobj, "admin", &jAdmin);
	printf("%s", json_object_get_string(jAdmin));

}
json_object * jArrayNew() {
	/*Creating a json object*/
	json_object * jobj = json_object_new_object();

	/*Creating a json array*/
	json_object *jarray = json_object_new_array();

	/*Creating json strings*/
	json_object *jstring1 = json_object_new_string("c");
	json_object *jstring2 = json_object_new_string("c++");
	json_object *jstring3 = json_object_new_string("php");

	/*Adding the above created json strings to the array*/
	json_object_array_add(jarray, jstring1);
	json_object_array_add(jarray, jstring2);
	json_object_array_add(jarray, jstring3);

	/*Form the json object*/
	json_object_object_add(jobj, "Categories", jarray);

	/*Now printing the json object*/
	printf("The json object created: %s\n", json_object_to_json_string(jobj));
	return jobj;
}
void jMultiArray() {

	json_object * jobj =
			json_tokener_parse(
					"/* more difficult test case */ { \"glossary\": { \"title\": \"example glossary\", \"pageCount\": 100, \"GlossDiv\": { \"title\": \"S\", \"GlossList\": [ { \"ID\": \"SGML\", \"SortAs\": \"SGML\", \"GlossTerm\": \"Standard Generalized Markup Language\", \"Acronym\": \"SGML\", \"Abbrev\": \"ISO 8879:1986\", \"GlossDef\": \"A meta-markup language, used to create markup languages such as DocBook.\", \"GlossSeeAlso\": [\"GML\", \"XML\", \"markup\"] } ] } } }");
	printf("jobj.to_string()=%s\n", json_object_to_json_string(jobj));

	JsonToFile(jobj);

	jobj = json_object_object_get(jobj, "glossary");
	printf("jobj.to_string()=%s\n", json_object_to_json_string(jobj));

	jobj = json_object_object_get(jobj, "pageCount");

	int pageCount = json_object_get_int(jobj);

	printf("Page count = %d", pageCount);

	json_object_put(jobj);
}

int main() {

	//printf("asdasd");
	//JsonParse(json_object_object_get(JsonFromFile(), "myStream"));
	//JsonToFile(jArrayNew());

}

/*
 //input2JsonFile();

 char * string = "{ \"streamId\" : \"myStream\",\"status\" : \"off\"}";
 printf("JSON string: %s\n", string);
 json_object * jobj = json_tokener_parse(string);

 //JsonParse(jobj);
 }
 */
