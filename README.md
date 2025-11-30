# Système Radar Sonar avec Arduino

Ce projet transforme une carte Arduino en un système radar simple utilisant un capteur à ultrasons monté sur un servomoteur. Le radar scanne un arc de 180 degrés et affiche une carte textuelle des obstacles détectés sur un écran LCD 16x2 et via le moniteur série.

## Matériel Requis

*   Une carte Arduino (Uno, Nano, etc.)
*   Un servomoteur (ex: SG90)
*   Un capteur de distance à ultrasons (ex: HC-SR04)
*   Un écran LCD I2C 16x2
*   Des fils de connexion (jumper wires)
*   Une platine d'expérimentation (breadboard)

## Branchements

Assurez-vous que les composants sont correctement alimentés (5V et GND).

*   **Servomoteur :**
    *   Fil de signal (orange/jaune) sur la broche `D9` de l'Arduino.
*   **Capteur Ultrasons (HC-SR04) :**
    *   Les broches `Trig` et `Echo` sont connectées ensemble à la broche `D7` de l'Arduino.
*   **Écran LCD I2C :**
    *   `SDA` sur la broche `A4` de l'Arduino (ou `SDA` sur les cartes plus récentes).
    *   `SCL` sur la broche `A5` de l'Arduino (ou `SCL` sur les cartes plus récentes).

## Installation

1.  **Bibliothèques Arduino :**
    Assurez-vous d'avoir installé les bibliothèques suivantes via le gestionnaire de bibliothèques de l'IDE Arduino :
    *   `Servo` (généralement incluse par défaut)
    *   `Wire` (généralement incluse par défaut)
    *   `Adafruit LiquidCrystal` par Adafruit

2.  **Téléversement :**
    Ouvrez le fichier `arduino_code.cpp` dans l'IDE Arduino, sélectionnez le bon type de carte et le bon port, puis téléversez le code.

## Utilisation

1.  Ouvrez le **Moniteur Série** dans l'IDE Arduino (débit de 9600 bauds).
2.  Le message `SYSTEME RADAR INITIALISE` s'affiche.
3.  Tapez `start` et appuyez sur Entrée pour commencer le balayage.
4.  Tapez `stop` et appuyez sur Entrée pour arrêter le balayage.

## Fonctionnement

Le servomoteur effectue un balayage de 0 à 180 degrés par pas de 12 degrés. À chaque pas, le capteur à ultrasons mesure la distance d'un éventuel obstacle.

### Affichage

*   **Écran LCD :** L'écran est mis à jour en temps réel.
    *   La **première ligne** affiche une carte de 16 caractères représentant l'environnement. Chaque caractère correspond à un segment de 12 degrés de l'arc de balayage.
    *   La **deuxième ligne** affiche l'angle actuel (`DEG`), la distance mesurée (`CM`) ou `VIDE` si aucun obstacle n'est détecté à portée.

*   **Moniteur Série :** À la fin de chaque balayage complet (aller ou retour), une carte complète est imprimée, par exemple :
    ```
    MAP - ALLER
    [+++++4269++++++]
    ```

### Symboles de la carte

La distance est convertie en un symbole pour un affichage simplifié :

*   `+` : Aucun obstacle détecté (distance > 315 cm).
*   `1` : Obstacle très proche (0-34 cm).
*   `2` à `8` : Obstacle à distance intermédiaire.
*   `9` : Obstacle lointain (280-314 cm).