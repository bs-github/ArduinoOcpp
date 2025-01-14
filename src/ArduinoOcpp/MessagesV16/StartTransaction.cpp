// matth-x/ESP8266-OCPP
// Copyright Matthias Akstaller 2019 - 2021
// MIT License

#include <ArduinoOcpp/MessagesV16/StartTransaction.h>
#include <ArduinoOcpp/TimeHelper.h>
#include <ArduinoOcpp/Core/OcppEngine.h>
#include <ArduinoOcpp/Tasks/Metering/MeteringService.h>

#include <Variants.h>

using ArduinoOcpp::Ocpp16::StartTransaction;

/*
StartTransaction::StartTransaction() {
    if (getChargePointStatusService() != NULL) {
      if (!getChargePointStatusService()->getIdTag().isEmpty()) {
        idTag = String(getChargePointStatusService()->getIdTag());
      }
    }
    if (idTag.isEmpty())
      idTag = String("fefed1d19876"); //Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
}

StartTransaction::StartTransaction(String &idTag) {
    this->idTag = String(idTag);
}*/

StartTransaction::StartTransaction(int connectorId) : connectorId(connectorId) {
  ChargePointStatusService *cpss = getChargePointStatusService();
  if (cpss != NULL) {
      if (cpss->existsUnboundAuthorization()) {
          this->idTag = String(cpss->getUnboundIdTag());
      } else {
          //The CP is not authorized. Try anyway, let the CS decide what to do ...
          this->idTag = String("fefed1d19876");
      }
  } else {
    this->idTag = String("fefed1d19876"); //Use a default payload. In the typical use case of this library, you probably you don't even need Authorization at all
  }
}

StartTransaction::StartTransaction(int connectorId, String &idTag) : connectorId(connectorId) {
  this->idTag = String(idTag);
}

const char* StartTransaction::getOcppOperationType(){
    return "StartTransaction";
}

DynamicJsonDocument* StartTransaction::createReq() {
  DynamicJsonDocument *doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(5) + (JSONDATE_LENGTH + 1) + (idTag.length() + 1));
  JsonObject payload = doc->to<JsonObject>();

  payload["connectorId"] = connectorId;
  MeteringService* meteringService = getMeteringService();
  if (meteringService != NULL) {
	  payload["meterStart"] = meteringService->readEnergyActiveImportRegister(connectorId);
  }
  char timestamp[JSONDATE_LENGTH + 1] = {'\0'};
  getJsonDateStringFromGivenTime(timestamp, JSONDATE_LENGTH + 1, now());
  payload["timestamp"] = timestamp;
  payload["idTag"] = idTag;

  return doc;
}

void StartTransaction::processConf(JsonObject payload) {

  const char* idTagInfoStatus = payload["idTagInfo"]["status"] | "Invalid";
  int transactionId = payload["transactionId"] | -1;

  if (!strcmp(idTagInfoStatus, "Accepted")) {
    if (DEBUG_OUT) Serial.print(F("[StartTransaction] Request has been accepted!\n"));

    ChargePointStatusService *cpStatusService = getChargePointStatusService();
    ConnectorStatus *connector = getConnectorStatus(connectorId);
    if (cpStatusService != NULL && connector != NULL){
      cpStatusService->bindAuthorization(idTag, connectorId);
      connector->startTransaction(transactionId);
      connector->startEnergyOffer();
    }

    SmartChargingService *scService = getSmartChargingService();
    if (scService != NULL){
      scService->beginChargingNow();
    }

  } else {
    Serial.print(F("[StartTransaction] Request has been denied!\n"));
  }
}


void StartTransaction::processReq(JsonObject payload) {

  /**
   * Ignore Contents of this Req-message, because this is for debug purposes only
   */

}

DynamicJsonDocument* StartTransaction::createConf(){
  DynamicJsonDocument* doc = new DynamicJsonDocument(JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2));
  JsonObject payload = doc->to<JsonObject>();

  JsonObject idTagInfo = payload.createNestedObject("idTagInfo");
  idTagInfo["status"] = "Accepted";
  payload["transactionId"] = 123456; //sample data for debug purpose

  return doc;
}
