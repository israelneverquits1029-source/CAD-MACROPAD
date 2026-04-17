#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, )

const int btnOK = 2;
const int btnRight = 3;
const int btnDown = 4;
const int btnLeft = 5;
const int btnUp = 6;

const int encCLK = 7;
const int encDT = 8;
const int joySW = 9;
const int encSW = 10;
const int buzzerPin = 11;

const int joyX = A0;
const int joyY = A1;

enum ScreenState {
    HOME_SCREEN,
    SOFTWARE_MENU,
    ENCODER_MENU,
    JOYSTICK_MENU,
    BUTTON_CONFIG_MENU,
    ERROR_SCREEN
};

enum SoftwareProfile {
    PROFILE_CAD,
    PROFILE_KICAD,
    PROFILE_BLENDER
};

enum EncoderMode {
    ENC_ZOOM,
    ENC_SCALE,
    ENC_ROTATE
};

enum JoystickMode {
    JOY_PAN,
    JOY_ORBIT,
    JOY_SKETCH
};

enum ButtonMode {
    BTN_DEFAULT,
    BTN_SHORTCUT1,
    BTN_SHORTCUT2,
    BTN_NOT_CONFIGURED
};

const char* profileNames[] = {"CAD", "KiCAD", "Blender"};
const char* encoderNames[] = {"Zoom", "Scale", "Rotate"};
const char* joystickNames[] = {"Pan", "Orbit", "Sketch"};
const char* buttonModeNames[] = {"Default", "Shortcut 1", "Shortcut 2", "Not Configured"};
const char* dirNames[] = {"UP", "DOWN", "LEFT", "RIGHT"};

ScreenState currentScreen = HOME_SCREEN;
SoftwareProfile currentProfile = PROFILE_CAD;
EncoderMode currentEncoderMode = ENC_ZOOM;
JoystickMode currentJoystickMode = JOY_PAN;

int menuIndex = 0;
bool displayDirty = true;

ButtonMode buttonAssignments[4] = {
    BTN_DEFAULT,  // UP
    BTN_DEFAULT,  // DOWN
    BTN_DEFAULT,  // LEFT
    BTN_DEFAULT,  // RIGHT
};

int configTargetButton = 0; // 0=UP,1=DOWN,2=LEFT,3=RIGHT

String lastAction = "Ready";
String errorMessage = "";

bool lastBtnOK = HIGH;
bool lastBtnUp = HIGH;
bool lastBtnDown = HIGH;
bool lastBtnLeft = HIGH;
bool lastBtnRight = HIGH;
bool lastEncSW = HIGH;
bool lastJoySW = HIGH;

unsigned long okPressStart = 0;
unsigned long upPressStart = 0;
unsigned long downPressStart = 0;
unsigned long leftPressStart = 0;
unsigned long rightPressStart = 0;

const unsigned long longPressTime = 100;

int lastCLK = HIGH;

int joyCenter = 512;
int deadZone = 150;

void beep(int duration = 30) {
    digitalWrite(buzzerPin, HIGH);
    delay(duration);
    digitalWrite(buzzerPin, LOW);
}

void errorBeep() {
    digitalWrite(buzzerPin, LOW);
    delay(80);
    digitalWrite(buzzerPin, LOW)
}

void markDirty() {
    displayDirty = true;
}

void setAction(const String& msg, bool doBeep = true) {
    lastAction = msg;
    if (doBeep) beep(15);
    markDirty();
    Serial.println(msg);
}

void showError(const String& msg) {
    errorMessage = msg;
    currentScreen = ERROR_SCREEN;
    errorBeep();
    markDirty();
    Serial.println("ERROR: " + msg);
}

void drawHomeScreen() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("CAD Control Pad");

    display.setCursor(0, 10);
    display.print("Software: ");
    display.println(profileNames[currentProfile]);

    display.setCursor(0, 20);
    display.print("Encoder: ");
    display.println(encoderNames[currentEncoderMode]);

    display.setCursor(0, 30);
    display.print("Joystick: ");
    display.println("JoystickNames[currentJoystickMode]");
    
    display.setCursor(0, 40);
    display.print("UP: ");
    display.println(buttonModeNames[buttonsAssignment[0]]);

    display.setCursor(0, 50);
    display.print("Action: ");
    display.setCursor(0, 58);
    display.println(lastAction);

    display.display();
}

