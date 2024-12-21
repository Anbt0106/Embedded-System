/*
 * bai1.c
 *
 * Created: 12/8/2022 9:06:36 PM
 * Author: Who
 */

#include <io.h>
#include <delay.h>
#include <glcd.h>
#include <font5x7.h>
#include <stdio.h>
#include <stdlib.h>
#include <eeprom.h>
#include <hardware.c>

// macro for easier usage
#define read_eeprom_word(address) eeprom_read_word ((const unsigned int*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((unsigned int*)address,(unsigned int)value)
#define update_eeprom_word(address,value) eeprom_update_word ((unsigned int*)address,(unsigned int)value)
unsigned int EEMEM my_eeprom;

char tmp[10];
char tmp2[10];
int miss = 0;
int counter = 0;
int dem_sai = 0;
int sai_toi_da = 4;

// Timer 0 overflow interrupt service routine with 1 ms
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    TCNT0=0x06;
    // Place your code here
    if (dem_sai >= sai_toi_da) {
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
    else {
        PORTD.4 = 0;
        PORTD.5 = 0;
    }       
}


// "inherited" from tai lieu thuc hanh

void mainScreen(unsigned char * c, int pass, int remaining) {
    glcd_clear();
    itoa(remaining, tmp2);
    glcd_outtextxy(5, 10, tmp2);
    glcd_outtextxy(15, 20, c);
    if (pass != -1) {
        itoa(pass, tmp);
        glcd_outtextxy(30, 30, tmp);
    }
}

void mainScreen2(unsigned char * c, char pass[], int pos, int remaining) {
    glcd_clear();
    itoa(remaining, tmp2);
    glcd_outtextxy(5, 10, tmp2);
    glcd_outtextxy(15, 20, c);
    if (pass != -1) {
        glcd_outtextxy(pos, 30, pass);
    }
}

void mainScreen3(unsigned char * c, int pass, int pos, char pass_ano[], int remaining) {
    glcd_clear();
    itoa(remaining, tmp2);
    glcd_outtextxy(5, 10, tmp2);
    glcd_outtextxy(15, 20, c);
    if (pass != -1) {
        itoa(pass, tmp);
        glcd_outtextxy(30, 30, pass_ano);
        glcd_outtextxy(pos, 30, tmp);
    }
}

void mainScreenLcd(unsigned char * c, char pass_ano1[]){
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts(c);
    lcd_gotoxy(0, 1);
    lcd_putchar(pass_ano1[0]);
    lcd_putchar(pass_ano1[1]);
    lcd_putchar(pass_ano1[2]);
}

void exe() {
    //char pass_ano[3];
    unsigned int input_times = 0, input = -1, pass = 0, require = 118, reader;
    reader = read_eeprom_word(&my_eeprom);
    if (reader >= 100 && reader <= 999)
        require = reader;   
    dem_sai = 0;
    while (pass != require) {
        char pass_ano[3], pass_ano1[3];
        pass_ano[0]=' '; pass_ano1[0]=' ';
        pass_ano[1]=' '; pass_ano1[1]=' ';
        pass_ano[2]=' '; pass_ano1[2]=' ';
        mainScreen("PASSWORD?", -1, sai_toi_da-dem_sai);

        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("PASSWORD? ");
        lcd_putchar(sai_toi_da-dem_sai+'0');
        while (input_times != 3) {
            input = read();
            delay_ms(200);
            if (input != -1) {
                input_times++;
                pass = pass * 10 + input;
                pass_ano1[input_times-1] = input+'0';
                mainScreen2("PASSWORD?", pass_ano, 30, sai_toi_da-dem_sai);
                mainScreen3("PASSWORD?", input, 30+(input_times-1)*4+input_times, pass_ano, sai_toi_da-dem_sai);

                mainScreenLcd("PASSWORD?", pass_ano1);
                lcd_gotoxy(10,0);
                lcd_putchar(sai_toi_da-dem_sai+'0');


                pass_ano[input_times-1] = '*';
                pass_ano1[input_times-1] = '*';
                delay_ms(500);
                mainScreenLcd("PASSWORD?", pass_ano1);
                lcd_gotoxy(10,0);
                lcd_putchar(sai_toi_da-dem_sai+'0');

                mainScreen2("PASSWORD?", pass_ano, 30, sai_toi_da-dem_sai);
                input = -1;
            }
        }
        
        if (pass != require) {
            glcd_clear();  
            glcd_outtextxy(15, 20, "NO?");
            
            lcd_clear();
            lcd_putsf("NO?");
            input_times = 0;
            input = -1;
            pass = 0;
            delay_ms(800);
            dem_sai++;
        }
        // bat role 1
        if (dem_sai == sai_toi_da){
            DDRC.3 = 1;
            PORTC.3 = 1;
            glcd_clear();
            glcd_outtextxy(15, 5, "khoa thiet bi");

            lcd_clear();
            lcd_gotoxy(0, 0);
            lcd_puts("Khoa thiet bi");
            return;
        }
        //miss++;
        // turn on role 1
        // if (miss == 3) {
        //     DDRC.3 = 1;
        //     PORTC.3 = 1;
        // }
    }
    
    miss = 0; 
    delay_ms(500);
    glcd_clear();
    PORTC.3 = 0;
    glcd_outtextxy(15, 20, "PASSED"); 

    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("PASSED");
}

void main(void)
{
    unsigned int pass = 0, input, input_times = 0;
    //init GLCD
    GLCDINIT_t glcd_init_data;
    glcd_init_data.font=font5x7;
    glcd_init_data.temp_coef=140;
    glcd_init_data.bias=4;
    glcd_init_data.vlcd=66;
    glcd_init(&glcd_init_data);
    
    init_lcd();
    init_register();
    DDRD.5 = 1;
    DDRD.4 = 1;
    
    //init Timer/Counter 0 with time period = 1 ms
    ASSR=0<<AS0;
    TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (1<<CS01) | (1<<CS00);
    TCNT0=0x06;
    OCR0=0x00;
    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);
    ETIMSK=(0<<TICIE3) | (0<<OCIE3A) | (0<<OCIE3B) | (0<<TOIE3) | (0<<OCIE3C) | (0<<OCIE1C); 
    #asm("sei")
    update_eeprom_word(&my_eeprom, 108);
    exe();
    delay_ms(1500);
    
    // mainScreen("NEW PASS?", -1);
    // while (input_times != 3) {
    //         input = read();
    //         delay_ms(200);
    //         if (input != -1) {
    //             input_times++;
    //             pass = pass * 10 + input;
    //             mainScreen("NEW PASS?", pass);
    //             input = -1;
    //         }
    // }
    // update_eeprom_word(&my_eeprom, pass);
    // delay_ms(800);
    // mainScreen("SAVED", -1);
    
    while (1)
    {
       
        
    }
}
