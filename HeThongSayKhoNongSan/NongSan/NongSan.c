
#include <io.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <dht11.c>
#include <hardware.c>

char x = 0;
unsigned char nhietdo = 0, doam = 0;
/*
 nhietdo_naodo, doam_naodo: Nhiet do, do am bat dau say kho
 nhietdo_naodo2: Nhiet do ket thuc say kho
 */
unsigned char nhietdo_naodo = 28, nhietdo_naodo2 = 30, doam_naodo = 53;
bool say_xong = false;
int counter = 0;

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
    init_lcd();
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
