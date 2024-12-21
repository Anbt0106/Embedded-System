/*
 * may_giat.c
 *
 * Created: 12/28/2022 2:23:27 AM
 * Author: Who
 */

#include <io.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <stdio.h>
#include <alcd.h>

int curSpeed = 0;
bool led = false;
int counter = 0;
char speedText[16];

// Interrupt cua timer1
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    // Reinitialize Timer1 value
    TCNT1H = 0xE0C0 >> 8;
    TCNT1L = 0xE0C0 & 0xff;

    // Dieu khien LED nhap nhay
    if (led) {
        if (counter == 2000)
            counter = 0;

        if (counter == 0) {
            PORTD.4 = 0;
            PORTD.5 = 1;
        }

        if (counter == 1000) {
            PORTD.5 = 0;
            PORTD.4 = 1;
        }
        counter++;
    }
}

// Ham dieu chinh toc do dong co
void adjustSpeed(int speed) {
    OCR0 = speed;
}

// Ham cap nhat toc do dong co
void speedUpdater(int option) {
    int newSpeed;
    if (curSpeed == 0) {
        curSpeed = 70;
    } else {
        if (option == 20)
            newSpeed = curSpeed + curSpeed / 5;
        else if (option == 100)
            newSpeed = curSpeed * 2;

        if (newSpeed > 255)
            curSpeed = 255;
        else
            curSpeed = newSpeed;
    }
    adjustSpeed(curSpeed);
}

// Ham xu ly che do vat
void vat() {
    int second = 0;

    // Bat role 2 trong 2 giay
    PORTC.2 = 1;
    delay_ms(2000);
    PORTC.2 = 0;

    // Quay dong co cho toi da trong 2 giay
    while (second < 2) {
        lcd_clear();
        lcd_putsf("Dang vat");
        lcd_gotoxy(0, 1);
        speedUpdater(100);

        // Hien thi toc do hien tai len GLCD
        sprintf(speedText, "Toc do: %d", curSpeed);
        glcd_outtextxy(10, 40, speedText);

        lcd_puts(speedText);

        second++;
        delay_ms(1000);
    }

    // Dung dong co sau 2 giay va reset toc do
    delay_ms(2000);
    curSpeed = 0;
    adjustSpeed(0);
    led = false;

    // Quay ve trang thai ban dau
    lcd_clear();
    lcd_putsf("Hoan thanh");
    glcd_outtextxy(10, 10, "Hoan thanh");
    delay_ms(2000);
}

// Ham xu ly che do giat
void giat() {
    int second = 0;

    // B?t role 1 trong 2 gi?y
    PORTC.3 = 1;
    lcd_clear();
    glcd_clear();
    lcd_putsf("Cap nuoc...");
    glcd_outtextxy(10, 10, "Cap nuoc...");
    delay_ms(2000);
    PORTC.3 = 0;

    // Quay d?ng co trong 10 gi?y
    lcd_clear();
    glcd_clear();
    lcd_putsf("Dang giat...");
    glcd_outtextxy(10, 10, "Dang giat...");

    while (second < 10) {
        speedUpdater(20);

        // Hi?n th? t?c d? hi?n t?i l?n GLCD v? LCD
        sprintf(speedText, "Toc do: %d", curSpeed);
        glcd_outtextxy(10, 40, speedText);
        lcd_gotoxy(0, 1);
        lcd_puts(speedText);

        second++;
        delay_ms(1000);
    }

    // Dung dong co sau khi hoan thanh
    delay_ms(2000);
    curSpeed = 0;
    adjustSpeed(0);

    lcd_clear();
    glcd_clear();
    lcd_putsf("Hoan thanh");
    glcd_outtextxy(10, 10, "Hoan thanh");
    delay_ms(2000);
}

void main(void)
{
    // Khoi tao GLCD
    GLCDINIT_t glcd_init_data;
    glcd_init_data.font = font5x7;
    glcd_init_data.temp_coef = 140;
    glcd_init_data.bias = 4;
    glcd_init_data.vlcd = 66;
    glcd_init(&glcd_init_data);
    lcd_init(16);

    // Keo len BT1
    DDRB.2 = 0;
    PORTB.2 = 1;

    // Keo len BT2
    DDRB.3 = 0;
    PORTB.3 = 1;

    // Role 1 va 2
    DDRC.3 = 1;
    DDRC.2 = 1;

    // LED
    DDRD.5 = 1;
    DDRD.4 = 1;

    // Khoi tao dong co va timer
    DDRB.4 = 1;
    DDRD.7 = 1;
    ASSR = 0 << AS0;
    TCCR0 = (1 << WGM00) | (1 << COM01) | (1 << COM00) | (0 << WGM01) | (1 << CS02) | (0 << CS01) | (0 << CS00);
    TCNT0 = 0x00;
    adjustSpeed(0);
    TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << COM1C1) | (0 << COM1C0) | (0 << WGM11) | (0 << WGM10);
    TCCR1B = (0 << ICNC1) | (0 << ICES1) | (0 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
    TCNT1H = 0xE0;
    TCNT1L = 0xC0;

    // Khoi tao ngat cho timer1
    TIMSK = (0 << OCIE2) | (0 << TOIE2) | (0 << TICIE1) | (0 << OCIE1A) | (0 << OCIE1B) | (1 << TOIE1) | (0 << OCIE0) | (0 << TOIE0);
    ETIMSK = (0 << TICIE3) | (0 << OCIE3A) | (0 << OCIE3B) | (0 << TOIE3) | (0 << OCIE3C) | (0 << OCIE1C);
#asm("sei")

    // Hien thi menu ban dau
    glcd_outtextxy(10, 10, "MAY GIAT");
    glcd_outtextxy(10, 20, "BT1. GIAT");
    //glcd_outtextxy(10, 30, "BT2. VAT");

    lcd_putsf("MAY GIAT");
    lcd_gotoxy(0, 1);
    lcd_putsf("1. GIAT");
    //lcd_putsf("2. VAT");

    while (1) {
        if (PINB.2 == 0)
        giat();
        //if (PINB.3 == 0)
        //vat();
        delay_ms(250);
    }
}
