#include <io.h>
#include <hardware.c>
#include <glcd.h>
#include <delay.h>
#include <ds1307.h>
#include <stdlib.h>
#include <alcd.h>
// ??nh nghia c?ng servo
#define servo_1 PORTC.7

// Bi?n to?n c?c
int dem = -1;              // Bi?n di?u khi?n servo
int vi_tri = 0;            // V? tr? c?a servo (ban d?u l? ngh?)
unsigned char gio, phut, giay;  // Th?i gian t? RTC
bool dang_dung = false;    // Tr?ng th?i t?m d?ng
bool dang_cho_an = false;  // Tr?ng th?i dang cho an

// Ng?t tr?n Timer1 (di?u khi?n servo)
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    // ??t l?i gi? tr? Timer1
    TCNT1H = 0xFCE0 >> 8;
    TCNT1L = 0xFCE0 & 0xff;

    // ?i?u khi?n servo
    dem++;
    if (dem >= 200)
        dem = 0;
    if (dem < vi_tri)
        servo_1 = 1;
    else
        servo_1 = 0;
}

// H?m c?p nh?t th?i gian t? RTC
//void cap_nhat_thoi_gian()
//{
  //  rtc_get_time(&gio, &phut, &giay);

    //lcd_gotoxy(0, 0);
    //lcd_putchar((gio / 10) + 48);
   // lcd_putchar((gio % 10) + 48);
   // lcd_putchar(':');
   // lcd_putchar((phut / 10) + 48);
   // lcd_putchar((phut % 10) + 48);
    //lcd_putchar(':');
    //lcd_putchar((giay / 10) + 48);
    //lcd_putchar((giay % 10) + 48);
//}

// H?m ch?nh
void main(void)
{
    // Kh?i t?o GLCD v? c?c thi?t b? ngo?i vi
    init_glcd(4, 66);
    init_lcd();
    init_register();
    init_timer();
    lcd_init(16);
    i2c_init();
    rtc_init(0, 0, 0);
    rtc_set_time(11, 10, 30);

    #asm("sei")  // Cho ph?p ng?t

    // ??m b?o servo kh?ng ho?t d?ng khi kh?i d?ng
    servo_1 = 0;

    while (1)
    {
      //  cap_nhat_thoi_gian();

        // Ki?m tra c?ng t?c 1 (T?m d?ng)
        if (PINB.2 == 0)
        {    
             lcd_clear();
            dang_dung = true;
            dang_cho_an = false;
            dung_DC();  // D?ng d?ng co
            vi_tri = 0;  // ??t servo v? v? tr? ngh?   
            lcd_putsf("Tam dung");
            glcd_outtextxy(30, 25, "Tam dung");
            delay_ms(500); 
            
        }
       
        // Ki?m tra c?ng t?c 2 (Cho c? an)
        if (PINB.3 == 0 && !dang_dung)
        {
            dang_cho_an = true;
            vi_tri = 100;  // ??t v? tr? servo d? cho an
            lcd_putsf("Dang cho an ...")        ;
            glcd_outtextxy(30, 25, "Dang cho an...");
            delay_ms(2000);  // Gi? l?p th?i gian cho an
            vi_tri = 0;      // ??t l?i v? tr? servo ban d?u
            dang_cho_an = false;
        }

        // X?a m?n h?nh khi kh?ng c? ho?t d?ng
        if (!dang_dung && !dang_cho_an)
        { 
            lcd_clear();
            glcd_clear();
        }
    }
}
