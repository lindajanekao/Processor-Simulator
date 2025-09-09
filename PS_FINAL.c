//Please do not copy this code without the permission from the creator
/*
Project Title: Processor Simulator
Created by Jinghan Linda Gao and Michel Viyella, March 2025.

Intro: This Processor Simulator aims to help users understand the basic
operating principles of a computer processor. Users are allowed to input
multiple instructions at a time, with a maximum of four. Once all instruction
inputs are completed, the simulation will demonstrate how the processor executes
these instructions step by step, showing the function of each unit in the
processor, including Memory, Registers, the ALU, and the PC.
*/

/* All 11 instructions are listed below. Users can follow these instructions to
 * explore how the simulator works. */
////user instruction:
/* press ADD, button_pressed=1: Ra,Rb*/
/* press SUB, button_pressed=2: Ra,Rb */
/* press NAND, button_pressed=3: Ra,Rb */
/* press ORi, button_pressed=4: imm */
/* press LOAD, button_pressed=5: Ra,Rb */
/* press STORE, button_pressed=6: Ra,Rb */
/* press BNZ, button_pressed=7:imm */
/* press BPZ, button_pressed=8:imm */
/* press shift, button_pressed=9:Ra, 0/1, imm(<4) */
/* press loadiR, button_pressed=10: Ra, imm */
/* press loadiM, button_pressed=11: memory address, imm */

#include "math.h"
#include "paimon_data.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
////switches
int switchData;
volatile int* switchPtr =
    (volatile int*)0xFF200040;  // Ensure this address is correct
volatile int* LEDPtr = (volatile int*)0xFF200000;
void checkButtonPressed();

////PS2 Keyboard
#define PS2_BASE 0xFF200100
char userInput[15];
char* operation[4];
char* multiInputs[4];  // multiInputs hold all machine code as strings in array
int reg1[4] = {0};
int reg2[4] = {0};
int userInput_int = 0;
int inputIndex = 0;
int multiInputIndex = 0;
int Enter_pressed = 1;
int Finish_pressed = 0;
int editor_writing_x = 115;
int editor_writing_y = 30;
int writing_x = 180;
int writing_y = 220;
int go_back_editor = 0;
void editor_Keyboard_input();
void Keyboard_input();
void reset_input();

////Start Screen
int which_screen = -1;
void init_char_store();
void draw_characters_stored();
void draw_instructions_screen();
void draw_editor_screen_initial();
void plot_image(int x, int y);
void delay_loop();
void delay_loop_byte();
void draw_circle(int x_c, int y_c, int r, int color);
void clear_label();
void clear_text();
void draw_cursor(int, int);
void erase_cursor(int, int);
// *the start screen is designed by ChatGPT
// short unsigned int paimon[153600] = {
/* Frame buffer data omitted for brevity */
//};

////Main screen
void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int color);
void draw_circle(int x_c, int y_c, int r, int color);
void swap(int* a, int* b);
unsigned char reverse_bits(unsigned char b);
void draw_main_screen();
void draw_char(int x, int y, char c, int color);
void draw_string(int x, int y, char* str, int color);
void draw_integer(int x, int y, int num, int color);
//// global variable
int sourceReg1, sourceReg2;
int button_pressed[4] = {-1};
int operation_index;
int pixel_buffer_start;
volatile int* pixel_ctrl_ptr = (int*)0xFF203020;
int shiftValue[4] = {-1};
int shiftLR[4] = {-1};
const unsigned char font[37][8] = {
    // A-Z (Index 0-25)
    {0x3C, 0x42, 0x81, 0x81, 0xFF, 0x81, 0x81, 0x81},  // 'A'
    {0xFE, 0x81, 0x81, 0xFE, 0x81, 0x81, 0x81, 0xFE},  // 'B'
    {0x7E, 0x81, 0x80, 0x80, 0x80, 0x80, 0x81, 0x7E},  // 'C'
    {0xFC, 0x82, 0x81, 0x81, 0x81, 0x81, 0x82, 0xFC},  // 'D'
    {0xFF, 0x80, 0x80, 0xFE, 0x80, 0x80, 0x80, 0xFF},  // 'E'
    {0xFF, 0x80, 0x80, 0xFE, 0x80, 0x80, 0x80, 0x80},  // 'F'
    {0x7E, 0x81, 0x80, 0x8F, 0x81, 0x81, 0x81, 0x7E},  // 'G'
    {0x81, 0x81, 0x81, 0xFF, 0x81, 0x81, 0x81, 0x81},  // 'H'
    {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C},  // 'I'
    {0x7F, 0x08, 0x08, 0x08, 0x08, 0x08, 0x88, 0x70},  // 'J'
    {0x81, 0x82, 0x84, 0xF8, 0x84, 0x82, 0x81, 0x81},  // 'K'
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF},  // 'L'
    {0x81, 0xC3, 0xA5, 0x99, 0x81, 0x81, 0x81, 0x81},  // 'M'
    {0x81, 0xC1, 0xA1, 0x91, 0x89, 0x85, 0x83, 0x81},  // 'N'
    {0x7E, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7E},  // 'O'
    {0xFE, 0x81, 0x81, 0xFE, 0x80, 0x80, 0x80, 0x80},  // 'P'
    {0x7E, 0x81, 0x81, 0x81, 0x85, 0x82, 0x81, 0x7D},  // 'Q'
    {0xFE, 0x81, 0x81, 0xFE, 0x82, 0x81, 0x81, 0x81},  // 'R'
    {0x7E, 0x81, 0x80, 0x7E, 0x01, 0x01, 0x81, 0x7E},  // 'S'
    {0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},  // 'T'
    {0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7E},  // 'U'
    {0x81, 0x81, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18},  // 'V'
    {0x81, 0x81, 0x81, 0x99, 0xA5, 0xC3, 0x81, 0x81},  // 'W'
    {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81},  // 'X'
    {0x81, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18},  // 'Y'
    {0xFF, 0x02, 0x04, 0x18, 0x30, 0x40, 0x80, 0xFF},  // 'Z'

    // 0-9 (Index 26-35)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},  // '0'
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},  // '1'
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},  // '2'
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},  // '3'
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},  // '4'
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},  // '5'
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},  // '6'
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},  // '7'
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},  // '8'
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},  // '9'
    {0x00, 0x00, 0x00, 0x7E, 0x7E, 0x00, 0x00, 0x00}   //'-' for neg number
};
////Memory
int memory_data[4] = {
    24, 5, 46, 79};  // initialize all data in memory as 8 and color black
