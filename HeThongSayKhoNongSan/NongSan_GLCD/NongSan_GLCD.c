#include <io.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <alcd.h>

#define PIN_DHT PINB.7
#define PORT_DHT PORTB.7
#define DDR_DHT DDRB.7

// Bi?n toàn c?c
char x = 0;
unsigned char nhietdo = 0, doam = 0;
unsigned char nhietdo_naodo = 28, nhietdo_naodo2 = 30, doam_naodo = 53;
bool say_xong = false;
int counter = 0;
int checksum = 0;

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

// Hàm ð?c DHT11
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

// Hàm kh?i t?o các thanh ghi và ph?n c?ng
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

// Hàm ði?u ch?nh t?c ð? ð?ng cõ
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

// Hàm hi?n th? nhi?t ð? và ð? ?m trên GLCD
void hien_thi_glcd() {
    x = dht11(&nhietdo, &doam);

    // Hi?n th? nhi?t ð?
    glcd_clear();  // Xóa màn h?nh GLCD trý?c khi hi?n th?
    glcd_outtextxy(0, 0, "TEMP:");  // Hi?n th? nh?n "TEMP"
    glcd_putchar(48 + nhietdo / 10);  // Hi?n th? ch? s? ð?u tiên c?a nhi?t ð?
    glcd_putchar(48 + nhietdo % 10);  // Hi?n th? ch? s? th? hai c?a nhi?t ð?

    // Hi?n th? ð? ?m
    glcd_outtextxy(0, 10, "HUMI:");  // Hi?n th? nh?n "HUMI"
    glcd_putchar(48 + doam / 10);  // Hi?n th? ch? s? ð?u tiên c?a ð? ?m
    glcd_putchar(48 + doam % 10);  // Hi?n th? ch? s? th? hai c?a ð? ?m
}

// Hàm chính
void main(void) {
    init_glcd(4, 66);  // Kh?i t?o GLCD
    init_register();   // Kh?i t?o các thanh ghi và ph?n c?ng
    init_timer();      // Kh?i t?o Timer

    while (1) {
        hien_thi_glcd();  // Hi?n th? thông tin nhi?t ð? và ð? ?m trên GLCD

        if (nhietdo == nhietdo_naodo && doam == doam_naodo)
            adjustSpeed(255);

        if (OCR0 == 0 && nhietdo == nhietdo_naodo2) {
            adjustSpeed(0);
            glcd_outtextxy(20, 20, "SAY XONG!");  // Hi?n th? "SAY XONG!" trên GLCD
            say_xong = true;
            while (1) {};  // D?ng t?i ðây khi motor ð? d?ng
        }

        delay_ms(250);
    }
}
