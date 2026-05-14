// ****************************
// * Source de Mesures U et I *
// *          UXI             *
// ****************************

// PhDV61 pour André :  J'ai passé les compteurs d'Energie en double, car le float n'a pas la précision nécessaire pour que les petits incréments
// puissent être ajoutés aux totaux lorsqu'ils deviennent grand. Je me suis fait avoir en mettant le compteur total UxIx3 à 4 500 000. 
// Comme mon Linky - changé il y a environ 1 an. Les incréments <0.5 étaient perdus, et sur quelques heures cela représentait
//  une erreur de 200W. Plus de souci en double. Mais cela demande des vérifications en UxI que je ne peux tester à la maison.
// Je ne vois pas de page données brutes en UxI ? Je n'ai pas pu mettre de traces comme je l'ai fait en UxIx3 comme tu verras.

void Setup_UxI() {
  for (int i = 0; i < 100; i++) {  //Reset table measurements
    voltM[i] = 0;
    ampM[i] = 0;
  }
}
void LectureUxI() {
  MeasurePower();
  ComputePower();
}
void MeasurePower() {  //Lecture Tension et courants pendant 20ms
  int iStore;
  value0 = analogRead(AnalogIn0);  //Mean value. Should be at 3.3v/2 /35
  static unsigned long OverflowOffset = 0;
  static unsigned long PrevMicros = 0;
  unsigned long NowMicros;
  unsigned long MeasureMillis = millis();
  while (millis() - MeasureMillis < 21) {  //Read values in continuous during 20ms. One loop is around 150 micro seconds
    NowMicros = micros();
    if (NowMicros < PrevMicros) {
      OverflowOffset = (OverflowOffset+ 7296)%20000;
    }
    iStore = ((NowMicros%20000 + OverflowOffset) % 20000) / 200;  //We have more results that we need during 20ms to fill the tables of 100 samples
    volt[iStore] = analogRead(AnalogIn1) - value0;//32
    amp[iStore] = analogRead(AnalogIn2) - value0;//33
    PrevMicros = NowMicros;
  }
}

void ComputePower() {
  float PWcal = 0;  //Computation Power in Watt
  float V;
  float I;
  float Uef2 = 0;
  float Ief2 = 0;
  
  for (int i = 0; i < 100; i++) {
    voltM[i] = (19.0 * voltM[i] + float(volt[i])) / 20.0;  //Mean value. First Order Filter. Short Integration
    V = kV * voltM[i];
    Uef2 += sq(V);
    ampM[i] = (19.0 * ampM[i] + float(amp[i])) / 20.0;  //Mean value. First Order Filter
    I = kI * ampM[i];
    Ief2 += sq(I);
    PWcal += V * I;
  }
  // Écriture via pointeurs canal : alimente _M ou _T selon Set_Canal_Lecture()
  Uef2 = Uef2 / 100.0;        //square of voltage
  *pTension = sqrt(Uef2);     //RMS voltage
  Ief2 = Ief2 / 100.0;        //square of current
  *pIntensite = sqrt(Ief2);   // RMS current
  PWcal = PfloatMax(PWcal / 100.0);
  float PVA = PfloatMax(floor((*pTension) * (*pIntensite)));
  float PowerFactor = 0;
  if (PVA > 0) {
    PowerFactor = floor(100.0 * PWcal / PVA) / 100.0;
  }
  *pPowerFactor = PowerFactor;
  if (PWcal >= 0) {
    *pEASfloat += PWcal / 90000.0;         // Watt Hour,Every 40ms. Soutirée
    *pEnergie_Soutiree_double = *pEASfloat; // PhDV61 : compteur double sauvegardé pour reset
    *pEnergie_Soutiree = long(*pEASfloat);
    *pPuissanceS_inst = PWcal;
    *pPuissanceI_inst = 0;
    *pPVAS_inst = PVA;
    *pPVAI_inst = 0;
  } else {
    *pEAIfloat += -PWcal / 90000.0;
    *pEnergie_Injectee = long(*pEAIfloat);
    *pEnergie_Injectee_double = *pEAIfloat; // PhDV61 : compteur double sauvegardé pour reset
    *pPuissanceS_inst = 0;
    *pPuissanceI_inst = -PWcal;
    *pPVAS_inst = 0;
    *pPVAI_inst = PVA;
  }
  Pva_valide = true;
  if (cptLEDyellow > 30) {
    cptLEDyellow = 4;
  }
  filtre_puissance();
  EnergieActiveValide = true;
  PuissanceRecue = true;
}
