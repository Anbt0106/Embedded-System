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

#define PIN_DHT PINB.7
#define PORT_DHT PORTB.7
#define DDR_DHT DDRB.7

char x = 0;
unsigned char nhietdo = 0, doam = 0;
unsigned char nhietdo_naodo = 28, nhietdo_naodo2 = 30, doam_naodo = 53;
bool say_xong = false;
int counter = 0;

int keypad[3][3] = {1,2,3,4,5,6,7,8,9};

int checksum = 0;

char dht11(unsigned char *nhietdo, unsigned char *doam) {
    int i, j;
    int buffer[5] = {0, 0, 0, 0, 0};
    // buoc 1
    DDR_DHT = 1;
    PORT_DHT = 1;
    delay_us(60);
    // buoc 2
    PORT_DHT = 0;
    delay_ms(25);
    // buoc 3
    DDR_DHT = 0;
    PORT_DHT = 1;

    // check pin
    delay_us(60);
    if (PIN_DHT) return 0;
    else while (!(PIN_DHT));
    delay_us(60);
    if (!PIN_DHT) return 0;
    else while ((PIN_DHT));


    for (i = 0; i < 5; ++i) {
        for (j = 0; j < 8; ++j) {
            while (!(PIN_DHT));
            delay_us(50);
            if (PIN_DHT) {
                buffer[i] |= (1 << (7 - j));
                while ((PIN_DHT));
            }
        }
    }
    checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];

    if ((checksum) != buffer[4]) return 0;

    *nhietdo = buffer[2];
    *doam = buffer[0];
    return 1;
}


void init_register() {
    // ban phim ma tran
    DDRF = 0b11101010;
    // 2 led
    DDRD.4 = 1;
    DDRD.5 = 1;
    // lcd
    DDRD.7 = 1;
    DDRB.4 = 1;
    // BT 1, 2
    PORTB.2 = 1;
    PORTB.3 = 1;
    PORTD.7 = 1;
    // servo
    DDRC.7 = 1;
}

int read() {
    char a, i, j;
    for(j = 0; j < 3; j++ ){
        if (j == 0) PORTF = 0b11111101;
        if (j == 1) PORTF = 0b11110111;
        if (j == 2) PORTF = 0b11011111;
        for(i = 0; i < 3; i++){
            if (i == 0){
                a = PINF&0x04;
                if(a != 0x04) {
                    init_register();
                    return keypad[i][j];
                }
            }
            if (i == 1){
                a = PINF&0x10;
                if (a != 0x10) {
                    init_register();
                    return keypad[i][j];
                }
            }
            if (i == 2){
                a = PINF&0x01;
                if (a != 0x01) {
                    init_register();
                    return keypad[i][j];
                }
            }
        }
    }
    init_register();
    return -1;
}


void init_glcd(int bias, int vlcd) {
    GLCDINIT_t glcd_init_data;
    glcd_init_data.font=font5x7;
    glcd_init_data.temp_coef=140;
    glcd_init_data.bias=bias;
    glcd_init_data.vlcd=vlcd;
    glcd_init(&glcd_init_data);
}

void adjustSpeed(int speed) {
    OCR0 = 255 - speed;
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

    // Timer 1 (0.1 ms) cho den va servo
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
    TCNT1H=0xFC;
    TCNT1L=0xE0;
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

void hien_thi() {
    x = dht11(&nhietdo, &doam);

    lcd_gotoxy(0, 0);
    lcd_putsf("TEMP:");
    lcd_putchar(nhietdo / 10 + 48);
    lcd_putchar(nhietdo % 10 + 48);
    lcd_gotoxy(0, 1);
    lcd_putsf("HUMI:");
    lcd_putchar(doam / 10 + 48);
    lcd_putchar(doam % 10 + 48);
}

void main(void)
{
    init_glcd(4, 66);
    lcd_init(16);
    init_register();

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

    init_timer();
    while (1)
    {
        hien_thi();
        if (nhietdo == nhietdo_naodo && doam == doam_naodo)
            adjustSpeed(255);
        // OCR0 = 0 tuong duong speed 255 (nghia la dang say kho)
        if (OCR0 == 0 && nhietdo == nhietdo_naodo2) {
            adjustSpeed(0);
            glcd_outtextxy(20, 20, "SAY XONG!");
            say_xong = true;
            while(1){};
        }
        delay_ms(250);
    }
}
