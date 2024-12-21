
#include <io.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <stdio.h>
#include <alcd.h>

bool led = false;
int counter = 0;

interrupt [TIM1_OVF] // Interrupt cua timer1

void timer1_ovf_isr(void) {
    // Reinitialize Timer1 value
    TCNT1H = 0xE0C0 >> 8;
    TCNT1L = 0xE0C0 & 0xff;

    // ?i?u khi?n LED nh?p nh?y
    if (led) {
        if (counter == 2000)
            counter = 0;

        if (counter == 0) {
            PORTD
            .4 = 0;
            PORTD
            .5 = 1;
        }

        if (counter == 1000) {
            PORTD
            .5 = 0;
            PORTD
            .4 = 1;
        }
        counter++;
    }
}


void xa_nuoc() {
    lcd_clear();
    glcd_clear();
    lcd_putsf("Bat dau xa nuoc");
    glcd_outtextxy(10, 10, "Bat dau xa nuoc");
    delay_ms(2000);
}

// H?m x? l? ch? d? v?t
void vat() {
    lcd_clear();
    glcd_clear();
    lcd_putsf("Bat dau vat");
    glcd_outtextxy(10, 10, "Bat dau vat");
    delay_ms(2000);
    lcd_clear();
    glcd_clear();
//    xa_nuoc();
    lcd_putsf("Hoan thanh");
    glcd_outtextxy(10, 10, "Hoan thanh");
}

// H?m x? l? ch? d? gi?t
void giat() {

    // B?t role 1 trong 2 gi?y
    PORTC
    .3 = 1;
    lcd_clear();
    glcd_clear();
    glcd_outtextxy(10, 10, "Cap nuoc");
    lcd_putsf("Cap nuoc");

    delay_ms(2000);

    glcd_clear();
    lcd_clear();

    PORTC
    .3 = 0;

    lcd_putsf("Bat dau giat");
    glcd_outtextxy(10, 10, "Bat dau giat");
    delay_ms(2000);
    lcd_clear();
    glcd_clear();
    delay_ms(50);

    xa_nuoc();
    vat();


}

void init_glcd(int bias, int vlcd) { //???
    GLCDINIT_t glcd_init_data;
    glcd_init_data.font = font5x7;
    glcd_init_data.temp_coef = 140;
    glcd_init_data.bias = bias;
    glcd_init_data.vlcd = vlcd;
    glcd_init(&glcd_init_data);
}

void main(void) {
    // Kh?i t?o GLCD
    init_glcd(4, 66);
    lcd_init(16);
    // K?o l?n BT1
    DDRB
    .2 = 0;
    PORTB
    .2 = 1;

    // K?o l?n BT2
    DDRB
    .3 = 0;
    PORTB
    .3 = 1;

    // Role 1 v? 2
    DDRC
    .3 = 1;
    DDRC
    .2 = 1;

    // LED
    DDRD
    .5 = 1;
    DDRD
    .4 = 1;

    // Kh?i t?o d?ng co v? timer
    DDRB
    .4 = 1;
    DDRD
    .7 = 1;

    ASSR = 0 << AS0;
    TCCR0 = (1 << WGM00) | (1 << COM01) | (1 << COM00) | (0 << WGM01) | (1 << CS02) | (0 << CS01) | (0 << CS00);
    TCNT0 = 0x00;
    TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (0 << COM1C1) | (0 << COM1C0) |
             (0 << WGM11) | (0 << WGM10);
    TCCR1B = (0 << ICNC1) | (0 << ICES1) | (0 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
    TCNT1H = 0xE0;
    TCNT1L = 0xC0;

    // Kh?i t?o ng?t cho timer1
    TIMSK = (0 << OCIE2) | (0 << TOIE2) | (0 << TICIE1) | (0 << OCIE1A) | (0 << OCIE1B) | (1 << TOIE1) | (0 << OCIE0) |
            (0 << TOIE0);
    ETIMSK = (0 << TICIE3) | (0 << OCIE3A) | (0 << OCIE3B) | (0 << TOIE3) | (0 << OCIE3C) | (0 << OCIE1C);
#asm("sei")

    // Hi?n th? menu ban d?u
    glcd_outtextxy(10, 10, "MAY GIAT");
    glcd_outtextxy(10, 20, "BT1. GIAT");
    glcd_outtextxy(10, 30, "BT2. VAT");

//    lcd_putsf("MAY GIAT");
//    lcd_gotoxy(0, 1);
//
    lcd_putsf("BT1.GIAT");
    lcd_gotoxy(0, 1);
    lcd_putsf("BT2.VAT");

    while (1) {
        if (PINB.2 == 0)
        giat();
        if (PINB.3 == 0)
        vat();
        delay_ms(250);
    }
}
