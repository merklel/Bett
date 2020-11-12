

#define PIN_KLATSCHS 8

#define PIN_R1 5
#define PIN_G1 6
#define PIN_B1 3

#define PIN_R2 11
#define PIN_G2 10
#define PIN_B2 9

#define PIN_TASTER_1 4
#define PIN_TASTER_2 2



void setup() {
  // put your setup code here, to run once:


  Serial.begin(115200);

  Serial.println("Start!");

  pinMode(PIN_KLATSCHS, INPUT);

  pinMode(PIN_R1, OUTPUT);
  pinMode(PIN_G1, OUTPUT);
  pinMode(PIN_B1, OUTPUT);

  pinMode(PIN_R2, OUTPUT);
  pinMode(PIN_G2, OUTPUT);
  pinMode(PIN_B2, OUTPUT);

  pinMode(PIN_TASTER_1, INPUT);
  pinMode(PIN_TASTER_1, INPUT_PULLUP);

  pinMode(PIN_TASTER_2, INPUT);
  pinMode(PIN_TASTER_2, INPUT_PULLUP);


}





class LedLeiste {
  private:
    double dimm = 0;
    double dimm_alt = 1;
    int r = 0;
    int g = 0;
    int b = 0;
    int r_alt = 255;
    int g_alt = 255;
    int b_alt = 255;
    int dimmRichtung = 0;
    int modus = 1;
    int flag_an_aus = 0;
    double startZeit = 0;
    double zweitZeit = 0;
    double drittZeit = 0;

  public:
    LedLeiste() {
      //Konstruktor
      dimm = 0;
      modus = 5;
      dimm_alt = 1;
      r_alt = 255;
      g_alt = 240;
      b_alt = 10;
      dimmRichtung = 0;
      flag_an_aus = 0;
    }

    void anschalten() {
      r = r_alt;
      g = g_alt;
      b = b_alt;
      this->dimm = this->dimm_alt;
      this->flag_an_aus = 1;
    }

    void ausschalten() {
      r_alt = r;
      g_alt = g;
      b_alt = b;
      r = 0;
      g = 0;
      b = 0;
      dimm_alt = dimm;
      dimm = 0;
      flag_an_aus = 0;
    }

    void dimmen() {
      if (flag_an_aus) {

        if (dimmRichtung == 0) {
          dimm = dimm - 0.002;
          if (dimm <= 0) {
            dimmRichtung = 1;
          }
        }
        if (dimmRichtung == 1) {
          dimm = dimm + 0.002;
          if (dimm >= 1) {
            dimmRichtung = 0;
          }
        }


      }

    }

    void changeModus() {
      if (flag_an_aus) {
        modus = modus + 1;
        if (modus > 5) {
          modus = 1;
        }

        if (modus == 1) {
          r = 255;
          g = 255;
          b = 255;
        }
        if (modus == 2) {
          r = 255;
          g = 0;
          b = 0;
        }
        if (modus == 3) {
          r = 0;
          g = 0;
          b = 255;
        }
        if (modus == 4) {
          r = 0;
          g = 255;
          b = 0;
        }
        if (modus == 5) {
          r = 255;
          g = 240;
          b = 10;
        }
      }
    }

    double getDimm() {
      return dimm;
    }
    double getDimm_alt() {
      return dimm_alt;
    }

    int readTaster(int pin, int status_t) {
      int warteAufZweiten = 1;
      startZeit = 0;
      zweitZeit = 0;
      drittZeit = 0;

      // Erster Impuls
      if ( (digitalRead(pin) == 0) and (status_t == 0  or status_t == 34)) {
        entprellen(10000);

        startZeit = millis();

        //Drückzeit 1 abwarten
        while (digitalRead(pin) == 0) {
          zweitZeit = millis();
          int deltaZeit2 = zweitZeit - startZeit;
          if (deltaZeit2 > 800) {
            //Dauerdrücken erkannt
            return 3;
          }
        }

        // While warten auf zweiten impuls
        while (warteAufZweiten) {
          drittZeit = millis();
          int deltaZeit3 = drittZeit - startZeit;

          if (deltaZeit3 > 350) {
            warteAufZweiten = 0;
          }
          if ( (digitalRead(pin) == 0)) {
            entprellen(10000);
            warteAufZweiten = 0;
            Serial.println("in 4");
            delay(150);
            //Doppelklick erkannt
            return 4;
          }

        }

        int deltaZeit1 = zweitZeit - startZeit;

        if (deltaZeit1 <= 300) {
          Serial.println("in 1");
          // Einfach Klick erkalannt
          return 1;
        } else if (deltaZeit1 > 300 and deltaZeit1 <= 800 and status_t != 3) {
          Serial.println("in 2");
          //Lange erkannt
          return 2;
        }

      }

      if (digitalRead(pin) == 0 and status_t == 3) {
        // Immer noch gedrückt
        return 3;
      }

      return 0;
    }