void drawSoftwareMenu() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("Select Software");

    for (int i = 0; i < 3; i++) {
        display.setCursor(0, 16 + i * 14);
        display.print(i == menuIndex ? "> " : "  ");
        display.println(profileNames[i]);
    }

    display.display();
}

void drawEncoderMenu() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("Encoder Mode");

    for (int i = 0; i < 3; i++) {
        display.setCursor(0, 16 + i * 14);
        display.print(i == menuIndex ? "> " : "  ");
        display.println(encoderNames[i]);
    }

    display.display();
}

void drawJoystickMenu() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("Joystick Mode");

    for (int i = 0; 1 < 3; i++) {
        display.setCursor(0, 16 + i * 14);
        display.print(i == menuIndex ? "> " : "  ");
        display.println(joystickNames[i]);
    }

    display.display();
}

void drawButtonConfigMenu() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Config ");
    display.println(dirNames[configTargetButton]);

    for (int i = 0; i < 4; i++) {
        display.setCursor(0, 14 + i * 12);
        display.print(i == menuIndex ? "> " : "  ");
        display.println(buttonModeNames[i]);
    }

    display.display();
}

void drawErrorScreen() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 10);
    display.println("Error");

    display.setCursor(0, 28);
    display.println(errorMessage);

    display.setCursor(0, 50);
    display.println("Press LEFT/OK");

    display.display();
}

void refreshDisplay() {
    if (!displayDirty) return;

    switch (currentScreen) {
        case HOME_SCREEN:   drawHomeScreen(); break;
        case SOFTWARE_MENU:   drawSoftwareMenu(); break;
        case ENCODER_MENU:   drawEncoderMenu(); break;
        case JOYSTICK_MENU:   drawJoystickMenu(); break;
        case BUTTON_CONFIG_MENU:   drawButtonConfigMenu(); break;
        case ERROR_SCREEN:   drawErrorScreen(); break;
    }

    displayDirty = false;
}

void goHome() {
    currentScreen = HOME_SCREEN;
    menuIndex = 0;
    markDirty();
}

void openSoftwareMenu() {
    currentScreen = SOFTWARE_MENU;
    menuIndex = currentProfile;
    beep();
    markDirty();
}

void openEncoderMenu() {
    currentScreen = ENCODER_MENU;
    menuIndex = currentEncoderMode;
    beep();
    markDirty();
}

void openJoystickMenu() {
    currentScreen = JOYSTICK_MENU;
    menuIndex = currentJoystickMode;
    beep();
    markDirty();
}

void openButtonConfigMenu(int target) {
    currentScreen = BUTTON_CONFIG_MENU;
    configTargetButton = target;
    menuIndex = buttonAssignments[target];
    beep();
    markDirty();
}

void moveMenuUp() {
    if (currentScreen == SOFTWARE_MENU || currentScreen == ENCODER_MENU || currentScreen == JOYSTICK_MENU || currentScreen == BUTTON_CONFIG_MENU) {
        menuIndex--;
        if (currentScreen == BUTTON_CONFIG_MENU) {
            if (menuIndex < 0) menuIndex = 3;
        } else {
            if (menuIndex < 0) menuIndex = 2;
        }
        beep(10);
        markDirty();
    }
}

void moveMenuDown() {
    if (currentScreen == SOFTWARE_MENU || currentScreen == ENCODER_MENU || currentScreen == JOYSTICK_MENU || currentScreen == BUTTON_CONFIG_MENU) {
        menuIndex++;
        if (currentScreen == BUTTON_CONFIG_MENU) {
            if (menuIndex > 3) menuIndex = 0;
        } else {
            if (menuIndex > 2) menuIndex = 0;
        }
        beep(10);
        markDirty();
    }
}

