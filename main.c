//Bibliotecas FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" 

#include <stdio.h>

//Bibliotecas padrão do Pico SDK
#include "pico/stdlib.h" // Biblioteca padrão pico
#include "hardware/gpio.h" // Biblioteca de GPIOs
#include "hardware/adc.h" // Biblioteca do ADC
#include "hardware/pwm.h" // Biblioteca do PWM
#include "hardware/i2c.h" // Biblioteca do I2C

//Bibliotecas HAL
#include "buttons.h"
#include "joystick.h"
#include "buzzer.h"
#include "include/hal_ssd1306.h"
#include "led_rgb.h"


//Listas de notas musicais
enum noteList {nNull,nC,nD,nE,nF,nG,nA,nB};

int noteFrequencies[8] = {0,NOTE_C4,NOTE_D4,NOTE_E4,NOTE_F4,NOTE_G4,NOTE_A4,NOTE_B4};

char noteLetters[8] = {' ','C','D','E','F','G','A','B'};

int noteArrayA[16] = {nC,nD,nE,nF,nG,nA,nB,0,nB,nA,nG,nF,nE,nD,nC,nC};
int noteArrayB[16] = {nD,nD,nD,nG,nG,nG,nA,nA,nA,nC,nC,nC,nE,nE,nE,nD};

int notePosition = 0;

int noteCursorX = 0;
int noteCursorY = 1;

bool buttonProcessed = false;
bool joystickProcessed = false;

//Handles das tarefas
TaskHandle_t display_task_handle = NULL;
TaskHandle_t buzzer_task_handle = NULL;
TaskHandle_t button_task_handle = NULL;
TaskHandle_t joystick_task_handle = NULL;

//Semaforos das tarefas
SemaphoreHandle_t xInputSemaphore;    
SemaphoreHandle_t xBuzzerSemaphore; 

//situação daas tarefas
bool led_task_running = true;
bool buzzer_task_running = true;

enum {
  LED_RED,
  LED_GREEN,
  LED_BLUE,
} ledStatus;

enum {
  BUZZER_OFF,
  BUZZER_ON,
} buzzerStatus;

enum {
  NO_BUTTON,
  BUTTON_A,
  BUTTON_B,
  BUTTON_J
} buttonStatus;

enum {
  NO_JOY,
  JOY_UP,
  JOY_DOWN,
  JOY_LEFT,
  JOY_RIGHT
} joystickStatus;

void print_note_arrays(){
  printf("Note Array A: ");
  for (int i = 0; i < 16; ++i) {
    printf("%d(%c) ", noteArrayA[i], noteLetters[noteArrayA[i]]);
  }
  printf("\n");

  printf("Note Array B: ");
  for (int i = 0; i < 16; ++i) {
    printf("%d(%c) ", noteArrayB[i], noteLetters[noteArrayB[i]]);
  }
  printf("\n");

}

void display_task(){
  while(true){
    hal_ssd1306_clear();
    for (int i = 0; i < 16; ++i) {
      hal_ssd1306_draw_char(i, 0, noteLetters[noteArrayA[i]]);
      hal_ssd1306_draw_char(i, 2, noteLetters[noteArrayB[i]]);
    }
    hal_ssd1306_draw_string(noteCursorX, noteCursorY, "-");
    hal_ssd1306_draw_string(notePosition, 4, ">");
    //hal_ssd1306_draw_string(0, 5, "<^#>      Cursor");
    hal_ssd1306_draw_string(0, 6, "A #   nota   ^ B");
    hal_ssd1306_draw_string(0, 7, "BtnJoy     Tocar");  
    printf("Begin rendering display task\n");
    hal_ssd1306_render();
    printf("Display task rendered\n");
    vTaskDelay(10);
  }
}

void buzzer_task(){
  while (true){
    if (xSemaphoreTake(xBuzzerSemaphore, portMAX_DELAY) == pdTRUE){
      notePosition = 0;
      print_note_arrays();
      while(notePosition < 16){
        printf("buzzer_task - Note position: %d Note Frequency 1: %d Note Frequency 2: %d\n", notePosition,noteFrequencies[noteArrayA[notePosition]],noteFrequencies[noteArrayB[notePosition]]);
        hal_play_notes(noteFrequencies[noteArrayA[notePosition]], noteFrequencies[noteArrayB[notePosition]], 200);
        ++notePosition;
        if (notePosition >= 16) {
          printf("buzzer_task - Reached end of note arrays, restarting...\n");
          xSemaphoreTake(xBuzzerSemaphore, 0);
        }
      }
      vTaskDelay(10); 
      
    }
  }
}

