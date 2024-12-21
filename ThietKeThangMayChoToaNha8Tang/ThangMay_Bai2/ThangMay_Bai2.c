/*
 * elevator.c
 *
 * Created: 12/29/2022 10:30:54 PM
 * Author: Who
 */

#include <io.h>
#include <hardware.c>
#include <delay.h>
#include <stdlib.h>
#include <glcd.h>
#include <alcd.h>

#define servo_1 PORTC.7

int dem = 0, vi_tri = 10;
char what[16];
int i;
// dieu khien cac tang trong thang may
bool order[9] = {false, false, false, false, false, false, false, false, false};


// dieu khien trang thai len / xuong
bool len = true;
int currentLevel = 0;

// Timer1 overflow interrupt service routine (delay 0.1 ms)
interrupt [TIM1_OVF]

void timer1_ovf_isr(void) {
    int input;
    // Reinitialize Timer1 value
    TCNT1H = 0xFCE0 >> 8;
    TCNT1L = 0xFCE0 & 0xff;

    // servo
    dem++;
    if (dem >= 200)
        dem = 0;
    if (dem < vi_tri)
    servo_1 = 1;
    else
    servo_1 = 0;

    // ~ delay 199 ms ban phim ma tran   
    if (dem == 199) {
        input = read();
        if (input != -1)
            order[input] = true;
    }
}

void hienThi(int i) {

    if (i == 0) {
        PORTF = 0x81;
        PORTD = 0xF9;
        PORTA = 0x7F;
    }
    if (i == 1) {
        PORTF = 0x87;
        PORTD = 0xFF;
        PORTA = 0x7F;
    }
    if (i == 2) {
        PORTF = 0x85;
        PORTD = 0xF1;
        PORTA = 0xFF;
    }
    if (i == 3) {
        PORTF = 0x85;
        PORTD = 0xF5;
        PORTA = 0x7F;
    }
    if (i == 4) {
        PORTF = 0x83;
        PORTD = 0xF7;
        PORTA = 0x7F;
    }
    if (i == 5) {
        PORTF = 0xA1;
        PORTD = 0xF5;
        PORTA = 0x7F;
    }
    if (i == 6) {
        PORTF = 0xA1;
        PORTD = 0xF1;
        PORTA = 0x7F;
    }
    if (i == 7) {
        PORTF = 0x85;
        PORTD = 0xFF;
        PORTA = 0x7F;
    }
    if (i == 8) {
        PORTF = 0x81;
        PORTD = 0xF1;
        PORTA = 0x7F;
    }
    if (i == 9) {
        PORTF = 0x81;
        PORTD = 0xF5;
        PORTA = 0x7F;
    }

}

void thang_may() {

    // neu ko co tang nao dc bam thi thoi ko lam gi ca
    for (i = 0; i <= 8; i++) {
        if (order[i] == true) {
            break;
        }
        if (i == 8) {
            return;
        }
    }
    // thiet lap trang thai len/xuong
    if (len) {
        for (i = currentLevel; i <= 8; i++) {
            // neu len tang 8 roi ma ko co yeu cau thi chuyen che do cho di xuong
            if (i == 8 && order[i] == false) {
                len = false;
            } else if (order[i] == true) {
                break;
            }
        }
    }
    if (!len) {
        for (i = currentLevel; i >= 0; i--) {
            // tuong tu voi doan di len. neu da o tang 0
            if (i == 0 && order[i] == false) {
                len = true;
                return;
            } else if (order[i] == true) {
                break;
            }
        }
    }

    // trong truong hop co tang dc bam, mo servo 1 goc 90 do trong 2s 
    vi_tri = 15;
    delay_ms(2000);
    vi_tri = 10;

    if (len)
        quay_DC();
    else
        quay_nguoc_DC();

    while (order[currentLevel] == false) {
        glcd_clear();
        glcd_outtextxy(5, 10, "Dang di chuyen");

        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_puts("Dang di chuyen ...");
        if (len) {
            currentLevel++;
        } else {
            currentLevel--;
        }
        // glcd_clear();
        itoa(currentLevel, what);
        glcd_outtextxy(5, 25, "Den tang");
        glcd_outtextxy(55, 25, what);


        lcd_gotoxy(0, 1);
        lcd_puts("Den tang: ");
        lcd_putchar(currentLevel + '0');
        hienThi(currentLevel);
        delay_ms(1000);
    }
    glcd_clear();
    itoa(currentLevel, what);
    glcd_outtextxy(5, 25, "Den tang");
    glcd_outtextxy(55, 25, what);

    lcd_clear();
    lcd_gotoxy(0, 1);
    lcd_puts("Den tang: ");
    lcd_putchar(currentLevel + '0');

    order[currentLevel] = false;
    dung_DC();
    vi_tri = 15;
    delay_ms(2000);
    vi_tri = 10;

}

void main(void) {
    //khoi_tao_led_7_doan();
    DDRF = 0xFF;
    DDRD = 0xFF;
    DDRA = 0xFF;


    init_glcd(4, 66);
    init_lcd();
    init_register();
    init_timer();
    itoa(currentLevel, what);
    glcd_outtextxy(30, 20, what);

    while (1) {
        // Please write your application code here
        thang_may();
    }
}
