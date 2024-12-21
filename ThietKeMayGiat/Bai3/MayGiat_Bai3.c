
#include <io.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <stdio.h>
#include <alcd.h>

// Bi?n to?n c?c
int curSpeed = 0;
char displayText[32];  // Bi?n luu chu?i hi?n th?

// H?m di?u ch?nh t?c d? d?ng co
void adjustSpeed(int speed) {
    OCR0 = speed;  // T?c d? d?ng co t? l? ngh?ch v?i gi? tr? OCR0
}

// H?m c?p nh?t v? hi?n th? t?c d?
void updateSpeedText() {
    sprintf(displayText, "Toc do: %d%%", (curSpeed * 100) / 255);
    glcd_outtextxy(10, 40, displayText);
    lcd_gotoxy(0,1);
    lcd_puts(displayText);
}

// H?m hi?n th? tr?ng th?i "V?t t?c d? cao"
void displayHighSpeed() {
    glcd_clear();
    lcd_clear();
    lcd_putsf("Vat toc do cao");
    glcd_outtextxy(10, 20, "Vat toc do cao");
    updateSpeedText();
}

// H?m x? l? ch? d? v?t
void vat() {
    // Hi?n th? tr?ng th?i "V?t t?c d? cao"
    curSpeed = 255;  // T?c d? t?i da
    adjustSpeed(curSpeed);
    displayHighSpeed();

    // Duy tr? t?c d? v?t trong 3 gi?y
    delay_ms(3000);

    // K?t th?c ch? d? v?t, hi?n th? th?ng b?o
    curSpeed = 0;
    adjustSpeed(0);
    glcd_clear();
    glcd_outtextxy(10, 30, "Hoan thanh chu trinh giat");
    lcd_clear();
    lcd_putsf("Hoan thanh chu trinh giat");

    // Gi? th?ng b?o trong 2 gi?y
    delay_ms(2000);
}

// H?m x? l? ch? d? gi?t
void giat() {
    int second = 0;

    // Hi?n th? tr?ng th?i gi?t
    glcd_clear();
    glcd_outtextxy(10, 20, "Dang giat...");
    lcd_clear();
    lcd_putsf("Dang giat");
    // Ch?y d?ng co gi?t trong 10 gi?y
    for (second = 0; second < 10; second++) {
        curSpeed = (second + 1) * 25;  // Tang t?c d? d?n m?i gi?y
        if (curSpeed > 255) curSpeed = 255;
        adjustSpeed(curSpeed);
        updateSpeedText();
        delay_ms(1000);
    }

    // D?ng d?ng co
    curSpeed = 0;
    adjustSpeed(0);

    // Chuy?n sang ch? d? v?t
    vat();
}

void main(void) {
    // Kh?i t?o GLCD
    GLCDINIT_t glcd_init_data;
    glcd_init_data.font = font5x7;
    glcd_init_data.temp_coef = 140;
    glcd_init_data.bias = 4;
    glcd_init_data.vlcd = 66;
    glcd_init(&glcd_init_data);
    lcd_init(16);
    // K?o l?n c?c n?t nh?n
    DDRB.2 = 0;  // N?t BT1
    PORTB.2 = 1;

    DDRB.3 = 0;  // N?t BT2
    PORTB.3 = 1;

    // Kh?i t?o d?ng co
    DDRB.4 = 1;  // PWM d?ng co
    TCCR0 = (1 << WGM00) | (1 << COM01) | (1 << COM00) | (0 << WGM01) | (1 << CS02);
    TCNT0 = 0x00;
    adjustSpeed(0);

    // Hi?n th? menu ban d?u
    lcd_clear();
    glcd_clear();

    lcd_putsf("May giat");
    lcd_gotoxy(0,1);
    lcd_putsf("BT1. GIAT");
    lcd_putsf("BT2. VAT");
    glcd_outtextxy(10, 10, "MAY GIAT");
    glcd_outtextxy(10, 20, "BT1. GIAT");
    glcd_outtextxy(10, 30, "BT2. VAT");

    // V?ng l?p ch?nh
    while (1) {
        if (PINB.2 == 0) {
            giat();  // Ch? d? gi?t
        }
        if (PINB.3 == 0) {
            vat();   // Ch? d? v?t
        }
        delay_ms(250);  // Tr?nh nhi?u t?n hi?u
    }
}