short int memDataColor[4] = {0};
int memCodeColor[4] = {0};
int address_selected = 0;
int content = 0;
void highlight_addr_content(int);
void draw_memory_address();
void draw_memory_data();
void draw_memory_machCode();
void clean_mem_data();
void machCode_decimal_to_binary(int num);
void translate_input_to_machine_code();
void reset_for_new_inputs();
char machCode_binary[9];
int memory_content_draw_x = 15;
int memory_content_draw_y = 30;

////Register
void highlight_sourceReg(int, int);
void highlight_destReg(int reg);
void update_reg_zn_value(int, int, int);
int regData[4] = {0, 1, 2, 3};
short int regDataColor[4] = {0, 0, 0, 0};
void draw_reg_data(short int, short int, short int, short int);
void clean_reg_data_display();
////ALU
char bitwise_input_binary1[9];
char bitwise_input_binary2[9];
int z_bit = 0;
int n_bit = 0;
void ALU_decimal_to_binary1(int num);
void ALU_decimal_to_binary2(int num);
void ALU_input_display(int, int);
void ALU_output_display(int);
////PC
int PC = 0;
int pc_imm_data[4] = {0};
////Operation main function
void operation_display();
void operation_indication_display();

int main(void) {
  /* Read location of the pixel buffer from the pixel buffer controller */
  pixel_buffer_start = *pixel_ctrl_ptr;
  switchData = *switchPtr;
  *LEDPtr = switchData;
  clear_screen();
  plot_image(0, 0);
  while (which_screen == -1) {
    Keyboard_input();
  }

  draw_instructions_screen();
  while (which_screen == 0) {
    Keyboard_input();
  }

  clear_screen();
  draw_editor_screen_initial();
  while (!Finish_pressed) {
    // clear_screen();
    draw_editor_screen_initial();
    editor_Keyboard_input();
  }

  while (1) {
    clear_screen();
    /// wait for user input all instructions
    while (!Finish_pressed) {
      draw_editor_screen_initial();
      editor_Keyboard_input();
    };

    if (button_pressed[operation_index] != 10 &&
        button_pressed[operation_index] != 11) {
      // now multiInputs[] hold all mach code in binary as strings
      for (operation_index = 0; operation_index < multiInputIndex;
           operation_index++) {
        *LEDPtr = 0;
        memCodeColor[operation_index] = 0x7f0;
        clear_screen();
        draw_main_screen();
        draw_memory_address();
        draw_memory_machCode();
        draw_memory_data();
        draw_reg_data(regDataColor[0], regDataColor[1], regDataColor[2],
                      regDataColor[3]);
        operation_indication_display();
        for (int i = 0; i < 60; i++) {
          wait_for_vsync();
        }
        machCode_binary[9] = multiInputs[operation_index];
        sourceReg1 = reg1[operation_index];
        sourceReg2 = reg2[operation_index];
        if (button_pressed[operation_index] != 7 &&
            button_pressed[operation_index] != 8 &&
            button_pressed[operation_index] != 9) {
          /* not branch condition, PC only need to add 1 */
          draw_integer(140, 185, PC, 0xffff);
          PC++;
          draw_integer(140, 185, PC, 0xfd20);
          // source reg highlight
          highlight_sourceReg(sourceReg1, sourceReg2);
          draw_reg_data(regDataColor[0], regDataColor[1], regDataColor[2],
                        regDataColor[3]);
          for (int i = 0; i < 60; i++) {
            wait_for_vsync();
          }
          // display ALU operation with ADD,SUB,NAND,ORI
          if (button_pressed[operation_index] == 1 ||
              button_pressed[operation_index] == 2 ||
              button_pressed[operation_index] == 3 ||
              button_pressed[operation_index] == 4) {
            ALU_input_display(sourceReg1, sourceReg2);
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          }
          // update reg data and z/n bit accroding to operation selected by the
          // user
          update_reg_zn_value(sourceReg1, sourceReg2,
                              button_pressed[operation_index]);

          // display memory data being fetched or changed with:LOAD, STORE
          // display ALU output with ADD,SUB,NAND,ORI
          if (button_pressed[operation_index] == 5 ||
              button_pressed[operation_index] == 6) {
            highlight_addr_content(address_selected);
            draw_integer(274, 154, z_bit, 0);
            draw_integer(294, 154, n_bit, 0);
            // draw_memory_address(address);
            draw_memory_data();
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          } else {
            ALU_output_display(sourceReg1);
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          }
          // highlight updated result in destination reg
          if (button_pressed[operation_index] != 6) {
            clean_reg_data_display();
            highlight_destReg(sourceReg1);
            draw_reg_data(regDataColor[0], regDataColor[1], regDataColor[2],
                          regDataColor[3]);
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          }
        }
        // Operation with only PC
        if (button_pressed[operation_index] == 7) {  // bnz
          if (z_bit == 0) {
            /*for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }*/
            draw_integer(140, 185, PC, 0xffff);
            PC = PC + pc_imm_data[operation_index];
            draw_integer(140, 185, PC, 0xfd20);
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          } else {
            draw_integer(140, 185, PC, 0xffff);
            PC++;
            draw_integer(140, 185, PC, 0xfd20);
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          }
        }
        if (button_pressed[operation_index] == 8) {  // bpz
          if (n_bit == 0) {
            /*for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }*/
            draw_integer(140, 185, PC, 0xffff);
            PC = PC + pc_imm_data[operation_index];
            draw_integer(140, 185, PC, 0xfd20);
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          } else {
            draw_integer(140, 185, PC, 0xffff);
            PC++;
            draw_integer(140, 185, PC, 0xfd20);
            for (int i = 0; i < 60; i++) {
              wait_for_vsync();
            }
          }
        }
        if (button_pressed[operation_index] == 9) {
          highlight_sourceReg(reg1[operation_index], -1);
          draw_reg_data(regDataColor[0], regDataColor[1], regDataColor[2],
                        regDataColor[3]);
          for (int i = 0; i < 60; i++) {
            wait_for_vsync();
          }
          clean_reg_data_display();
          if (shiftLR[operation_index] == 1) {
            regData[sourceReg1] = regData[sourceReg1]
                                  << shiftValue[operation_index];
          } else {
            regData[sourceReg1] =
                regData[sourceReg1] >> shiftValue[operation_index];
          }
          highlight_destReg(sourceReg1);
          draw_reg_data(regDataColor[0], regDataColor[1], regDataColor[2],
                        regDataColor[3]);
          for (int i = 0; i < 60; i++) {
            wait_for_vsync();
          }
        }
        memCodeColor[operation_index] = 0;
        memDataColor[address_selected] = 0;
        regDataColor[sourceReg1] = 0;
      }
      clear_screen();
      draw_main_screen();
      draw_memory_address();
      draw_memory_data();
      draw_reg_data(regDataColor[0], regDataColor[1], regDataColor[2],
                    regDataColor[3]);
    } else {
      /////////when button pressed is 10 or 11, which is load a imm to the reg
      /// or memory
      clear_screen();
      draw_main_screen();
      draw_memory_address();
      operation_index = 0;
      operation_indication_display();
      draw_memory_data();
      draw_reg_data(regDataColor[0], regDataColor[1], regDataColor[2],
                    regDataColor[3]);
    }
    reset_for_new_inputs();
    while (go_back_editor == 0) {
      editor_Keyboard_input();
    }
    go_back_editor = 0;
  }
}

