// ==========================================
// ARDUINO WEATHER STATION - FINAL (KUMANDA KODLU)
// ==========================================

#include <IRremote.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <DHT.h>       
#include <LiquidCrystal_I2C.h> 
#include <Wire.h> 

// === EKRAN ===
// PCF8574 A0-A1-A2 Power'a bağlıysa adres 0x27'dir.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === DHT11 SENSÖR ===
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === SENSÖRLER ===
const int waterSensor = A0;
const int sensorPin = A3; // LDR
int waterValue = 0, mmwaterValue = 0;
int luce = 0, pluce = 0;

// === IR ALICI ===
const int irReceiverPin = 3;
IRrecv irrecv(irReceiverPin);
decode_results results;

// === SAAT (RTC) ===
// Proteus: RST=5, DAT=6, CLK=7
ThreeWire myWire(6, 7, 5); 
RtcDS1302<ThreeWire> Rtc(myWire);

// ==========================================================
// 🔥 GÜNCELLENMİŞ KUMANDA KODLARI 🔥
// ==========================================================
unsigned long IR_0 = 0xFF4AB5;  // 0 Tuşu -> Ana Menü
unsigned long IR_1 = 0xFF6897;  // 1 Tuşu -> Saat
unsigned long IR_2 = 0xFF9867;  // 2 Tuşu -> Sıcaklık/Nem
unsigned long IR_3 = 0xFFB04F;  // 3 Tuşu -> Işık Seviyesi
unsigned long IR_4 = 0xFF30CF;  // 4 Tuşu -> Su Seviyesi
unsigned long IR_5 = 0xFF18E7;  // 5 Tuşu -> Sunum Modu

unsigned long lastIrCode = 0;

// Modlar
enum Mode { MODE_HOME=0, MODE_TIME, MODE_TEMP_HUM, MODE_DARKNESS, MODE_WATER, MODE_PRESENTATION };
Mode currentMode = MODE_HOME;

unsigned long lastUiMs = 0;
const unsigned long UI_REFRESH_MS = 500; // Ekran yenileme hızı
unsigned long lastDhtMs = 0;
const unsigned long DHT_REFRESH_MS = 2000;
float dhtTempC = 0, dhtHum = 0;

// Sunum modu değişkenleri
unsigned long presNextMs = 0;
uint8_t presStep = 0;

// --- FONKSİYONLAR ---

// Serial üzerinden saati ayarlamak için: SET 2025 12 21 23 55 00
void handleRtcSetFromSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (!line.startsWith("SET ")) return;
  int yr = line.substring(4, 8).toInt();
  int mon = line.substring(9, 11).toInt();
  int date = line.substring(12, 14).toInt();
  int hr = line.substring(15, 17).toInt();
  int min = line.substring(18, 20).toInt();
  int sec = line.substring(21, 23).toInt();
  RtcDateTime newTime(yr, mon, date, hr, min, sec);
  Rtc.SetDateTime(newTime);
}

// Mod değiştiğinde ekranı temizle
void setMode(Mode m) {
  currentMode = m;
  lcd.clear();
  lastUiMs = 0; 
  presNextMs = 0;
  presStep = 0;
}

// 0: ANA EKRAN
void renderHome() {
  lcd.setCursor(0, 0); lcd.print("Weather Station ");
  lcd.setCursor(0, 1); lcd.print("Ready to Start..");
}

// 1: SAAT EKRANI
void print_time() {
  RtcDateTime now = Rtc.GetDateTime();
  lcd.setCursor(0, 0);
  lcd.print(now.Year()); lcd.print("-");
  if(now.Month()<10) lcd.print("0"); lcd.print(now.Month()); lcd.print("-");
  if(now.Day()<10) lcd.print("0"); lcd.print(now.Day());
  
  lcd.setCursor(0, 1);
  if(now.Hour()<10) lcd.print("0"); lcd.print(now.Hour()); lcd.print(":");
  if(now.Minute()<10) lcd.print("0"); lcd.print(now.Minute()); lcd.print(":");
  if(now.Second()<10) lcd.print("0"); lcd.print(now.Second());
}

