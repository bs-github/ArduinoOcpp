// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#ifndef OCPPSERVER_H
#define OCPPSERVER_H

#include <vector>
#include <WebSocketsServer.h>

namespace ArduinoOcpp {

typedef uint8_t WsClient;
typedef std::function<bool(const char*, size_t)> ReceiveTXTcallback;

namespace EspWiFi {

struct ReceiveTXTroute {
    IPAddress ip_addr;
    WsClient num;
    ReceiveTXTcallback processTXT;
};

class OcppServer {
private:

    std::vector<ReceiveTXTroute> receiveTXTrouting;

    WebSocketsServer wsockServer = WebSocketsServer(80);

    OcppServer();
    static OcppServer *instance;
public:
    static OcppServer *getInstance();

    void loop();

    void wsockEvent(WsClient num, WStype_t type, uint8_t * payload, size_t length);

    void setReceiveTXTcallback(IPAddress &ip_addr, ReceiveTXTcallback &callback);

    void removeReceiveTXTcallback(IPAddress &ip_addr);

    bool sendTXT(IPAddress &ip_addr, String &out);
};

} //end namespace EspWiFi
} //end namespace ArduinoOcpp
#endif