//// Main Screen ////
void wait_for_vsync() {
  *(pixel_ctrl_ptr) = 1;
  int i = *(pixel_ctrl_ptr + 3);
  while ((i & 1) != 0) {
    i = *(pixel_ctrl_ptr + 3);
  }
  return;
}
void plot_pixel(int x, int y, short int line_color) {
  volatile short int* one_pixel_address;
  one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
  *one_pixel_address = line_color;
}
void clear_screen() {
  for (int i = 0; i < 320; i++) {
    for (int j = 0; j < 240; j++) {
      plot_pixel(i, j, 0xffff);
    }
  }
}
void swap(int* a, int* b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}
void draw_line(int x0, int y0, int x1, int y1, short int color) {
  bool is_steep = abs(y1 - y0) > abs(x1 - x0);
  if (is_steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }
  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }
  int deltax = x1 - x0;
  int deltay = abs(y1 - y0);
  int error = -(deltax / 2);
  int y = y0;
  int y_step = 0;
  if (y0 > y1) {
    y_step = -1;
  } else {
    y_step = 1;
  }
  for (int x = x0; x <= x1; x++) {
    if (is_steep) {
      plot_pixel(y, x, color);
    } else {
      plot_pixel(x, y, color);
    }
    error = error + deltay;
    if (error > 0) {
      y += y_step;
      error -= deltax;
    }
  }
  return;
}
void draw_reg_data(short int c0, short int c1, short int c2, short int c3) {
  draw_integer(140, 30, regData[0], regDataColor[0]);
  draw_integer(140, 70, regData[1], regDataColor[1]);
  draw_integer(140, 110, regData[2], regDataColor[2]);
  draw_integer(140, 150, regData[3], regDataColor[3]);
}
void draw_circle(int x_c, int y_c, int r, int color) {
  int x = 0, y = r;
  int p = 3 - 2 * r;

  while (x <= y) {
    // Plot the 8 symmetric points
    plot_pixel(x_c + x, y_c + y, color);
    plot_pixel(x_c - x, y_c + y, color);
    plot_pixel(x_c + x, y_c - y, color);
    plot_pixel(x_c - x, y_c - y, color);
    plot_pixel(x_c + y, y_c + x, color);
    plot_pixel(x_c - y, y_c + x, color);
    plot_pixel(x_c + y, y_c - x, color);
    plot_pixel(x_c - y, y_c - x, color);

    x++;

    // Update the decision parameter
    if (p < 0) {
      p = p + 4 * x + 6;
    } else {
      y--;
      p = p + 4 * (x - y) + 10;
    }
  }
}
void draw_main_screen() {
  // Memory
  draw_memory_address();
  draw_memory_data();
  draw_line(5, 20, 95, 20, 0);
  draw_line(5, 180, 95, 180, 0);
  draw_line(5, 20, 5, 180, 0);
  draw_line(95, 20, 95, 180, 0);
  draw_line(5, 200, 95, 200, 0);
  draw_line(5, 200, 5, 120, 0);
  draw_line(95, 200, 95, 120, 0);
  draw_line(5, 40, 95, 40, 0);
  draw_line(5, 60, 95, 60, 0);
  draw_line(5, 80, 95, 80, 0);
  draw_line(5, 100, 95, 100, 0);
  draw_line(5, 120, 95, 120, 0);
  draw_line(5, 140, 95, 140, 0);
  draw_line(5, 160, 95, 160, 0);
  // Registers
  /*R0*/
  draw_line(100, 20, 175, 20, 0);
  draw_line(100, 50, 175, 50, 0);
  draw_line(100, 20, 100, 50, 0);
  draw_line(175, 20, 175, 50, 0);

  /*R1*/
  draw_line(100, 60, 175, 60, 0);
  draw_line(100, 90, 175, 90, 0);
  draw_line(100, 60, 100, 90, 0);
  draw_line(175, 60, 175, 90, 0);

  /*R2*/
  draw_line(100, 100, 175, 100, 0);
  draw_line(100, 130, 175, 130, 0);
  draw_line(100, 100, 100, 130, 0);
  draw_line(175, 100, 175, 130, 0);

  /*R3*/
  draw_line(100, 140, 175, 140, 0);
  draw_line(100, 170, 175, 170, 0);
  draw_line(100, 140, 100, 170, 0);
  draw_line(175, 140, 175, 170, 0);

  // ALU
  /*source data*/
  draw_line(190, 40, 230, 40, 0);
  draw_line(190, 70, 230, 70, 0);
  draw_line(190, 40, 190, 70, 0);
  draw_line(230, 40, 230, 70, 0);

  draw_line(190, 120, 230, 120, 0);
  draw_line(190, 150, 230, 150, 0);
  draw_line(190, 120, 190, 150, 0);
  draw_line(230, 120, 230, 150, 0);
  /*shape*/
  draw_line(240, 40, 240, 70, 0);
  draw_line(240, 120, 240, 150, 0);
  draw_line(265, 75, 265, 115, 0);
  draw_line(240, 40, 265, 75, 0);
  draw_line(240, 150, 265, 115, 0);
  draw_line(240, 70, 250, 100, 0);
  draw_line(240, 120, 250, 100, 0);
  /*line between source data and shape*/
  draw_line(230, 55, 240, 55, 0);
  draw_line(230, 135, 240, 135, 0);
  /*output data*/
  draw_line(275, 80, 315, 80, 0);
  draw_line(275, 110, 315, 110, 0);
  draw_line(275, 80, 275, 110, 0);
  draw_line(315, 80, 315, 110, 0);
  draw_line(265, 95, 275, 95, 0);
  // z bit + n bit
  draw_char(274, 140, 'Z', 0);
  draw_char(294, 140, 'N', 0);
  draw_integer(274, 154, z_bit, 0);
  draw_integer(294, 154, n_bit, 0);
  draw_line(270, 150, 285, 150, 0);
  draw_line(270, 165, 285, 165, 0);
  draw_line(270, 150, 270, 165, 0);
  draw_line(285, 150, 285, 165, 0);
  draw_line(290, 150, 305, 150, 0);
  draw_line(290, 165, 305, 165, 0);
  draw_line(290, 150, 290, 165, 0);
  draw_line(305, 150, 305, 165, 0);
  // PC
  draw_string(100, 180, "PC", 0);
  draw_line(120, 180, 175, 180, 0);
  draw_line(120, 195, 175, 195, 0);
  draw_line(120, 180, 120, 195, 0);
  draw_line(175, 180, 175, 195, 0);
  draw_integer(140, 185, PC, 0);
  // user interface
  draw_string(2, 220, "CLICK B RESTART", 0);
  /*box*/
  draw_line(160, 215, 310, 215, 0);
  draw_line(160, 229, 310, 229, 0);
  draw_line(160, 215, 160, 229, 0);
  draw_line(310, 215, 310, 229, 0);
  // Block Names
  draw_string(5, 10, "MEMORY", 0);
  draw_string(100, 10, "REGISTERS", 0);
  draw_string(240, 10, "ALU", 0);
  draw_string(160, 205, "OPERATION", 0);

  draw_string(103, 30, "R0", 0);
  draw_string(103, 70, "R1", 0);
  draw_string(103, 110, "R2", 0);
  draw_string(103, 150, "R3", 0);
}
unsigned char reverse_bits(unsigned char b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}
void draw_char(int x, int y, char c, int color) {
  int index = -1;

  if (c >= 'A' && c <= 'Z') {
    index = c - 'A';
  } else if (c >= '0' && c <= '9') {
    index = (c - '0') + 26;
  } else if (c == '-') {
    index = 36;  // The '-' sign is at index 36
  }

  if (index == -1) return;  // Ignore unsupported characters

  for (int row = 0; row < 8; row++) {
    unsigned char bitmap = font[index][row];

    // If it's a number (0-9), reverse the bits
    if (c >= '0' && c <= '9') {
      bitmap = reverse_bits(bitmap);
    }

    for (int col = 0; col < 8; col++) {
      if (bitmap & (1 << (7 - col))) {
        plot_pixel(x + col, y + row, color);
      }
    }
  }
}
void draw_string(int x, int y, char* str, int color) {
  while (*str) {
    draw_char(x, y, *str, color);

    // Move x to the next character position
    if (*str == '-')
      x += 8;  // The '-' sign is narrower, so move by 8 pixels
    else
      x += 10;  // Move x to the next character position for letters and
                // numbers

    str++;
  }
}
void draw_integer(int x, int y, int num, int color) {
  char buffer[12];  // Enough to hold any integer including negatives and null
                    // terminator
  sprintf(buffer, "%d", num);  // Convert integer to string
  draw_string(x, y, buffer,
              color);  // Draw the string representation of the integer
}

