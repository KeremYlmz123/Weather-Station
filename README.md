# Arduino & Proteus Weather Station Simulation

Bu proje, **Arduino UNO** mikrodenetleyicisi kullanılarak geliştirilmiş, çok fonksiyonlu bir meteoroloji istasyonunun **Proteus** üzerindeki tam kapsamlı simülasyonudur. Sistem; sıcaklık, nem, ışık yoğunluğu ve su seviyesi verilerini ölçer, gerçek zamanlı saati (RTC) takip eder ve tüm bu verileri **I2C LCD** ekran üzerinde kullanıcıya sunar. Kullanıcı arayüzü, **IR (Kızılötesi) Uzaktan Kumanda** ile yönetilmektedir.

## Proje Özellikleri

- ** Gerçek Zamanlı Saat (RTC):** DS1302 modülü entegrasyonu ile tarih ve saat bilgisi (Yıl-Ay-Gün Saat:Dakika:Saniye) hassas bir şekilde gösterilir.
- ** Çevresel İzleme:** DHT11 sensörü kullanılarak ortam sıcaklığı (°C) ve nem oranı (%) ölçülür.
- ** Işık Seviyesi Tespiti:** LDR sensörü ve gerilim bölücü devre ile ortam aydınlığı yüzdesel (%0-100) olarak hesaplanır.
- ** Su Seviyesi Kontrolü:** Potansiyometre simülasyonu ile su seviyesi (mm cinsinden) takip edilir.
- ** Uzaktan Kontrol:** Sistemdeki modlar arasında geçiş yapmak için IR Kumanda kullanılır.
- ** Otomatik Sunum Modu:** Tüm sensör verilerini belirli aralıklarla ekranda döndüren eller serbest (hands-free) modu bulunur.
- ** I2C Protokolü:** LCD ekran kontrolü için sadece 2 pin (SDA/SCL) kullanılarak pin tasarrufu sağlanmıştır.

## Donanım Gereksinimleri (Proteus Bileşenleri)

Simülasyon aşağıdaki sanal bileşenler üzerine kurulmuştur:

- **Mikrodenetleyici:** Arduino UNO R3
- **Görüntüleme:** LM016L (16x2 LCD) + PCF8574 (I2C Genişletici)
- **Sensörler:**
  - `DHT11` (Sıcaklık ve Nem)
  - `LDR` (Torch LDR - Işık Sensörü)
  - `POT-HG` (Su Seviyesi Simülasyonu için)
- **Zamanlayıcı:** DS1302 RTC Modülü
- **Giriş:** IR Receiver (IRLINK) ve TV Remote (NEC Protokolü)
- **Pasif Bileşenler:** 10k Dirençler (I2C Pull-Up ve LDR devresi için)

## Gerekli Arduino Kütüphaneleri

Projenin derlenebilmesi için Arduino IDE Library Manager üzerinden aşağıdaki kütüphanelerin yüklenmesi gerekmektedir:

1.  **LiquidCrystal I2C** (by Frank de Brabander)
2.  **DHT sensor library** (by Adafruit)
3.  **Adafruit Unified Sensor** (by Adafruit)
4.  **Rtc by Makuna** (DS1302 için)
5.  **IRremote**
6.  **ThreeWire**

## Pin Bağlantı Şeması

| Bileşen           | Arduino Pini | Açıklama                                 |
| :---------------- | :----------- | :--------------------------------------- |
| **I2C LCD (SDA)** | A4           | 10k Pull-Up direnci ile +5V'a bağlanmalı |
| **I2C LCD (SCL)** | A5           | 10k Pull-Up direnci ile +5V'a bağlanmalı |
| **IR Alıcı**      | D3           | Dijital Giriş                            |
| **DHT11 Data**    | D4           | Dijital Giriş                            |
| **DS1302 RST**    | D5           | -                                        |
| **DS1302 DAT**    | D6           | -                                        |
| **DS1302 CLK**    | D7           | -                                        |
| **Su Sensörü**    | A0           | Analog Giriş (0-1023)                    |
| **LDR (Işık)**    | A3           | Analog Giriş (0-1023)                    |

## Kullanım Kılavuzu (Kumanda Kodları)

Simülasyon çalıştırıldığında sistem hazır modunda açılır. Kumanda üzerindeki tuşların fonksiyonları şöyledir:

- **`0` Tuşu:** Ana Menü / Bekleme Ekranı
- **`1` Tuşu:** Tarih ve Saat Modu
- **`2` Tuşu:** Sıcaklık ve Nem Modu
- **`3` Tuşu:** Işık Seviyesi Modu
- **`4` Tuşu:** Su Seviyesi Modu
- **`5` Tuşu:** Otomatik Sunum Modu (3 saniyede bir ekran değişir)

## Kurulum ve Çalıştırma

1.  Bu repoyu bilgisayarınıza indirin (`git clone` veya ZIP olarak).
2.  `sketch_weather_station.ino` dosyasını Arduino IDE ile açın.
3.  Yukarıda listelenen kütüphanelerin yüklü olduğundan emin olun.
4.  Kodu derleyin (**Verify** butonu).
5.  Derleme çıktısındaki `.hex` dosyasının yolunu kopyalayın.
6.  Proteus projesini (`.pdsprj`) açın.
7.  Arduino UNO üzerine çift tıklayın ve **Program File** kısmına kopyaladığınız `.hex` yolunu yapıştırın.
8.  Simülasyonu başlatın (**Play** butonu).

## Önemli Notlar (Troubleshooting)

- **LCD Çalışmıyorsa:** Proteus simülasyonunda I2C hatlarının (SDA ve SCL) çalışması için harici **Pull-Up dirençleri (10k)** bağlanmalıdır.
- **I2C Adresi:** Kod içerisinde LCD adresi `0x27` olarak tanımlanmıştır. PCF8574 entegresinin A0, A1 ve A2 bacaklarının **High (+5V)** konumunda olduğundan emin olun.
- **DS1302:** Kristal frekansının özelliklerden `32.768kHz` olarak ayarlandığını kontrol edin.

---

**Lisans:** [MIT License](LICENSE)