// 2: SICAKLIK VE NEM
void renderTempHum(unsigned long now) {
  if (now - lastDhtMs >= DHT_REFRESH_MS || lastDhtMs == 0) {
    lastDhtMs = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
      dhtHum = h;
      dhtTempC = t;
    }
  }
  lcd.setCursor(0, 0); lcd.print("Temp: "); lcd.print(dhtTempC, 1); lcd.print("C");
  lcd.setCursor(0, 1); lcd.print("Hum : "); lcd.print(dhtHum, 1); lcd.print("%");
}

// 3: IŞIK SEVİYESİ (LDR)
void renderDarkness() {
  luce = analogRead(sensorPin);
  pluce = map(luce, 0, 1023, 0, 100); 
  lcd.setCursor(0, 0); lcd.print("Light Level:    ");
  lcd.setCursor(0, 1); lcd.print(pluce); lcd.print("%           ");
}

// 4: SU SEVİYESİ
void renderWater() {
  waterValue = analogRead(waterSensor);
  mmwaterValue = map(waterValue, 0, 1023, 0, 40); 
  lcd.setCursor(0, 0); lcd.print("Water Lvl (mm): ");
  lcd.setCursor(0, 1); lcd.print(mmwaterValue); lcd.print("            ");
}

// 5: SUNUM (OTOMATİK DÖNGÜ)
void renderPresentation(unsigned long now) {
  if (presNextMs == 0 || now >= presNextMs) {
    presNextMs = now + 3000; // 3 saniyede bir değiş
    presStep = (presStep + 1) % 4;
    lcd.clear();
  }
  if (presStep == 0) print_time();
  else if (presStep == 1) renderTempHum(now);
  else if (presStep == 2) renderDarkness();
  else if (presStep == 3) renderWater();
}

void setup() {
  Serial.begin(9600);
  Rtc.Begin();
  dht.begin();
  irrecv.enableIRIn();
  lcd.init();
  lcd.backlight();
  pinMode(sensorPin, INPUT);
  
  lcd.setCursor(0, 0);
  lcd.print("System Loading..");
  delay(1000);
  setMode(MODE_HOME);
}

void loop() {
  handleRtcSetFromSerial();
  unsigned long now = millis();

  // IR Sinyal Yakalama
  if (irrecv.decode(&results)) {
    unsigned long code = results.value;
    
    // Eğer tuş basılı tutulursa (FFFFFFFF) eski kodu kullan
    if (code == 0xFFFFFFFF) code = lastIrCode;
    else lastIrCode = code;
    
    irrecv.resume(); // Sıradaki sinyal için dinlemeye devam et

    // --- MOD DEĞİŞTİRME MANTIĞI ---
    if (code == IR_1) setMode(MODE_TIME);
    else if (code == IR_2) setMode(MODE_TEMP_HUM);
    else if (code == IR_3) setMode(MODE_DARKNESS);
    else if (code == IR_4) setMode(MODE_WATER);
    else if (code == IR_5) setMode(MODE_PRESENTATION);
    else if (code == IR_0) setMode(MODE_HOME);
  }

  // Seçili Modu Ekranda Göster (Sürekli Güncelle)
  if (now - lastUiMs >= UI_REFRESH_MS) {
    lastUiMs = now;
    if (currentMode == MODE_HOME) renderHome();
    else if (currentMode == MODE_TIME) print_time();
    else if (currentMode == MODE_TEMP_HUM) renderTempHum(now);
    else if (currentMode == MODE_DARKNESS) renderDarkness();
    else if (currentMode == MODE_WATER) renderWater();
    else if (currentMode == MODE_PRESENTATION) renderPresentation(now);
  }
  
  delay(10);
}