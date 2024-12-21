#include <io.h>
#include <hardware.c>
#include <glcd.h>
#include <delay.h>
#include <ds1307.h>
#include <stdlib.h>
//#include <led7.c>

// ??nh nghia c?ng servo
#define servo_1 PORTC.7

// Bi?n to?n c?c
int dem = -1;
int vi_tri = 0;
unsigned char gio, phut, giay;  // Th?i gian t? RTC
bool cho_an = false;    // Tr?ng th?i t?m d?ng
bool dang_cho_an = false;  // Tr?ng th?i dang cho an
int hour_1, hour_2, min_1, min_2, second_1, second_2;
char buf[20];
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
void cap_nhat_thoi_gian()
{
    rtc_get_time(&gio, &phut, &giay);
    hour_1 = gio/10;
    hour_2 = gio - hour_1*10;
    min_1 = phut/10;
    min_2 = phut - min_1*10;
    second_1 = giay / 10;
    second_2 = giay - second_1*10;
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putchar(hour_1 + 48);
    lcd_putchar(hour_2 + 48);
    lcd_putchar(':');
    lcd_putchar(min_1 + 48);
    lcd_putchar(min_2 + 48);

//    hienThi(hour_1, 0b00000001);
//    hienThi(hour_2, 0b00001000);
//    hienThi(min_1, 0b00010000);
//    hienThi(min_2, 0b01000000);

    glcd_clear();
    itoa(hour_1, buf);
    glcd_outtext(buf);
    itoa(hour_2, buf);
    glcd_outtext(buf);
    glcd_putchar(':');
    itoa(min_1, buf);
    glcd_outtext(buf);
    itoa(min_2, buf);
    glcd_outtext(buf);


}

// H?m ch?nh
void main(void)
{
    // 2 led
    DDRD.4 = 1;
    DDRD.5 = 1;
    // BT 1, 2
    PORTB.2 = 1;
    PORTB.3 = 1;
    PORTD.7 = 1;
    // lcd
    DDRD.7 = 1;
    DDRB.4 = 1;
    // servo
    DDRC.7 = 1;
    DDRF = 0xFF;
    DDRD = 0xFF;
    DDRA.7 = 1;
    // Kh?i t?o GLCD v? c?c thi?t b? ngo?i vi
    init_glcd(4, 66);
    init_lcd();
    init_timer();
    i2c_init();
    rtc_init(0, 0, 0);

#asm("sei")  // Cho ph?p ng?t

    // ??m b?o servo kh?ng ho?t d?ng khi kh?i d?ng
    servo_1 = 0;
    dung_DC();
    while (1)
    {
        // Ki?m tra c?ng t?c 1 (T?m d?ng)
        if (PINB.2 == 0)
        {
            int dem_nguoc = 5;
            while(dem_nguoc != -1){
                char buffer[10];
                itoa(dem_nguoc, buffer);
                //hienThi(dem_nguoc,0b01000000);
                glcd_clear();
                glcd_outtextxy(15, 15, buffer);
                lcd_clear();
                lcd_puts(buffer);
                dem_nguoc--;
                delay_ms(1000);
            }
            if (dem_nguoc == -1){
                quay_DC();
                glcd_clear();
                glcd_outtextxy(15, 15, "DANG CHO CA AN...");

                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_putsf("DANG CHO CA AN:");
                delay_ms(5000);
            }
        }
        cap_nhat_thoi_gian();
        dung_DC();
    }
}