    int getR() {
      return this->r * this->dimm;
    }
    int getG() {
      return this->g * this->dimm;
    }
    int getB() {
      return this->b * this->dimm;
    }

};

void entprellen(int zeit) {
  delayMicroseconds(zeit);
}


LedLeiste rechts;
LedLeiste links;

int status_taster1 = 0;
int flag_an_aus_1 = 0;

int status_taster2 = 0;
int flag_an_aus_2 = 0;

int i = 0;

void loop() {

  i++;

  status_taster1 = rechts.readTaster(PIN_TASTER_1, status_taster1);
  entprellen(10000);
  status_taster2 = links.readTaster(PIN_TASTER_2, status_taster2);

  // Mögliche Statusse des Tasters
  // status_taster == 1: Kurz
  // status_taster == 2: Lang
  // status_taster == 3: Dauer
  // status_taster == 4: Doppelt


  /////////////////////////////////////////////////////
  // Erste Leiste (rechts)

  if (status_taster1 == 1) {
    Serial.println("Kurz");
    if (flag_an_aus_1 == 0) {
      rechts.anschalten();
      flag_an_aus_1 = 1;
    } else if (flag_an_aus_1 == 1) {
      rechts.ausschalten();
      flag_an_aus_1 = 0;
    }
  }

  if (status_taster1 == 2) {
    Serial.println("Lang");
    // Bei langem klick soll das andere licht getriggert werden!
    if (flag_an_aus_1 == 0) {
      links.anschalten();
      flag_an_aus_1 = 1;
    } else if (flag_an_aus_1 == 1) {
      links.ausschalten();
      flag_an_aus_1 = 0;
    }

  }

  if (status_taster1 == 3) {
    Serial.println("gedrückt gehalten");
    rechts.dimmen();
  }

  if (status_taster1 == 4) {
    Serial.println("Doppelt");
    rechts.changeModus();
  }






  /////////////////////////////////////////////////////
  // Zweite Leiste (links)

  if (status_taster2 == 1) {
    Serial.println("Kurz");
    if (flag_an_aus_2 == 0) {
      links.anschalten();
      flag_an_aus_2 = 1;
    } else if (flag_an_aus_2 == 1) {
      links.ausschalten();
      flag_an_aus_2 = 0;
    }
  }

  if (status_taster2 == 2) {
    Serial.println("Lang");
    // Bei langem klick soll das andere licht getriggert werden!
    if (flag_an_aus_2 == 0) {
      rechts.anschalten();
      flag_an_aus_2 = 1;
    } else if (flag_an_aus_2 == 1) {
      rechts.ausschalten();
      flag_an_aus_2 = 0;
    }
  }

  if (status_taster2 == 3) {
    Serial.println("gedrückt gehalten");
    links.dimmen();
  }

  if (status_taster2 == 4) {
    Serial.println("Doppelt");
    links.changeModus();
  }




  //////////////////////////////////////////////////////////////////////////////
  //----------------- Klatsch Steuerung ----------------------------------------
  //----------------------------------------------------------------------------
  //  int warten = 0;
  //  int startZeit = 0;
  //  int zweitZeit = 0;
  //  if (digitalRead(PIN_KLATSCHS)) {
  //    startZeit = millis();
  //    warten = 1;
  //  }
  //  while (warten) {
  //    zweitZeit = millis();
  //    Serial.println("schleife grammar nazi evi");
  //
  //    if ( digitalRead(PIN_KLATSCHS) and ((zweitZeit - startZeit) < 500) and ((zweitZeit - startZeit) > 50) and (flag_an_aus_1 == 1 or flag_an_aus_2==1)) {
  //     links.ausschalten();
  //     rechts.ausschalten();
  //     flag_an_aus_1 = 0;
  //     flag_an_aus_2 = 0;
  //    }
  ////    else if ( digitalRead(PIN_KLATSCHS) and ((zweitZeit - startZeit) < 500) and ((zweitZeit - startZeit) > 50) and flag_an_aus == 1) {
  ////      wert_alt = wert;
  ////      wert = 0;
  ////      warten = 0;
  ////      flag_an_aus = 0;
  ////    }
  //
  //    if ((zweitZeit - startZeit ) > 1000) {
  //      warten = 0;
  //      Serial.println("shcleife stop");
  //    }
  //  }
  //
  //  //delayMicroseconds(200);



  Serial.println(links.getDimm());


  analogWrite(PIN_R1, rechts.getR());
  analogWrite(PIN_G1, rechts.getG());
  analogWrite(PIN_B1, rechts.getB());

  analogWrite(PIN_R2, links.getR());
  analogWrite(PIN_G2, links.getG());
  analogWrite(PIN_B2, links.getB());

}
