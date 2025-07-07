// Pumpendrehzahl von Byte 0-255 auf Prozent 0-100%

void MAP_PWM_TO_PERCENT(volatile int &PWMw, int &PWMp){
  PWMp = map(PWMw, 0, 255, 0, 100); // Wandelt den int Wert in ein Prozentwert
  Serial.print("Wandle PuPwmWert: ");
  Serial.print(PWMw);
  Serial.print(", in Prozentwert: ");
  Serial.print(PWMp);
  Serial.print(" %");
  Serial.println();
}
void MAP_PERCENT_TO_PWM(int &PWMp, volatile int &PWMw){
  PWMw = map(PWMp, 0, 100, 0, 255); // Wandelt den Integer in Prozent
  Serial.print("Wandele Pumpenprozentwert: ");
  Serial.print(PWMp);
  Serial.print(" %");
  Serial.println();
  Serial.print(", in Pumpen PWM Wert: ");
  Serial.print(PWMw);
  Serial.println();
}

// Temperatur Differenz Rechnung P zu K

void CALC_DIFF(float &x, float &y, float &z){
  x = y - z;
}

// Hystere Merker für Pumpe EIN/AUS
void HYSTERESE(float &d, bool &h, const float &zE, const float &zA){
  if (d >= zE) {
    h = true;
  }
  else if (d <= zA) {
    h = false;
  }
  Serial.print("Hysterese Merker ist: ");
  Serial.print(h);
  Serial.println();
}

