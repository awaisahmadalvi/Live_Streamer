#include <libwebsockets.h>
#include <Server.h>

//static void INT_HANDLER(int signo) {
//	destroy_flag = 1;
//}

/* *
 * websocket_write_back: write the string data to the destination wsi.
 */
int websocket_write_back(struct lws *wsi_in, char *str, int str_size_in) {
	if (str == NULL || wsi_in == NULL)
		return -1;

	int n;
	int len;
	char *out = NULL;

	if (str_size_in < 1)
		len = strlen(str);
	else
		len = str_size_in;

	out = (char *) malloc(
			sizeof(char)
					* (LWS_SEND_BUFFER_PRE_PADDING + MAXDATASIZE
							+ LWS_SEND_BUFFER_POST_PADDING));

	//* reply to client */

	/* {"streamId":"myStream","action":"status"} */

	memset(&tempBufP[0], 0, MAXDATASIZE);
	json_object * jobj = json_tokener_parse(str);
	char *strmID = getJsonValueFromObj("streamId", jobj);
	if (strcmp(getJsonValueFromObj("action", jobj), "status") == 0) {
		sprintf(tempBufP, "{\"streamId\":\"%s\",\"status\":\"%s\"}", strmID,
				getJsonValueFromFile("status", strmID));
		//tempBufP[MAXDATASIZE-1] = '\0';
	} else {
		//if (strcmp(getJsonValueFromObj("action", jobj), "live") == 0) {
		setJsonValue("action", getJsonValueFromObj("action", jobj), strmID);
		sprintf(tempBufP, "ACK");
		//tempBufP[MAXDATASIZE-1] = '\0';
	}

	//* setup the buffer*/
	memcpy(out + LWS_SEND_BUFFER_PRE_PADDING, tempBufP, strlen(tempBufP));
	//* write out*/
	n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, strlen(tempBufP),
			LWS_WRITE_TEXT);

	printf(KBLU"[websocket_write_back] %s\n"RESET, tempBufP);
	//* free the buffer*/
	free(out);

	return n;
}

static int ws_service_callback(struct lws *wsi,
		enum lws_callback_reasons reason, void *user, void *in, size_t len) {

	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		printf(KYEL"[Main Service] Connection established\n"RESET);
		break;

		//* If receive a data from client*/
	case LWS_CALLBACK_RECEIVE:
		printf(KCYN_L"[Main Service] Server received:%s\n"RESET, (char *) in);

		//* echo back to client*/
		websocket_write_back(wsi, (char *) in, -1);

		break;
	case LWS_CALLBACK_CLOSED:
		printf(KYEL"[Main Service] Client close.\n"RESET);
		break;

	default:
		break;
	}

	return 0;
}

struct per_session_data {
	int fd;
};

int startPlyrSrvr() {
	// server url will usd port 5000
	int port = PLYRPORT;
	const char *interface = NULL;
	struct lws_context_creation_info info;
	struct lws_protocols protocol;
	struct lws_context *context;
	// Not using ssl
	const char *cert_path = NULL;
	const char *key_path = NULL;
	// no special options
	int opts = 0;

	//* register the signal SIGINT handler */
	struct sigaction act;
	//act.sa_handler = INT_HANDLER;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	//sigaction(SIGINT, &act, 0);

	//* setup websocket protocol */
	protocol.name = "my-echo-protocol";
	protocol.callback = ws_service_callback;
	protocol.per_session_data_size = sizeof(struct per_session_data);
	protocol.rx_buffer_size = 0;

	//* setup websocket context info*/
	memset(&info, 0, sizeof info);
	info.port = port;
	info.iface = interface;
	info.protocols = &protocol;
	info.extensions = lws_get_internal_extensions();
	info.ssl_cert_filepath = cert_path;
	info.ssl_private_key_filepath = key_path;
	info.gid = -1;
	info.uid = -1;
	info.options = opts;

	//* create libwebsocket context. */
	context = lws_create_context(&info);
	if (context == NULL) {
		printf(KRED"[Main] Websocket context create error.\n"RESET);
		return -1;
	}

	printf(KGRN"[Main] Websocket context create success.\n"RESET);

	//* websocket service */
	while (!destroy_flag) {
		lws_service(context, 50);
	}
	usleep(10);
	lws_context_destroy(context);

	return 0;
}
