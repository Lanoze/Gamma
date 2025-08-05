#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include <math.h>

#define SAIDA 13
#define VRX_PIN 26   
#define VRY_PIN 27   
#define JOYSTICK_BUTTON 22
#define wr 65535

uint32_t last_time=0;
int table_index=0;

uint8_t gamma_table[256];

void generate_gamma_table(float gamma) {
 puts("Printing Gamma Table:");
    for (int i = 0; i < 256; ++i) {
        // 1. Normaliza o valor de entrada 'i' para uma escala de 0.0 a 1.0
        // 2. Aplica a função de potência (correção de gama)
        // 3. Converte o resultado de volta para a escala de 0 a 255
        float corrected = powf((float)i / 255.0f, gamma) * 255.0f;
        
        // Armazena o valor calculado na tabela
        gamma_table[i] = (uint8_t)corrected;
        printf("gamma_table[%d] = %hhu\n",i,gamma_table[i]);
    }
}

void comando_joystick(uint16_t y){
 uint32_t current_time = to_ms_since_boot(get_absolute_time());
 if(current_time-last_time>=10){
     if(y>=3500){
         uint16_t pwm_level;
         last_time=current_time;
         table_index++;
         if(table_index>255){
             table_index=255;
             return;
            }
         pwm_level=(uint16_t)gamma_table[table_index]*257;
         printf("table index = %d, pwm_level = %u\n",table_index,pwm_level);
         pwm_set_gpio_level(SAIDA,pwm_level);
        }else if(y<=700){
         uint16_t pwm_level;
         last_time=current_time;
         table_index--;
         if(table_index<0){
             table_index=0;
             return;
            }
         pwm_level=(uint16_t)gamma_table[table_index]*257;
         printf("table index = %d, pwm_level = %u\n",table_index,pwm_level);
         pwm_set_gpio_level(SAIDA,pwm_level);
        }
    }
}

uint pwm_init_gpio(uint gpio, uint wrap, float divider){
  gpio_set_function(gpio, GPIO_FUNC_PWM);

  uint slice_num = pwm_gpio_to_slice_num(gpio);
  pwm_set_wrap(slice_num, wrap);
  
  pwm_set_enabled(slice_num, true);  
  pwm_set_clkdiv(slice_num, divider);
  return slice_num;
}

int main()
{
    //uint16_t vry_value;
    stdio_init_all();
    pwm_init_gpio(SAIDA, wr,4.0f);
    generate_gamma_table(2.2f);
    adc_init();
    adc_gpio_init(VRY_PIN);
    adc_select_input(0);
    while (true) {
     comando_joystick(adc_read());
     sleep_ms(10);
    }
 puts("Erro fatal");
 return 1;
}
