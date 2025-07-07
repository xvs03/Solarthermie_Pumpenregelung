
  // BA Nummer Vorwahl durchzählen 1-2-3-1-2-3-... NUR für HvO Taster

  void BA_A_H_U() {
  
    // BA Nummern durchzählen 1-3
    BAselect++; // Eine BA Nummer hochzählen
    if (BAselect > 3) {
      BAselect = 1;
    } 

    Serial.println("Betriebsart:");
    Serial.println(BAselect);
    Serial.println("---------------");
  }


  // Betriebsarten erkennen und entsprechende BA Funktion aufrufen

  void BA_Detection() {
    switch (BAselect) {
      case 1:
        BA_AUTO(HyM);
        Serial.println("BA AUTO");
        break;
      case 2:
        BA_HAND();
        Serial.println("BA HAND");
        break;
      case 3:
        BA_URLAUB();
        Serial.println("BA URLAUB");
        break;
      default:
        BA_AUTO(HyM); // Im Zweifel immer AUTO
        Serial.println("BA AUTO DEFAULT");
        break;
    }
  }

  // BA Vorwahl AUTO

  void BA_AUTO(bool &m) {
    Serial.print("BA_AUTO angewählt");
    Serial.println();
    PiH_state = false; // Status Pumpe in HAND = AUS setzen 
    digitalWrite(PuPower, HIGH); // Pumpe Versorgungsspannung EIN
    Serial.print("Versorgungsspannung PUMPE EIN");
    Serial.println();
    stateResetH = true; // PWM Signale bei BA wieder umschalten auf HAND resetten

      // Bedingungen für Pumpenanlauf
      if(m == true){
        Serial.print("Hysterese FREIGABE Pumpenlauf");
        Serial.println();
        MAP_PWM_TO_PERCENT(PuPwmWert, PuPwmProz); // PWM Wert in Prozent wandeln
        PUMPPOWER(TempDiff_K_P, PuPwmProz); // Pumpenleistung nach TempDiff P-K regeln
      }
      else{
        Serial.print("Hysterese SPERRE Pumpenlauf");
        Serial.println();
        PuPwmProz = 0; // Pumpe AUS 0 %
        MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert);
        digitalWrite(PuPwmSig, PuPwmWert);
        Serial.print("Pumpe Leistung auf: ");
        Serial.print(PuPwmProz);
        Serial.print(" %");
        Serial.println();
        Serial.print("Pumpe PWM Wert auf: ");
        Serial.print(PuPwmWert);
        Serial.println();

      }
  }


  // BA Vorwahl HAND

  void BA_HAND() {    
    Serial.print(" In Funktion BA_HAND # # gesprungen ");
    // 1x Reset aller PWM und Merker bei Wiederanwahl BA HAND
    if (stateResetH == true) {
      PiH_state = false;
      PuPwmWert = 0; // PWM Werrt 0 setzen
      stateResetH = false;
      Serial.println("1x Reset !!!");
    }
  }


  // BA Vorwahl URLAUB | Ist der gleiche Betrieb wie BA_AUTO nur mit einer Pufferkühlung die über das OH konfiguriert wird

  // !!!!! BA URAUB noch nicht voll in Funktion !!!!!

  void BA_URLAUB() {
    PiH_state = false; // Status Pumpe in HAND = AUS setzen    
    Serial.print("In Funktion BA_URLAUB # # # gesprungen");
    stateResetH = true; // PWM Signale bei BA wieder umschalten auf HAND resetten
  }


  // In BA HAND Pumpe EIN/AUS über Taster am HvO Interruptaktion

  void TASTER_PUMPE_HAND() {
    if(BAselect == 2){    
      Serial.print(" In Funktion PUMPE_HAND # # + für EIN/AUS ");
      if(PiH_state == false){
        PiH_state = true;
        Serial.println();
        PuPwmWert = 255; // PWM auf 100%
        Serial.print(" Pumpe über HAND eingeschaltet ");
        
        Serial.print(PiH_state);
        Serial.print(" # ");
        Serial.print(PuPwmWert);
      }
        else{
          PiH_state = false;
          Serial.print(" Pumpe über HAND ausgeschaltet ");
          PuPwmWert = 0;
          Serial.print(PiH_state);
          Serial.print(" # ");
          Serial.print(PuPwmWert);
          Serial.println();
          


        }
      Serial.print("PiH State nach HvO Taster Betätigung : ");
      Serial.print(PiH_state);
    }
  }

  // In BA HAND Pumpe EIN/AUS über REMOTE

  void REMOTE_PUMPE_EIN_AUS() {
    if (BAselect == 2 && PiHcom == true && PiH_state == false){
        PiH_state = true;
        Serial.println("Pumpe über REMOTE EIN geschaltet");
        PuPwmWert = 255;
      
    }
    if (BAselect == 2 && PiHcom == false && PiH_state == true){
        PiH_state = false;
        Serial.println("Pumpe über REMOTE AUS geschaltet");
        PuPwmWert = 0;
      }

    Serial.println("PiH_state nach REMOTE Kommando : ");
    Serial.println(PiH_state);
  }