//// Memory ////
void machCode_decimal_to_binary(int num) {
  for (int i = 7; i >= 0; i--) {  // Loop from highest bit to lowest
    machCode_binary[7 - i] = ((num >> i) & 1) ? '1' : '0';
    ;  // Extract and print each bit
  }
  machCode_binary[8] = '\0';
}
void highlight_addr_content(int address) { memDataColor[address] = 0x07f0; }
void draw_memory_address() {
  int x = 6;
  int y = 30;
  for (int i = 0; i < 8; i++) {
    draw_integer(x, y, i, 0x94A5);
    y += 20;
  }
}
void draw_memory_data() {
  int x = 35;
  int y = 110;
  for (int i = 0; i < 4; i++) {
    draw_integer(x, y, memory_data[i], memDataColor[i]);
    y += 20;
  }
}
void draw_memory_machCode() {
  int x = 15;
  int y = 30;
  for (int i = 0; i < multiInputIndex; i++) {
    draw_string(x, y, multiInputs[i], memCodeColor[i]);
    y += 20;
  }
}
void clean_mem_data() {
  int x = 35;
  int y = 110;
  for (int i = 0; i < 4; i++) {
    draw_integer(x, y, memory_data[i], 0xffff);
    y += 20;
  }
}
void reset_for_new_inputs() {
  if (button_pressed == 10 || button_pressed == 11) {
    operation_index = 0;
    multiInputIndex = 0;
    inputIndex = 0;
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x = 115;
    editor_writing_y = 30;
    draw_cursor(editor_writing_x, editor_writing_y);
    PC = 0;
    Finish_pressed = 0;
    clear_screen();
    return;
  }
  int x = 15;
  int y = 30;
  for (int i = 0; i < multiInputIndex; i++) {
    draw_string(x, y, multiInputs[i], 0xffff);
    y += 20;
  }
  for (int i = 0; i < multiInputIndex; i++) {
    multiInputs[i] = "\0";
  }
  for (int i = 0; i < multiInputIndex; i++) {
    operation[i] = "\0";
  }
  for (int i = 0; i < multiInputIndex; i++) {
    pc_imm_data[i] = 0;
  }
  for (int i = 0; i < inputIndex; i++) {
    machCode_binary[i] = '\0';
  }
  memory_content_draw_y = 30;
  multiInputIndex = 0;
  inputIndex = 0;
  writing_x = 180;
  PC = 0;
  Finish_pressed = 0;
}
//// Registers ////
void highlight_sourceReg(int reg1, int reg2) {
  regDataColor[reg1] = 0xf800;
  if (reg2 == -1) {
    return;
  }
  regDataColor[reg2] = 0xf800;
}
void highlight_destReg(int reg) { regDataColor[reg] = 0x07ff; }
void update_reg_zn_value(int reg1, int reg2, int button_pressed) {
  int new_value = 0;
  if (button_pressed == 1) {  // add
    new_value = regData[reg1] + regData[reg2];
  } else if (button_pressed == 2) {  // sub
    new_value = regData[reg1] - regData[reg2];
  } else if (button_pressed == 3) {  // nand
    new_value = ~(regData[reg1] & regData[reg2]);
  } else if (button_pressed == 4) {  // ori
    new_value = regData[1] | userInput_int;
  } else if (button_pressed == 5) {  // load
    address_selected = regData[reg2];
    content = memory_data[address_selected];
    new_value = content;
  } else if (button_pressed == 6) {  // store
    address_selected = regData[reg2];
    content = regData[reg1];
    new_value = content;
    clean_mem_data();
    memory_data[address_selected] = new_value;
  }
  draw_integer(274, 154, z_bit, 0xFFFF);  // clean z and n bit
  draw_integer(294, 154, n_bit, 0xFFFF);
  if (new_value == 0) {
    z_bit = 1;
  } else {
    z_bit = 0;
  }
  if (new_value < 0) {
    n_bit = 1;
  } else {
    n_bit = 0;
  }
  regData[reg1] = new_value;
  regDataColor[0] = 0;
  regDataColor[1] = 0;
  regDataColor[2] = 0;
  regDataColor[3] = 0;
}
void clean_reg_data_display() {
  for (int i = 135; i < 170; i++) {
    for (int j = 25; j < 45; j++) {
      plot_pixel(i, j, 0xffff);
    }
  }
  for (int i = 135; i < 170; i++) {
    for (int j = 65; j < 85; j++) {
      plot_pixel(i, j, 0xffff);
    }
  }
  for (int i = 135; i < 170; i++) {
    for (int j = 105; j < 125; j++) {
      plot_pixel(i, j, 0xffff);
    }
  }
  for (int i = 135; i < 170; i++) {
    for (int j = 145; j < 165; j++) {
      plot_pixel(i, j, 0xffff);
    }
  }
}

