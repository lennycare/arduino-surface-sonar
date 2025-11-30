#include <Servo.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

const int pinSignal = 7; 
const int pinServo = 9;

Servo monServo;
char carteRadar[16]; 
bool radarActif = false; 

void setup() {
  Serial.begin(9600);
  monServo.attach(pinServo);
  monServo.write(0); 
  lcd.begin(16, 2); 
  lcd.setBacklight(1); 
  Serial.println("SYSTEME RADAR INITIALISE");
  Serial.println("Tapez 'start' pour commencer et 'stop' pour arreter.");
  lcd.print(" PRET - ATTENTE ");
}

void loop() {
  if (Serial.available() > 0) {
    String commande = Serial.readStringUntil('\n'); 
    commande.trim(); 
    
    if (commande == "start") {
      radarActif = true;
      Serial.println(">> RADAR ACTIVE");
      lcd.clear();
      for(int i=0; i<16; i++) {
        carteRadar[i] = ' '; 
      }
      lcd.setCursor(0,0);
      lcd.print("                ");
    }
    else if (commande == "stop") {
      radarActif = false;
      monServo.write(0);
      Serial.println(">> RADAR DESACTIVE");
    }
  }

  if (radarActif) {
      // Balayage du servo de 0 à 180 degrés.
      for (int angle = 0; angle <= 180; angle += 12) {
        effectuerMesure(angle);
      }
      imprimerLaCarte("ALLER");
      // Balayage retour du servo de 180 à 0 degrés.
      for (int angle = 180; angle >= 0; angle -= 12) {
        effectuerMesure(angle);
      }
      imprimerLaCarte("RETOUR");
  }
}

void effectuerMesure(int angle) {
  monServo.write(angle);
  delay(150); // Laisse le temps au servo de se positionner.
  
  int distance = mesurerDistance();
  
  int index = angle / 12; // Convertit l'angle (0-180) en une colonne sur l'écran (0-15).
  if (index > 15) index = 15; 
  
  char symbole = determinerSymbole(distance);
  carteRadar[index] = symbole; // Met à jour la carte en mémoire avec le nouveau symbole.

  lcd.setCursor(index, 0);
  lcd.print(symbole); // Affiche le symbole directement sur l'écran.

  lcd.setCursor(0, 1);
  lcd.print("DEG:");
  lcd.print(angle);
  if (distance < 315) {
    lcd.print(" CM:");
    lcd.print(distance);
  }
  else {
    lcd.print(" VIDE");
  }
  lcd.print("     "); // Efface le reste de la ligne pour éviter les anciens caractères.
}

int mesurerDistance() {
  pinMode(pinSignal, OUTPUT);
  digitalWrite(pinSignal, LOW);
  delayMicroseconds(2);
  digitalWrite(pinSignal, HIGH);
  delayMicroseconds(5);
  digitalWrite(pinSignal, LOW);

  pinMode(pinSignal, INPUT); // Passe la broche en mode écoute.
  long duree = pulseIn(pinSignal, HIGH); // Mesure le temps de l'écho en microsecondes.
  return duree * 0.034 / 2; // Calcule la distance en cm (durée * vitesse du son / 2).
}

char determinerSymbole(int dist) {
  if (dist == 0 || dist > 315) {
    return '+'; // Symbole pour une distance hors de portée ou une erreur de mesure.
  }
  int index = dist / 35; // Convertit la distance en une valeur de 0 à 8.
  if (index > 8) index = 8;
  return '1' + index; // Renvoie un caractère de '1' à '9' selon la proximité de l'objet.
}

void imprimerLaCarte(String sens) {
  Serial.print("MAP ["); Serial.print(sens); Serial.println("] :");
  Serial.print("|");
  for (int i = 0; i < 16; i++) {
    Serial.print(carteRadar[i]); // Affiche la carte complète dans le moniteur série pour le débogage.
  }
  Serial.println("|");
}