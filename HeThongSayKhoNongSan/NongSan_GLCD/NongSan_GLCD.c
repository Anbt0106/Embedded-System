#include <io.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <alcd.h>
#include <stdlib.h>
#include <dht11.h>

#define PIN_DHT PINB.7
#define PORT_DHT PORTB.7
#define DDR_DHT DDRB.7

// Bi?n to�n c?c
char x = 0;
unsigned char nhietdo = 0, doam = 0;
unsigned char nhietdo_naodo = 28, nhietdo_naodo2 = 30, doam_naodo = 53;
bool say_xong = false;
int counter = 0;

int keypad[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
char input_buffer[16]; // L�u s? nh?p v�o
int buffer_index = 0;  // V? tr� hi?n t?i trong b? �?m
int number = 0;

int checksum = 0;
char number_str[6];

// Interrupt c?a Timer1
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    TCNT1H = 0xE0C0 >> 8;
    TCNT1L = 0xE0C0 & 0xff;

    if (say_xong) {
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

// H�m �?c DHT11
char dht11(unsigned char *nhietdo, unsigned char *doam) {
    int i, j;
    int buffer[5] = {0, 0, 0, 0, 0};

    DDR_DHT = 1;
    PORT_DHT = 1;
    delay_us(60);
    PORT_DHT = 0;
    delay_ms(25);
    DDR_DHT = 0;
    PORT_DHT = 1;

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

// H�m kh?i t?o c�c thanh ghi v� ph?n c?ng
void init_register() {
    DDRF = 0b11101010;
    DDRD.4 = 1;
    DDRD.5 = 1;
    DDRD.7 = 1;
    DDRB.4 = 1;
    PORTB.2 = 1;
    PORTB.3 = 1;
    PORTD.7 = 1;
    DDRC.7 = 1;
}

// H�m �i?u ch?nh t?c �? �?ng c�
void adjustSpeed(int speed) {
    OCR0 = 255 - speed;
}

// Kh?i t?o GLCD
void init_glcd(int bias, int vlcd) {
    GLCDINIT_t glcd_init_data;
    glcd_init_data.font = font5x7;
    glcd_init_data.temp_coef = 140;
    glcd_init_data.bias = bias;
    glcd_init_data.vlcd = vlcd;
    glcd_init(&glcd_init_data);
}

// Kh?i t?o Timer
void init_timer() {
    ASSR = 0 << AS0;
    TCCR0 = (1 << WGM00) | (1 << COM01) | (1 << COM00) | (1 << CS02);
    TCNT0 = 0x00;
    adjustSpeed(0);

    TCCR1A = 0;
    TCCR1B = (1 << CS10);
    TCNT1H = 0xFC;
    TCNT1L = 0xE0;
    TIMSK = (1 << TOIE1);
    ETIMSK = 0;
#asm("sei")
}

// H�m hi?n th? nhi?t �? v� �? ?m tr�n GLCD
void hien_thi_glcd() {
    x = dht11(&nhietdo, &doam);

    // Hi?n th? nhi?t �?
    glcd_clear();  // X�a m�n h?nh GLCD tr�?c khi hi?n th?
    glcd_outtextxy(0, 0, "TEMP:");  // Hi?n th? nh?n "TEMP"
    glcd_putchar(48 + nhietdo / 10);  // Hi?n th? ch? s? �?u ti�n c?a nhi?t �?
    glcd_putchar(48 + nhietdo % 10);  // Hi?n th? ch? s? th? hai c?a nhi?t �?

    // Hi?n th? �? ?m
    glcd_outtextxy(0, 10, "HUMI:");  // Hi?n th? nh?n "HUMI"
    glcd_putchar(48 + doam / 10);  // Hi?n th? ch? s? �?u ti�n c?a �? ?m
    glcd_putchar(48 + doam % 10);  // Hi?n th? ch? s? th? hai c?a �? ?m
}

// H�m x? l? b�n ph�m ma tr?n
void BUTTON() {
    char a;
    int i, j;
    for (j = 0; j < 3; j++) {
        // K�ch ho?t t?ng h�ng c?a b�n ph�m
        if (j == 0) PORTF = 0b11111101;
        if (j == 1) PORTF = 0b11110111;
        if (j == 2) PORTF = 0b11011111;

        for (i = 0; i < 3; i++) {
            // Ki?m tra t?ng c?t
            if (i == 0) {
                a = PINF & 0x04;
                if (a != 0x04) {
                    input_buffer[buffer_index++] = keypad[j][i] + '0'; // Chuy?n s? sang k? t? �? l�u v�o b? �?m
                }
            }
            if (i == 1) {
                delay_ms(50);
                a = PINF & 0x10;
                if (a != 0x10) {
                    input_buffer[buffer_index++] = keypad[j][i] + '0';
                }
            }
            if (i == 2) {
                delay_ms(50);
                a = PINF & 0x01;
                if (a != 0x01) {
                    input_buffer[buffer_index++] = keypad[j][i] + '0';
                }
            }
        }
    }
}

// H�m ch�nh
void main(void) {
    init_glcd(4, 66);  // Kh?i t?o GLCD
    init_register();   // Kh?i t?o c�c thanh ghi v� ph?n c?ng
    init_timer();      // Kh?i t?o Timer

    while (1) {
        BUTTON();  // Ki?m tra b�n ph�m ma tr?n
        delay_ms(250);

        if (!(PINB & (1 << PINB2))) {  // Button 1 ��?c nh?n
            // Nh?p gi� tr? nhi?t �? v� �? ?m t? b�n ph�m
            if (PINB.2 == 0) // N?u button 1 ��?c nh?n
            {
                // break
            }
            glcd_clear();
            glcd_outtextxy(0, 0, "Enter Temp: ");
            // �?c gi� tr? t? b�n ph�m v� chuy?n v�o `input_buffer`
            while (buffer_index < 4) {  // Ch? cho ph�p nh?p t?i �a 2 ch? s?
                // N?u �? 4 ch? s?, nh?p v�o buffer
            }
            nhietdo = atoi(input_buffer);  // Chuy?n chu?i th�nh s?
            glcd_outtextxy(0, 10, "Enter Humidity: ");
            buffer_index = 0;  // Reset b? �?m
            while (buffer_index < 4) {
                // Nh?p �? ?m
            }
            doam = atoi(input_buffer);  // Chuy?n chu?i th�nh s?
        }

        if (!(PINB & (1 << PINB3))) {  // Button 2 ��?c nh?n
            hien_thi_glcd();
            // Hi?n th? nhi?t �? v� �? ?m t? DHT11
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

        delay_ms(250);
    }
}