void selectCurrentMenuItem() {
    if (currentScreen == SOFTWARE_MENU) {
        currentProfile = (SoftwareProfile)menuIndex;
        setAction("Software set");
        goHome();
    } else if (currentScreen == ENCODER_MENU) {
        currentEncoderMode = (EncoderMode)menuIndex;
        setAction("Encoder set");
        goHome();
    } else if (currentScreen == JOYSTICK_MENU) {
        currentJoystickMode = (JoystickMode)menuIndex;
        setAction("Joystick set");
        goHome();
    } else if (currentScreen == BUTTON_CONFIG_MENU) {
        buttonAssignments[configTargetButton] = (ButtonMode)menuIndex;
        setAction(String(dirNames[configTargetButton]) + " assigned");
        goHome();
    } else if (currentScreen == ERROR_SCREEN) {
        goHome();
    }
}

void performDirectionalAction(int dir) {
    ButtonMode mode = buttonAssignments[dir];

    if (mode == BTN_NOT_CONFIGURED) {
        showError("Not configured");
        return;
    }

    if (mode == BTN_DEFAULT) {
        if (currentProfile == PROFILE_CAD) {
            setAction(String(dirNames[dir]) + " CAD move");
        } else if (currentProfile == PROFILE_KiCAD) {
            setAction(String(dirNames[dir]) + " KiCAD nav");
        } else if (currentProfile == PROFILE_BLENDER) {
            setAction(String(dirNames[dir]) + " Blender nav");
        }
    } else if (mode == BTN_SHORTCUT1) {
        if (currentProfile == PROFILE_CAD) {
            setAction(String(dirNames[dir]) + " Shortcut 1");
        } else {
            showError("Mode incomoatible");
        }
    } else if (mode == BTN_SHORTCUT2) {
        if (currentProfile == PROFILE_BLENDER) {
            setAction(String(dirNames[dir]) + " Shortcut 2");
        } else {
            showError("Mode incomoatible");
        }
    }
}

void encoderAction(bool clockwise) {
    if (currentScreen != HOME_SCREEN) return;

    if (currentEncoderMode == ENC_ZOOM) {
        setAction(clockwise ? "Zoom In" : "Zoom Out", false);
    } else if (currentEncoderMode == ENC_SCALE) {
        setAction(clockwise ? "Scale Up" : "Scale Down", false);
    } else if (currentEncoderMode == ENC_ROTATE) {
        setAction(clockwise ? "Rotate CW" : "Rotate CCW", false);
    }
}

void handleEncoderRotation() {
    int currentCLK = digitalRead(encCLK);

    if (currentCLK != lastCLK) {
        if (lastCLK == HIGH && currentCLK == LOW) {
            int dtState = digitalRead(encDT);

            if (dtState == HIGH) {
                encoderAction(true);
            } else {
                encoderAction(false);
            }
        }
    }

    lastCLK = currentCLK;
}

void handleJoystick() {
    if (currentScreen != HOME_SCREEN) return;

    int x = analogRead(joyX);
    int y = analogRead(joyY);

    if (currentJoystickMode == JOY_PAN) {
        if (x > joyCenter + deadZone) {
            setAction("Pan Left", false);
        } else if (x < joyCenter - deadZone) {
            setAction("Pan Right", false);
        }

        if (y > joyCenter + deadZone) {
            setAction("Pan Up", false);
        } else if (y < joyCenter - deadZone) {
            setAction("Pan Down", false);
        }
    }

    else if (currentJoystickMode == JOY_ORBIT) {
        if (x > joyCenter + deadZone) {
            setAction("Orbit Left", false);
        } else if (x < joyCenter - deadZone) {
            setAction("Orbit Right", false);
        }

        if (y > joyCenter + deadZone) {
            setAction("Orbit Up", false);
        } else if (y < joyCenter - deadZone) {
            setAction("Orbit Down", false);
        }
    }

    else if (currentJoystickMode == JOY_SKETCH) {
        if (x > joyCenter + deadZone) {
            setAction("Sketch Left", false);
        } else if (x < joyCenter - deadZone) {
            setAction("Sketch Right", false);
        }

        if (y > joyCenter + deadZone) {
            setAction("Sketch Up", false);
        } else if (y < joyCenter - deadZone) {
            setAction("Sketch Down", false);
        }
    }
}

