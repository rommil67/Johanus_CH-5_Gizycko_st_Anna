#include <U8g2lib.h>
#include <Wire.h>

// SH1106 128x64 I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  u8g2.begin();
}

void loop() {
  for(int i = -5; i <= 4; i++) {
    u8g2.clearBuffer();

    // --- Mały napis u góry ---
    u8g2.setFont(u8g2_font_6x12_tr);   // mała czcionka
    const char* header = "Transpose:";
    int16_t headerWidth = u8g2.getStrWidth(header);
    int16_t headerX = (128 - headerWidth) / 2;  // wycentrowanie poziome
    u8g2.drawStr(headerX, 14, header);          // y=14, trochę niżej od góry

    // --- Duża liczba wyśrodkowana, trochę niżej ---
    u8g2.setFont(u8g2_font_fub42_tr);          // duża czcionka
    String text = (i >= 0 ? "+" : "") + String(i);
    int16_t strWidth = u8g2.getStrWidth(text.c_str());
    int16_t x = (128 - strWidth) / 2;
    int16_t y = 40 + 21;  // obniżone w pionie, żeby nie nachodziło na nagłówek
    u8g2.drawStr(x, y, text.c_str());

    u8g2.sendBuffer();
    delay(1000);
  }
}
