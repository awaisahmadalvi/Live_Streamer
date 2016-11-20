#include "client_ws.hpp"

using namespace std;

typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;

int main() {
    WsClient client("localhost:8080/echo");
    client.onmessage=[&client](shared_ptr<WsClient::Message> message) {
        auto message_str=message->string();
        
        cout << "Client: Message received: \"" << message_str << "\"" << endl;
        
        cout << "Client: Sending close connection" << endl;
        client.send_close(1000);
    };
    
    client.onopen=[&client]() {
        cout << "Client: Opened connection" << endl;
        
        string message="Hello";
        cout << "Client: Sending message: \"" << message << "\"" << endl;

        auto send_stream=make_shared<WsClient::SendStream>();
        *send_stream << message;
        client.send(send_stream);
    };
    
    client.onclose=[](int status, const string& /*reason*/) {
        cout << "Client: Closed connection with status code " << status << endl;
    };
    
    //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    client.onerror=[](const boost::system::error_code& ec) {
        cout << "Client: Error: " << ec << ", error message: " << ec.message() << endl;
    };
    
    client.start();
    
    return 0;
}
