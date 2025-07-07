
  
  
  
// ### JSON Objekt für MQTT pulish generieren ###
  
void MQTTandJSON() {

  
  MAP_PWM_TO_PERCENT(PuPwmWert, PuPwmProz); // Prozentwert Drehzahl Pumpe erstellen


  // JSON-Dokument mit maximalem Kapazität von 256 Bytes erstellen

  StaticJsonDocument<256> doc; //Static

  // Variablen im JSON-Dokument speichern

  doc["WLAN_QoS"]["RSSI"] = rssi; // WLAN Signalstärke
  doc["Temperaturen"]["Temp_K"] = RohTemp_K; // Temperatur Kollektor roh nicht aufbereitet
  doc["Temperaturen"]["Temp_P"] = RohTemp_P; // Temperatur Puffer roh nicht aufbereitet
  doc["Temperaturen"]["TDiffPK"] = TempDiff_K_P; // Differenztemp. P zu K
  doc["Betriebsarten"]["BA"] = BAselect; // Betriebsart Nr.: 1-3
  doc["Betriebsarten"]["PiH"] = PiH_state; // Status Pumpe in BA HAND
  doc["Pumpe"]["PWMout"] = PuPwmWert; // PWM Ansteuerung Pumpe Format Byte 0-255
  doc["Pumpe"]["PWMproz"] = PuPwmProz; // PWM Ansteuerung Pumpe Format Prozent 0-100%
  doc["Hysterese"]["HyM"] = HyM; // Hysteresemerker | Differenztemperatur im Bereich der Hysterese für Pumpe EIN/AUS
  doc["Hysterese"]["P_Hy_E"] = P_Hy_E; // Pumpe soll ab Tempdiff X starten
  doc["Hysterese"]["P_Hy_A"] = P_Hy_A; // Pumpe soll ab Tempdiff X stoppen

  // JSON-String erstellen
  char json_data[256];
  serializeJson(doc, json_data);
  //Serial.println("JSON Frame Mastertopic:");
  //Serial.println(json_data);
    
  // Publish die JSON Message via MQTT
  
  client.publish(willTopic, willMessageOn, willRetain);   // Topic für Last Will Message: "Client wieder ONLINE"
  client.publish(PubMasterTopic, json_data); // Sende WLAN Signalstärke als JSON Objekt an MQTT Server

  //client.publish(PubWlanSignalTopic, json_data); // Sende WLAN Signalstärke als JSON Objekt an MQTT Server
  //client.publish(PubWlanSignalTopic, String(rssi).c_str()); // Sende WLAN Signalstärke des IoT
}



// MQTT Befehle vom OH empfangen im JSON Format CALLBACK

void callback(char* SubMasterTopic, byte* payload, unsigned int length) {
  Serial.print("Received message [");
  Serial.print(SubMasterTopic);
  Serial.print("] ");
  

  // Umwandlungsschleife des empfangenen Payload in eine Zeichenfolge

  String payloadStr;
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  // Kommandos empfangenen und übersetzen des JSON-Objekts
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payloadStr);

  if (error) {
    Serial.println("Error parsing JSON");
    return;
  }

  // Kommandos via REMOTE empfangen und übersetzen

  // BA Anwahl Remote | Wenn BA Inhalt ungültig verharrt die BA im letzten Zustand | Problem JSON Frame Kommandos über ein Topic
  int BAcom = doc["BA"];

  if (BAcom == 0) {
    Serial.print("Inhalt von BA UNGÜLTIG, BA wird nicht geändert");
    Serial.println();
    Serial.print("BAcom ist: ");
    Serial.print(BAcom);
    Serial.println();
    BAselect = BAselect;
    Serial.print("BAselect ist: ");
    Serial.print(BAselect);
    Serial.println();
  }
  else {
    Serial.print("Inhalt von BA GÜLTIG, BA wird geändert");
    Serial.println();
    Serial.print("BAcom ist: ");
    Serial.print(BAcom);
    Serial.println();
    BAselect = BAcom;
    Serial.print("BAselect ist: ");
    Serial.print(BAselect);
    Serial.println();
  }
  

  
  // Pumpe in BA Hand Remote
  PiHcom = doc["PiH"];

  REMOTE_PUMPE_EIN_AUS();

  Serial.println("Kommando BA empfangen");
  Serial.println("BAselect:");
  Serial.println(BAselect);

  Serial.println("PiH_stat:");
  Serial.println(PiH_state);
}


  // Hysterese über Remote einstellen

  


// Wiederherstellung MQTT Server Verbindung nach Verlust

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");

    if (client.connect(mqttDeviceId, mqttUsername, mqttPassword, willTopic, willQoS, willRetain, willMessage)) {
      Serial.println("connected");
    }
    else {
      Serial.println("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