//// ALU ////
void ALU_decimal_to_binary1(int num) {
  for (int i = 7; i >= 0; i--) {  // Loop from highest bit to lowest
    bitwise_input_binary1[7 - i] = ((num >> i) & 1) ? '1' : '0';
  }
  bitwise_input_binary1[8] = '\0';
}
void ALU_decimal_to_binary2(int num) {
  for (int i = 7; i >= 0; i--) {  // Loop from highest bit to lowest
    bitwise_input_binary2[7 - i] = ((num >> i) & 1) ? '1' : '0';
  }
  bitwise_input_binary2[8] = '\0';
}
void ALU_input_display(int reg1, int reg2) {
  if (button_pressed[operation_index] == 3) {  // NAND, show binary input data
    ALU_decimal_to_binary1(regData[reg1]);
    ALU_decimal_to_binary2(regData[reg2]);
    draw_string(191, 50, bitwise_input_binary1, 0x0f09);
    draw_string(191, 130, bitwise_input_binary2, 0x0f09);
    return;
  }
  if (button_pressed[operation_index] == 4) {  // ORI, show binary input data
    ALU_decimal_to_binary1(regData[1]);
    ALU_decimal_to_binary2(userInput_int);
    draw_string(191, 50, bitwise_input_binary1, 0x0f09);
    draw_string(191, 130, bitwise_input_binary2, 0x0f09);
    return;
  }
  draw_integer(210, 50, regData[reg1], 0x0f09);
  draw_integer(210, 130, regData[reg2], 0x0f09);
  return;
}
void ALU_output_display(int reg) {
  if (button_pressed[operation_index] == 3) {
    ALU_decimal_to_binary1(regData[reg]);
    draw_string(240, 90, bitwise_input_binary1, 0x001f);
  } else if (button_pressed[operation_index] == 4) {
    ALU_decimal_to_binary1(regData[1]);
    draw_string(240, 90, bitwise_input_binary1, 0x001f);
  } else {
    draw_integer(285, 90, regData[reg], 0x001F);
  }
  for (int i = 0; i < 60; i++) {
    wait_for_vsync();
  }
  draw_integer(274, 154, z_bit, 0x0c1F);
  draw_integer(294, 154, n_bit, 0x0c1F);
}

