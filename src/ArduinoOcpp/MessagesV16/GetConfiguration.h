// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#ifndef GETCONFIGURATION_H
#define GETCONFIGURATION_H

#include <Variants.h>

#include <ArduinoOcpp/Core/OcppMessage.h>
#include <LinkedList.h>

namespace ArduinoOcpp {
namespace Ocpp16 {

class GetConfiguration : public OcppMessage {
private:
  LinkedList<String> keys;
public:
  GetConfiguration();
  ~GetConfiguration();

  const char* getOcppOperationType();

  void processReq(JsonObject payload);

  DynamicJsonDocument* createConf();

};

} //end namespace Ocpp16
} //end namespace ArduinoOcpp
#endif