void button_task(){
  while (true){  
    if(hal_button_a_single_press()){
      if(noteCursorY == 1){
        if(noteArrayA[noteCursorX] > 0) {
            --noteArrayA[noteCursorX];
          } else {
            noteArrayA[noteCursorX] = 7; // Reseta se ultrapassar o tamanho do array
          }
      } else if (noteCursorY == 3){
        if(noteArrayB[noteCursorX] > 0) {
            --noteArrayB[noteCursorX];
          } else {
            noteArrayB[noteCursorX] = 7; // Reseta se ultrapassar o tamanho do array
          }
      } else {
        noteCursorY = 1;
      }
    } 
    if(hal_button_b_single_press()){
      if(noteCursorY == 1){
        if(noteArrayA[noteCursorX] < 7) {
            ++noteArrayA[noteCursorX];
          } else {
            noteArrayA[noteCursorX] = 0; // Reseta se ultrapassar o tamanho do array
          }
      } else if (noteCursorY == 3){
        if(noteArrayB[noteCursorX] < 7) {
            ++noteArrayB[noteCursorX];
          } else {
            noteArrayB[noteCursorX] = 0; // Reseta se ultrapassar o tamanho do array
          }
      } else {
        noteCursorY = 1;
      }
    } 
    if(hal_button_j_single_press()){
      xSemaphoreGive(xBuzzerSemaphore);
    }
  vTaskDelay(10);  
  }
}

void joystick_task(){
  while (true){
    switch(hal_joystick_read_direction()){
      case joyUP:
          printf("Joystick UP\n");
          if(noteCursorY == 1) {
            noteCursorY = 3; // Muda para a linha de notas B
          } else if (noteCursorY == 3) {
            noteCursorY = 1; // Muda para a linha de notas A
          } else {
            noteCursorY = 1; // Reseta para a linha de notas A
          }
          break;
      case joyDOWN:
          printf("Joystick DOWN\n");
          if(noteCursorY == 1) {
            noteCursorY = 3; // Muda para a linha de notas B
          } else if (noteCursorY == 3) {
            noteCursorY = 1; // Muda para a linha de notas A
          } else {
            noteCursorY = 1; // Reseta para a linha de notas A
          }
          break;
      case joyLEFT:
          printf("Joystick LEFT\n");
          if(noteCursorX > 0) {
            --noteCursorX; // Move o cursor para a esquerda
          } else {
            noteCursorX = 15; // Reseta se ultrapassar o tamanho do array
          }
          break;
      case joyRIGHT:
          printf("Joystick RIGHT\n");
          if(noteCursorX < 15) {
            ++noteCursorX; // Move o cursor para a direita
          } else {
            noteCursorX = 0; // Reseta se ultrapassar o tamanho do array
          }
          break;
    }
  vTaskDelay(10);
  }
}

int main() {
   stdio_init_all();
   hal_ssd1306_init(); // Inicializa o display OLED SSD1306 com comunicação I2C.
   hal_buzzer_init(); // Inicializa o buzzer.
   hal_buttons_init(); // Inicializa os botões.
   initJoystick(); // Inicializa o joystick.
   hal_init_rgb_led(); // Inicializa o LED RGB.
    
  hal_ssd1306_clear();
  hal_ssd1306_draw_string(0, 0, "BitDogLab RTOS  ");
  hal_ssd1306_draw_string(0, 1, "Sintetizador de ");
  hal_ssd1306_draw_string(0, 2, "Notas Musicais  ");
  hal_ssd1306_draw_string(0, 3, "by pedrohdavelar");
  hal_ssd1306_draw_string(0, 5, "Pressione       ");      
  hal_ssd1306_draw_string(0, 6, "  qualquer botao");
  hal_ssd1306_draw_string(0, 7, "para iniciar!   ");
  hal_ssd1306_render();
  hal_wait_button_press();

  xBuzzerSemaphore = xSemaphoreCreateBinary();
  xTaskCreate(display_task, "Display_Task", 1024, NULL, 1, NULL);
  xTaskCreate(buzzer_task, "Buzzer_Task", 1024, NULL, 1, &buzzer_task_handle);
  xTaskCreate(button_task, "Read_Input", 1024, NULL, 1, &button_task_handle);
  xTaskCreate(joystick_task, "Process_Input", 1024, NULL, 1, &joystick_task_handle);

  vTaskStartScheduler();

  while(1){
        
  };
}