//// keyboard ////
void Keyboard_input() {
  volatile int* PS2_ptr = (int*)PS2_BASE;
  int PS2_data, RVALID;
  char byte1 = 0, byte2 = 0, byte3 = 0;
  // PS/2 mouse needs to be reset (must be already plugged in)
  *(PS2_ptr) = 0xFF;  // reset
  while (1) {
    PS2_data = *(PS2_ptr);
    // read the Data register in the PS/2 port
    RVALID = PS2_data & 0x8000;  // extract the RVALID field
    if (RVALID) {
      /* shift the next data byte into the display */
      byte1 = byte2;
      byte2 = byte3;
      byte3 = PS2_data & 0xFF;
      if (byte2 == 0xF0 && byte3 == 0x2D) {
        draw_char(writing_x, writing_y, 'R', 0xFF);
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x2B) {  // 'F'
        draw_char(writing_x, writing_y, 'F', 0xFF);
        Finish_pressed = 1;
        return;
      }
      if (byte2 == 0xF0 && byte3 == 0x45) {
        draw_char(writing_x, writing_y, '0', 0xFF);
        userInput[inputIndex] = '0';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x16) {
        draw_char(writing_x, writing_y, '1', 0xFF);
        userInput[inputIndex] = '1';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x1E) {
        draw_char(writing_x, writing_y, '2', 0xFF);
        userInput[inputIndex] = '2';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x26) {
        draw_char(writing_x, writing_y, '3', 0xFF);
        userInput[inputIndex] = '3';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x25) {  // '4'
        draw_char(writing_x, writing_y, '4', 0xFF);
        userInput[inputIndex] = '4';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x2E) {  // '5'
        draw_char(writing_x, writing_y, '5', 0xFF);
        userInput[inputIndex] = '5';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x36) {  // '6'
        draw_char(writing_x, writing_y, '6', 0xFF);
        userInput[inputIndex] = '6';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x3D) {  // '7'
        draw_char(writing_x, writing_y, '7', 0xFF);
        userInput[inputIndex] = '7';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x3E) {  // '8'
        draw_char(writing_x, writing_y, '8', 0xFF);
        userInput[inputIndex] = '8';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x46) {  // '9'
        draw_char(writing_x, writing_y, '9', 0xFF);
        userInput[inputIndex] = '9';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x4E) {  // '-'
        draw_char(writing_x, writing_y, '-', 0xFF);
        userInput[inputIndex] = '-';
        inputIndex++;
        writing_x += 15;
      }
      if (byte2 == 0xF0 && byte3 == 0x29) {  // space bar
        userInput[inputIndex] = ' ';
        inputIndex++;
        writing_x += 20;
      }
      if (byte2 == 0xF0 && byte3 == 0x31) {  // 'N'
        draw_char(10, 10, 'N', 0xffff);
        which_screen++;
        return;
      }
      if (byte2 == 0xF0 && byte3 == 0x5A) {  // enter
        userInput[inputIndex] = '\0';
        checkButtonPressed();
        translate_input_to_machine_code();  // after this the string
                                            // "machCode_binary" hold the
        if (button_pressed != 10) {         // machine code
          multiInputs[multiInputIndex] = strdup(machCode_binary);
          draw_string(memory_content_draw_x, memory_content_draw_y,
                      multiInputs[multiInputIndex],
                      memCodeColor[multiInputIndex]);
        }
        reset_input();
      }
    }
  }
}
void editor_Keyboard_input() {
  volatile int* PS2_ptr = (int*)PS2_BASE;
  int PS2_data, RVALID;
  char byte1 = 0, byte2 = 0, byte3 = 0;
  // PS/2 mouse needs to be reset (must be already plugged in)
  *(PS2_ptr) = 0xFF;  // reset
  while (1) {
    PS2_data = *(PS2_ptr);
    // read the Data register in the PS/2 port
    RVALID = PS2_data & 0x8000;  // extract the RVALID field
    if (RVALID) {
      /* shift the next data byte into the display */
      byte1 = byte2;
      byte2 = byte3;
      byte3 = PS2_data & 0xFF;
      if (byte2 == 0xF0 && byte3 == 0x2D) {
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, 'R', 0xFF);
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x2B) {  // 'F'
        draw_char(editor_writing_x, editor_writing_y, 'F', 0xFF);
        Finish_pressed = 1;
        editor_writing_x = 115;
        editor_writing_y = 30;
        return;
      }
      if (byte2 == 0xF0 && byte3 == 0x45) {
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '0', 0xFF);
        userInput[inputIndex] = '0';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x16) {
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '1', 0xFF);
        userInput[inputIndex] = '1';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x1E) {
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '2', 0xFF);
        userInput[inputIndex] = '2';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x26) {
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '3', 0xFF);
        userInput[inputIndex] = '3';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x25) {  // '4'
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '4', 0xFF);
        userInput[inputIndex] = '4';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x2E) {  // '5'
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '5', 0xFF);
        userInput[inputIndex] = '5';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x36) {  // '6'
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '6', 0xFF);
        userInput[inputIndex] = '6';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x3D) {  // '7'
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '7', 0xFF);
        userInput[inputIndex] = '7';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x3E) {  // '8'
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '8', 0xFF);
        userInput[inputIndex] = '8';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x46) {  // '9'
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '9', 0xFF);
        userInput[inputIndex] = '9';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x4E) {  // '-'
        if (Enter_pressed == 1) {
          operation_display();
          Enter_pressed = 0;
        }
        draw_char(editor_writing_x, editor_writing_y, '-', 0xFF);
        userInput[inputIndex] = '-';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x29) {  // space bar
        userInput[inputIndex] = ' ';
        inputIndex++;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x += 20;
        draw_cursor(editor_writing_x, editor_writing_y);
      }
      if (byte2 == 0xF0 && byte3 == 0x31) {  // 'N'
        draw_char(10, 10, 'N', 0xffff);
        which_screen++;
        return;
      }
      if (byte2 == 0xF0 && byte3 == 0x32) {  // 'B'
        go_back_editor = 1;
        return;
      }
      if (byte2 == 0xF0 && byte3 == 0x5A) {  // enter
        Enter_pressed = 1;
        erase_cursor(editor_writing_x, editor_writing_y);
        editor_writing_x = 115;
        if (editor_writing_y < 170) editor_writing_y += 15;
        draw_cursor(editor_writing_x, editor_writing_y);
        userInput[inputIndex] = '\0';
        checkButtonPressed();
        translate_input_to_machine_code();
        // after this the string "machCode_binary" hold the machine code
        if (button_pressed != 10) {
          multiInputs[multiInputIndex] = strdup(machCode_binary);
          draw_string(memory_content_draw_x, memory_content_draw_y,
                      multiInputs[multiInputIndex],
                      memCodeColor[multiInputIndex]);
        }
        reset_input();
      }
    }
  }
}

