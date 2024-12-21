#include <io.h>
#include <delay.h>
#include <keypad.c> // G?i t?p keypad.h ð? s? d?ng các hàm và bi?n liên quan
#include <alcd.h>   // Thý vi?n LCD

void main(void) {
    // Kh?i t?o LCD
    lcd_init(16); 
    lcd_clear();

    // Kh?i t?o c?ng cho bàn phím
    DDRF = 0b1110 1010 = 0xEA;
    PORTF = 0b0001 0101 = 0x15;

    lcd_puts("Press Keys:"); // Hi?n th? hý?ng d?n trên LCD

    while (1) {
        BUTTON(); // G?i hàm BUTTON t? keypad.h ð? ð?c phím
        lcd_gotoxy(0, 1); // Di chuy?n con tr? ð?n d?ng th? 2 trên LCD
        lcd_puts(input_buffer); // Hi?n th? n?i dung ð? nh?p t? bàn phím
    }
}
