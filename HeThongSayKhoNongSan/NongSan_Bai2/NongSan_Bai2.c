/*
 * NongSan_LCD.c
 *
 * Created: 12/21/2024 9:46:40 PM
 * Author: buian
 */

#include <io.h>
#include <alcd.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <dht11.c>


char x = 0;
// Nhiet do va do am
unsigned char nhietdo = 0, doam = 0;

// nhiet do, do am dat yeu cau: nhietdo_chuan, doam_chuan
unsigned char nhietdo_chuan = 28; // nhiet do chuan tat may say kho
unsigned char doam_chuan = 53; // doam chuan tat may say kho
int speed = 255 / 10; // toc do cua dong co 


bool say_xong = false;
int counter = 0;

void init_register() {
    // ban phim ma tran
//    DDRF = 0b11101010;
    // khoi tao ban phim ma tran
    DDRF  = 0xEA;
    PORTF  = 0x15;
    // 2 led
    DDRD.4 = 1;
    DDRD.5 = 1;
    // lcd
    DDRD.7 = 1;
    DDRB.4 = 1;
    // BT 1, 2
    PORTB.2 = 1; // BT1
    PORTB.3 = 1; // BT2
    PORTB.0 = 1; // BT3
    PORTD.7 = 1;
    // servo
    DDRC.7 = 1;
}


void init_glcd(int bias, int vlcd) { //???
    GLCDINIT_t glcd_init_data;
    glcd_init_data.font=font5x7;
    glcd_init_data.temp_coef=140 ;
    glcd_init_data.bias=bias;
    glcd_init_data.vlcd=vlcd;
    glcd_init(&glcd_init_data);
}

void adjustSpeed(int speed) {
    OCR0 = speed;
}

void quay_DC() {
    adjustSpeed(120);
}

void dung_DC() {
    adjustSpeed(0);
}

void init_timer() {
    // khoi tao timer 0 cho dong co

    ASSR=0<<AS0;
    TCCR0=(1<<WGM00) | (1<<COM01) | (1<<COM00) | (0<<WGM01) | (1<<CS02) | (0<<CS01) | (0<<CS00);
    TCNT0=0x00;
    adjustSpeed(0);

    // khoi tao timer 1 voi chu ki 1 ms
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
    TCNT1H=0xE0;
    TCNT1L=0xC0;

    // khoi tao interrupt cho timer1
    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (1<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);
    ETIMSK=(0<<TICIE3) | (0<<OCIE3A) | (0<<OCIE3B) | (0<<TOIE3) | (0<<OCIE3C) | (0<<OCIE1C);
#asm("sei")
}



// Interrupt cua timer1
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    // Reinitialize Timer1 value
    TCNT1H=0xE0C0 >> 8;
    TCNT1L=0xE0C0 & 0xff;
    // Place your code here
    if (say_xong) {
        if (counter == 2000)
            counter  = 0;

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


void hien_thi_LCD() {
    lcd_gotoxy(0, 0);
    lcd_putsf("TEMP:");
    lcd_putchar(nhietdo / 10 + 48);
    lcd_putchar(nhietdo % 10 + 48);
    lcd_gotoxy(0, 1);
    lcd_putsf("HUMI:");
    lcd_putchar(doam / 10 + 48);
    lcd_putchar(doam % 10 + 48);
}
//
void hien_thi_GLCD() {
    glcd_outtextxy(0, 0, "TEMP:");
    glcd_putchar(nhietdo / 10 + 48);
    glcd_putchar(nhietdo % 10 + 48);
    glcd_outtextxy(0, 10, "HUMI:");
    glcd_putchar(doam / 10 + 48);
    glcd_putchar(doam % 10 + 48);
}
void main(void)
{
    init_glcd(4, 66);
    lcd_init(16);
    init_register();

    // khoi tao ban phim ma tran
    DDRF  = 0xEA;
    PORTF  = 0x15;

    // some port
    DDRB.4 = 1;
    DDRD.7 = 1;
    DDRD.4 = 1;
    DDRD.5 = 1;

    //pull up BT1
    DDRB.2 = 0;
    PORTB.2 = 1;
    //pull up BT2
    DDRB.3 = 0;
    PORTB.3 = 1;
    //pull up BT3
    DDRB.0 = 0;
    PORTB.0 = 1;

    init_timer();

    while (1)
    {
        adjustSpeed(speed);
        delay_ms(1000); 
        if (speed >= 255 ) {
            delay_ms(2000); // Ch? 2 gi?y khi t?c d? l?n 100%
            adjustSpeed(0); // D?ng d?ng co
            say_xong = true;
            
            lcd_clear();
            lcd_gotoxy(0, 0);
            DDRB.4 = 0;
            lcd_putsf("DAT YEU CAU!");
            glcd_clear();
            glcd_outtextxy(10, 10, "DAT YEU CAU!");
            break; // Thoat vong lap
        }
        speed = speed + speed / 5;
    }


}


