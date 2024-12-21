#include <io.h>
#include <delay.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <alcd.h>

int keypad[3][3] = {1, 4, 7, 2, 5, 8, 3, 6, 9};
char input_buffer[16]; // Luu d? li?u nh?p t? bàn phím
int buffer_index = 0;  // V? trí hi?n t?i trong b? d?m

void BUTTON() {
    char a;
    int i, j;
    for (j = 0; j < 3; j++) {
        // K?ch ho?t t?ng h?ng c?a b?n ph?m
        if (j == 0) PORTF = 0b11111101;
        if (j == 1) PORTF = 0b11110111;
        if (j == 2) PORTF = 0b11011111;

        for (i = 0; i < 3; i++) {
            // Ki?m tra t?ng c?t
            if (i == 0) {
                a = PINF & 0x04;
                if (a != 0x04) {
                    input_buffer[buffer_index++] = keypad[j][i] + '0'; // Chuy?n s? sang k? t? v? luu v?o b? d?m
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