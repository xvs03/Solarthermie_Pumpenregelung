// +++ Pumpenleistung nach Temperaturdifferenzen von K u. P in % regulieren +++


// Funktion in BAs d = DiffTemp K+P , pz = Pumpenleistungs % Zahl
void PUMPPOWER(float &d, int &pz){


  //float td0 = PuTempDiffGrenz[0], td1 = PuTempDiffGrenz[1], td2 = PuTempDiffGrenz[2], td3 = PuTempDiffGrenz[3], td4 = PuTempDiffGrenz[4], td5 = PuTempDiffGrenz[5], td6 = PuTempDiffGrenz[6], td7 = PuTempDiffGrenz[7], td8 = PuTempDiffGrenz[8], td9 = PuTempDiffGrenz[9], td10 = PuTempDiffGrenz[10];  // 11 Temperaturdifferenzgrenzwerte für Pumpenleistungsstufen [C°]
  //uint8_t pz0 = 30, pz1 = 35, pz2 = 40, pr3 = 45, pz4 = 50, pz5 = 55, pz6 = 60, pz7 = 65, pz8 = 70, pz9 = 75, pz10 = 100; // 11 Pumpenleistungsstufen für Temperaturdifferenzgrenzwerte [%]

  //float PuTempDiffGrenz[11]; // Array für die Pumpenleistungsregelung über Temperaturdifferenzgrenzwerte von K u. P | 11 Stufen
  //int PuPowerSteps[11]; // Array Pumpenleistungsstufen für Temperaturdifferenzgrenzwerte | 11 Stufen


  // Pumpenleistung für verschiedene Differenztemperaturen von K u. P

  
  // Temperaturgrenze 1 von 0°C an | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[0]){
    pz = PuPowerSteps[0]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 1");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 2 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[1] && d > PuTempDiffGrenz[0] ){
    pz = PuPowerSteps[1]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 2");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 3 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[2] && d > PuTempDiffGrenz[1] ){
    pz = PuPowerSteps[2]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 3");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 4 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[3] && d > PuTempDiffGrenz[2] ){
    pz = PuPowerSteps[3]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 4");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 5 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[4] && d > PuTempDiffGrenz[3] ){
    pz = PuPowerSteps[4]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 5");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 6 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[5] && d > PuTempDiffGrenz[4] ){
    pz = PuPowerSteps[5]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 6");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 7 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[6] && d > PuTempDiffGrenz[5] ){
    pz = PuPowerSteps[6]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 7");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 8 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[7] && d > PuTempDiffGrenz[6] ){
    pz = PuPowerSteps[7]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 8");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 9 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[8] && d > PuTempDiffGrenz[7] ){
    pz = PuPowerSteps[8]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 9");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }

  // Temperaturgrenze 10 | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[9] && d > PuTempDiffGrenz[8] ){
    pz = PuPowerSteps[9]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 10");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }  

  // Temperaturgrenze 11 LETZTE | variabel einstellbar über OH
  if( d < PuTempDiffGrenz[10] && d > PuTempDiffGrenz[9] ){
    pz = PuPowerSteps[10]; // Eingabe Pumpenleistung in Prozent % aus EEPROM
    Serial.println("Temperaturgrenze 11");
    Serial.print("Pumpenleistung: ");
    Serial.print(pz);
    Serial.print(" %");
    Serial.println();
    MAP_PERCENT_TO_PWM(PuPwmProz, PuPwmWert); // Umwandeln von Prozent auf PWM Wert
    digitalWrite(PuPwmSig, PuPwmWert);
    Serial.print("PWM Wert für Pumpenleistung (informativ): ");
    Serial.print(PuPwmWert);
    Serial.println();
  }  

} // PUMPPOWER END