#include "LookupServer.h"
#include <uwebsockets/App.h>

int main() {
	uWS::SSLApp app = uWS::SSLApp({
		.key_file_name = "key.pem",
		.cert_file_name = "cert.pem",
		.passphrase = "123456789"
	});

    app.get("/*", [](auto* res, auto* req) {
        res->end("hi");
    });

    app.listen(9876, [](auto socket) {
        if(socket) {
            std::cout << "Listening on port 9876" << std::endl;
        }
    });

    app.run();
    return 0;
}