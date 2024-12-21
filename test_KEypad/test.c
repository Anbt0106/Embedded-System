#include <io.h>
#include <delay.h>
#include <keypad.c> // G?i t?p keypad.h �? s? d?ng c�c h�m v� bi?n li�n quan
#include <alcd.h>   // Th� vi?n LCD

void main(void) {
    // Kh?i t?o LCD
    lcd_init(16); 
    lcd_clear();

    // Kh?i t?o c?ng cho b�n ph�m
    DDRF = 0b1110 1010 = 0xEA;
    PORTF = 0b0001 0101 = 0x15;

    lcd_puts("Press Keys:"); // Hi?n th? h�?ng d?n tr�n LCD

    while (1) {
        BUTTON(); // G?i h�m BUTTON t? keypad.h �? �?c ph�m
        lcd_gotoxy(0, 1); // Di chuy?n con tr? �?n d?ng th? 2 tr�n LCD
        lcd_puts(input_buffer); // Hi?n th? n?i dung �? nh?p t? b�n ph�m
    }
}