void translate_input_to_machine_code() {
  if (button_pressed[multiInputIndex] == 10) {  // load imm to reg
    int reg_to_be_changed, imm_value;
    sscanf(userInput, "%d %d", &reg_to_be_changed, &imm_value);
    regData[reg_to_be_changed] = imm_value;
    return;
  }
  if (button_pressed[multiInputIndex] == 11) {  // load imm to memory
    int memory_to_be_changed, imm_value;
    sscanf(userInput, "%d %d", &memory_to_be_changed, &imm_value);
    memory_data[memory_to_be_changed] = imm_value;
    return;
  }
  if (button_pressed[multiInputIndex] == 4) {
    sourceReg1 = 1;
    sourceReg2 = -1;
  } else if (button_pressed[multiInputIndex] == 9) {
    sourceReg1 = userInput[0] - '0';
    sourceReg2 = -1;
    shiftLR[multiInputIndex] = userInput[2] - '0';
    shiftValue[multiInputIndex] = userInput[4] - '0';
  } else if (button_pressed[multiInputIndex] == 7 ||
             button_pressed[multiInputIndex] == 8) {
    sourceReg1 = -1;
    sourceReg2 = -1;
  } else {
    sscanf(userInput, "%d %d", &sourceReg1, &sourceReg2);
  }
  reg1[multiInputIndex] = sourceReg1;
  reg2[multiInputIndex] = sourceReg2;
  if (button_pressed[multiInputIndex] == 4 ||
      button_pressed[multiInputIndex] == 7 ||
      button_pressed[multiInputIndex] == 8) {  // ori, bnz, bpz
    userInput_int = atoi(userInput);
    if (button_pressed[multiInputIndex] == 7 ||
        button_pressed[multiInputIndex] == 8) {
      pc_imm_data[multiInputIndex] = userInput_int;
    }
  }
  // the string machCode_binary hold the machine code of the userInput being
  // passed
  int machine_code = -1;
  if (button_pressed[multiInputIndex] == 1) {
    machine_code = 0b0100;
  } else if (button_pressed[multiInputIndex] == 2) {
    machine_code = 0b0110;
  } else if (button_pressed[multiInputIndex] == 3) {
    machine_code = 0b1000;
  } else if (button_pressed[multiInputIndex] == 4) {
    machine_code = (userInput_int << 3) + 0b111;
    machCode_decimal_to_binary(machine_code);
    return;
  } else if (button_pressed[multiInputIndex] == 5) {
    machine_code = 0;
  } else if (button_pressed[multiInputIndex] == 6) {
    machine_code = 0b0010;
  } else if (button_pressed[multiInputIndex] == 7) {
    machine_code = (userInput_int << 4) + 0b1001;
    machCode_decimal_to_binary(machine_code);
    return;
  } else if (button_pressed[multiInputIndex] == 8) {
    machine_code = (userInput_int << 4) + 0b1101;
    machCode_decimal_to_binary(machine_code);
    return;
  } else if (button_pressed[multiInputIndex] == 9) {
    machine_code = (sourceReg1 << 6) + (shiftLR[multiInputIndex] << 5) +
                   (shiftValue[multiInputIndex] << 3) + 0b011;
    machCode_decimal_to_binary(machine_code);
    return;
  }
  machine_code = (sourceReg1 << 6) + (sourceReg2 << 4) + machine_code;
  machCode_decimal_to_binary(machine_code);
  return;
}
void reset_input() {
  if (button_pressed[multiInputIndex] == 10 ||
      button_pressed[multiInputIndex] == 11) {  // load imm to reg
    /*for (int i = 0; i < inputIndex; i++) {
      machCode_binary[i] = '\0';
    }*/
    for (int i = 151; i < 305; i++) {
      for (int j = 216; j < 228; j++) {
        plot_pixel(i, j, 0xffff);
      }
    }
    userInput[15] = "\0";
    inputIndex = 0;
    writing_x = 180;
    return;
  }
  for (int i = 0; i < inputIndex; i++) {
    machCode_binary[i] = '\0';
  }
  for (int i = 151; i < 305; i++) {
    for (int j = 216; j < 228; j++) {
      plot_pixel(i, j, 0xffff);
    }
  }
  userInput[15] = "\0";
  memory_content_draw_y += 20;
  multiInputIndex++;
  inputIndex = 0;
  writing_x = 180;
  return;
}
////Swithches
void checkButtonPressed() {
  switchData = *switchPtr;
  if (switchData == 0b1) {
    // draw_string(120, 120, "ADD", 0xFF);
    button_pressed[multiInputIndex] = 1;
  } else if (switchData == 0b10) {
    button_pressed[multiInputIndex] = 2;
  } else if (switchData == 0b100) {
    button_pressed[multiInputIndex] = 3;
  } else if (switchData == 0b1000) {
    button_pressed[multiInputIndex] = 4;
  } else if (switchData == 0b10000) {
    button_pressed[multiInputIndex] = 5;
  } else if (switchData == 0b100000) {
    button_pressed[multiInputIndex] = 6;
  } else if (switchData == 0b1000000) {
    button_pressed[multiInputIndex] = 7;
  } else if (switchData == 0b10000000) {
    button_pressed[multiInputIndex] = 8;
  } else if (switchData == 0b100000000) {
    button_pressed[multiInputIndex] = 9;
  } else if (switchData == 0b1000000000) {
    button_pressed[multiInputIndex] = 10;
  } else if (switchData == 0b1100000000) {
    button_pressed[multiInputIndex] = 11;
  } else {
    button_pressed[multiInputIndex] = -1;
  }

  return;
}
////Start Screen
void plot_image(int x, int y) {
  for (int i = 0; i < 240; i++) {
    for (int j = 0; j < 320; j++) {
      plot_pixel(x + j, y + i, paimon[i * 320 + j]);
    }
  }
}
void draw_instructions_screen() {
  clear_screen();

  draw_string(40, 40, "WELCOME TO PROCESSULATOR", 0x780F);  // Royal Purple
  delay_loop_instructions();

  draw_string(100, 75, "INSTRUCTIONS:", 0xFA60);  // Burnt Orange
  delay_loop_instructions();

  draw_string(2, 105, "1 SELECT OPERATION", 0x03BF);  // Teal Blue
  delay_loop_instructions();

  draw_string(2, 120, "2 TYPE REGISTERS AND IMM VALUE",
              0x8811);  // Emerald Green
  delay_loop_instructions();

  draw_string(2, 135, "3 PRESS ENTER FOR A NEW INPUT",
              0x07E00);  // Deep Magenta
  delay_loop_instructions();

  draw_string(2, 150, "4 PRESS F WHEN FINISHED", 0xB000);  // Crimson Red
  delay_loop_instructions();

  draw_line(56, 180, 261, 180, 0);
  draw_line(56, 200, 261, 200, 0);
  draw_line(56, 180, 56, 200, 0);
  draw_line(261, 180, 261, 200, 0);
  draw_string(80, 185, "PRESS N TO START", 0xFF);
}
void delay_loop_instructions() {
  volatile long i;
  for (i = 0; i < 250000; i++);  // Adjust based on CPU speed
}
void draw_cursor(int writing_x, int writing_y) {
  draw_line(writing_x - 2, writing_y, writing_x - 2, writing_y + 10, 0xFF);
}
void draw_editor_screen_initial() {
  draw_string(260, 220, "EDITOR", 0xFF);
  draw_string(110, 10, "USER INPUT", 0x0);
  draw_line(110, 20, 310, 20, 0xFF);
  draw_line(110, 200, 310, 200, 0xff);
  draw_line(110, 20, 110, 200, 0xff);
  draw_line(310, 20, 310, 200, 0xff);
  draw_string(5, 10, "MEMORY", 0);
  draw_line(5, 20, 95, 20, 0);
  draw_line(5, 180, 95, 180, 0);
  draw_line(5, 20, 5, 180, 0);
  draw_line(95, 20, 95, 180, 0);
  draw_line(5, 200, 95, 200, 0);
  draw_line(5, 200, 5, 120, 0);
  draw_line(95, 200, 95, 120, 0);
  draw_line(5, 40, 95, 40, 0);
  draw_line(5, 60, 95, 60, 0);
  draw_line(5, 80, 95, 80, 0);
  draw_line(5, 100, 95, 100, 0);
  draw_line(5, 120, 95, 120, 0);
  draw_line(5, 140, 95, 140, 0);
  draw_line(5, 160, 95, 160, 0);
  draw_cursor(115, 30);
}
void erase_cursor(int writing_x, int writing_y) {
  draw_line(writing_x - 2, writing_y, writing_x - 2, writing_y + 10, 0xFFFFFF);
}
void operation_display() {
  switchData = *switchPtr;
  *LEDPtr = switchData;
  if (switchData == 0b1) {
    operation[multiInputIndex] = "ADD";
    draw_string(editor_writing_x, editor_writing_y, "ADD", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);
  } else if (switchData == 0b10) {
    operation[multiInputIndex] = "SUB";
    draw_string(editor_writing_x, editor_writing_y, "SUB", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);

  } else if (switchData == 0b100) {
    operation[multiInputIndex] = "NAND";
    draw_string(editor_writing_x, editor_writing_y, "NAND", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);

  } else if (switchData == 0b1000) {
    operation[multiInputIndex] = "ORI";
    draw_string(editor_writing_x, editor_writing_y, "ORI", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);

  } else if (switchData == 0b10000) {
    operation[multiInputIndex] = "LOAD";
    draw_string(editor_writing_x, editor_writing_y, "LOAD", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);

  } else if (switchData == 0b100000) {
    operation[multiInputIndex] = "STORE";
    draw_string(editor_writing_x, editor_writing_y, "STORE", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);

  } else if (switchData == 0b1000000) {
    operation[multiInputIndex] = "BNZ";
    draw_string(editor_writing_x, editor_writing_y, "BNZ", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);

  } else if (switchData == 0b10000000) {
    operation[multiInputIndex] = "BPZ";
    draw_string(editor_writing_x, editor_writing_y, "BPZ", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);
  } else if (switchData == 0b100000000) {
    operation[multiInputIndex] = "SHIFT";
    draw_string(editor_writing_x, editor_writing_y, "SHIFT", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);
  } else if (switchData == 0b1000000000) {
    operation[0] = "LIR";
    draw_string(editor_writing_x, editor_writing_y, "LIR", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);
  } else if (switchData == 0b1100000000) {
    operation[0] = "LIM";
    draw_string(editor_writing_x, editor_writing_y, "LIM", 0xFF);
    erase_cursor(editor_writing_x, editor_writing_y);
    editor_writing_x += 50;
    draw_cursor(editor_writing_x, editor_writing_y);
  }
  return;
}
////operation screen
void operation_indication_display() {
  draw_string(writing_x, writing_y, operation[operation_index], 0xFA60);

}