void setup() {
    Serial.begin(115200);

    Wire.setSDA(0);
    Wire.setSCL(1);
    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (true) {}
    }

    pinMode(btnOK, INPUT_PULLUP);
    pinMode(btnRight, INPUT_PULLUP);
    pinMode(btnDown, INPUT_PULLUP);
    pinMode(btnLeft, INPUT_PULLUP);
    pinMode(btnUp, INPUT_PULLUP);
    
    pinMode(encCLK, INPUT);
    pinMode(encDT, INPUT);
    pinMode(encSW, INPUT_PULLUP);
    pinMode(joySW, INPUT_PULLUP);

    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);

    lastCLK = digitalRead(encCLK);

    markDirty();
    refreshDisplay();
    beep(60);
}

void loop() {
    bool currentOK = digitalRead(btnOK);
    bool currentUp = digitalRead(btnUp);
    bool currentDown = digitalRead(btnDown);
    bool currentLeft = digitalRead(btnLeft);
    bool currentRight = digitalRead(btnRight);
    bool currentEncoderSW = digitalRead(encSW);
    bool currentJoySW = digitalRead(joySW);

    if(currentOK == LOW && lastBtnOK == HIGH) {
        okPressStart = millis();
    }

    if(currentOK == HIGH && lastBtnOK == LOW) {
        unsigned long pressDuration = millis() - okPressStart;

        if (pressDuration >= longPressTime) {
            openSoftwareMenu();
        } else {
            if (currentScreen == SOFTWARE_MENU || currentScreen == ENCODER_MENU || currentScreen == JOYSTICK_MENU || currentScreen == BUTTON_CONFIG_MENU || currentScreen == ERROR_SCREEN) {
                selectCurrentMenuItem();
            }
        }
        delay(120);
    }

   if (currentUp == LOW && lastBtnUp == HIGH) {
    upPressStart = millis();
   }
   if (currentUp == HIGH && lastBtnUp == LOW) {
       unsigned long pressDuration = millis() - upPressStart;
       if (pressDuration >= longPressTime) {
        openButtonConfigMenu(0);
       } else {
        if (currentScreen == HOME_SCREEN) performDirectionalAction(0);
        else moveMenuUp();
       }
       delay(120);
    }

    if (currentDown == LOW && lastBtnDown == HIGH) {
        downPressStart = millis();
    }
    if (currentDown == HIGH && lastBtnDown == LOW) {
        unsigned long pressDuration = millis() - downPressStart;
        if (pressDuration >= longPressTime) {
         openButtonConfigMenu(1);
        } else {
         if (currentScreen == HOME_SCREEN) performDirectionalAction(1);
         else moveMenuDown();
        }
        delay(120);
    }

    if (currentLeft == LOW && lastBtnDown == HIGH) {
        leftPressStart = millis();
    }
    if (currentLeft == HIGH && lastBtnLeft == LOW) {
        unsigned long pressDuration = millis() - leftPressStart;
        if (pressDuration >= longPressTime) {
         openButtonConfigMenu(2);
        } else {
         if (currentScreen == HOME_SCREEN) performDirectionalAction(2);
         else goHome();
        }
        delay(120);
    }

    if (currentRight == LOW && lastBtnDown == HIGH) {
        rightPressStart = millis();
    }
    if (currentRight == HIGH && lastBtnRight == LOW) {
        unsigned long pressDuration = millis() - rightPressStart;
        if (pressDuration >= longPressTime) {
         openButtonConfigMenu(3);
        } else {
         if (currentScreen == HOME_SCREEN) performDirectionalAction(3);
         else moveMenuDown();
        }
        delay(120);
    }

    if (currentEncoderSW == LOW && lastEncSW == HIGH) {
        if (currentScreen == HOME_SCREEN) {
            openEncoderMenu();
        } else {
            selectCurrentMenuItem();
        }
        delay(120);
    }

    if (currentJoySW == LOW && lastJoySW == HIGH) {
        if (currentScreen == HOME_SCREEN) {
            openJoystickMenu();
        } else {
            selectCurrentMenuItem();
        }
        delay(120);
    }

    lastBtnOK = currentOK;
    lastBtnUp = currentUp;
    lastBtnDown = currentDown;
    lastBtnLeft = currentLeft;
    lastBtnRight = currentRight;
    lastEncSW = currentEncoderSW;
    lastJoySW = currentJoySW;

    handleEncoderRotation();
    handleJoystick();

    refreshDisplay();
}
