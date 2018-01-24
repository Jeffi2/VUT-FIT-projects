/*******************************************************************************
   original_file: (user_main: main for application using keyboard with interrupt)
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): (original)Jan Markovic <xmarko04 AT stud.fit.vutbr.cz>
              (modifications) Petr Jůda <xjudap00 AT stud.fit.vutbr.cz>
   
   Date: 21-12-2017
   FPGA code ununtouched. (Original from: demo application using keyboard with interrupt)
   MCU code (main.c) changed approximately 75%. 
   Added JP9 pin setup for external 8x8 LED display. Implemented "Game Life" algorithem.
   After press 1-4 buttons on FITkit is possible to change init state. "*" button cause
   pause of program and "0" button step forward. Added timer interrupt for clocking of the game.

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/

#include <fitkitlib.h>
#include <irq/fpga_interrupt.h>
#include <keyboard/keyboard.h>

#define MATRIX_SIZE 8

// debug mode flag
int pause = 0;

// main game matrix
int gameMatrix[MATRIX_SIZE][MATRIX_SIZE] = {
 {0,0,0,0,0,0,0,0},
 {0,1,1,0,0,0,0,0},
 {0,1,1,0,0,0,0,0},
 {0,0,0,1,1,0,0,0},
 {0,0,0,1,1,0,0,0},
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,1,1,1},
 {0,0,0,0,0,0,0,0},
}; 

// matrix for neighbors count
int nextMatrix[MATRIX_SIZE][MATRIX_SIZE] = {
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0},
 }; 

/*******************************************************************************
 * Print help (function called after "help" command in terminal)
*******************************************************************************/
void print_user_help(void)
{
  term_send_str_crlf("User help:\r\n For pause/run program press \'*\'\r\n For step forwarc pause program and press \'0\'");
}

/*******************************************************************************
 * Copy matrix from source to target
*******************************************************************************/
void copyMatrix(int source[MATRIX_SIZE][MATRIX_SIZE], int target[MATRIX_SIZE][MATRIX_SIZE]) {
  int i = 0;
  int j = 0;
  for (i = 0; i < MATRIX_SIZE; i++)
  {
    for (j = 0; j < MATRIX_SIZE; j++)
    {
      target[i][j] = source[i][j];
    }
  }
}

/*******************************************************************************
 * Cyclic edges
*******************************************************************************/
int testBorders(int val) {
  if (val == -1)
  {
    return 7;
  } else if (val == 8)
  {
    return 0;
  }
  return val;
}

/*******************************************************************************
 * Count neighbors
*******************************************************************************/
void checkNeighbors() {
  int l;
  int m;
  int i;
  int j;
  int liveNeighbors = 0;
  for (i = 0; i < MATRIX_SIZE; i++)
  {
    for (j = 0; j < MATRIX_SIZE; j++)
    {
      nextMatrix[i][j] = 0;
    }
  }

  for (i = 0; i < MATRIX_SIZE; i++)
  {
    for (j = 0; j < MATRIX_SIZE; j++)
    {
      liveNeighbors = 0;
      for (l = -1; l < 2; l++)
      {
        for (m = -1; m < 2; m++)
        {
          if (gameMatrix[testBorders(i+l)][testBorders(j+m)] == 1)
          {
            if ((l == 0) && (m ==0))
            {
              continue;
            }
            liveNeighbors++;
          }
        }
      }
      nextMatrix[i][j] = liveNeighbors;
    }
  }
}

/*******************************************************************************
 * Create next game array
*******************************************************************************/
void createNextState() {
  checkNeighbors();
  int i = 0;
  int j = 0;

  for (i = 0; i < MATRIX_SIZE; i++)
  {
    for (j = 0; j < MATRIX_SIZE; j++)
    { 
      // current live
      if (gameMatrix[i][j] == 1)
      {
        if (nextMatrix[i][j] < 2)
        {
          gameMatrix[i][j] = 0;
        }
        else if (nextMatrix[i][j] > 3)
        {
          gameMatrix[i][j] = 0;
        }
        else 
        { 
          gameMatrix[i][j] = 1;
        }
      }
      // current dead
      else
      {
        if (nextMatrix[i][j] == 3)
        {
          // reborn
          gameMatrix[i][j] = 1;
        }
        else
        {
          // still dead
          gameMatrix[i][j] = 0;
        }
      }
    }
  }
}

/*******************************************************************************
 * Handle keyboard interrupt
*******************************************************************************/
void keyboard_idle()
{
  int i = 0;
  int j = 0;
  unsigned char ch;
  if ((ch = keyboard_get_char())!=0)
    {
      term_send_crlf();
      term_send_str("Na klavesnici byla zmacnuta klavesa \'");
      term_send_char(ch);
      term_send_char('\'');
      term_send_crlf();
      term_send_str(" >");
    }

  if (ch == '1')
  {
    pause = 0;
    int tmp[8][8] = {
    {0,1,0,0,0,0,0,1},
    {1,0,0,1,0,0,0,0},
    {1,0,0,1,1,0,0,1},
    {0,0,0,0,0,0,0,1},
    {0,0,1,0,0,0,0,0},
    {1,0,1,1,0,0,0,0},
    {1,1,0,1,1,1,0,0},
    {1,1,0,0,0,1,0,0},
    }; 
    copyMatrix(&tmp, &gameMatrix);
  }

  if (ch == '2')
  {
    pause = 0;
    int tmp[8][8] = {
    {0,0,0,1,0,0,0,1},
    {0,0,1,0,0,1,0,0},
    {0,1,0,1,1,0,1,0},
    {1,0,0,0,0,1,0,1},
    {0,1,0,1,1,0,1,0},
    {1,0,0,0,0,1,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,0,0,0,0},
    }; 
    copyMatrix(&tmp, &gameMatrix);
  }

  if (ch == '3')
  {
    pause = 0;
    int tmp[8][8] = {
    {0,0,0,0,0,0,0,0},
    {1,0,1,0,0,0,0,0},
    {0,1,1,0,0,0,1,0},
    {0,1,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,0,1,1,0,0,1},
    {0,0,0,0,0,1,1,1},
    };  
    copyMatrix(&tmp, &gameMatrix);
  }

  if (ch == '4')
  {
    pause = 0;
    int tmp[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    }; 
    copyMatrix(&tmp, &gameMatrix);
  }

  // Debug mode
  if (ch == '*') {
    if (pause == 0) {
      pause = 1;
    }
    else {
      pause = 0;
    }
  }

  if (ch == '0' && (pause == 1)) {
    pause = 2;
  }
}

/*******************************************************************************
 * Initialization of periferies for programming FPGA
*******************************************************************************/
void fpga_initialized()
{
  term_send_str_crlf("IRQ Init");
  fpga_interrupt_init(BIT2);
}

/*******************************************************************************
 * Decode and do users commands
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

/*******************************************************************************
 * Handle interrupt from FPGA
*******************************************************************************/
void fpga_interrupt_handler(unsigned char bits)
{
  if (bits & BIT2)
    keyboard_handle_interrupt(); 
}

/*******************************************************************************
 * Init functions (register JP9 cleaning)
*******************************************************************************/
void clearRow() {
  P6OUT = 0x00;
  P6DIR = 0xFF;
}

void clearCol() {
  P4OUT &= 0xC0;
  P4DIR &= 0xC0;
  P2OUT &= 0x3F;
  P2DIR &= 0x3F;
}

void initJP9() {
  clearRow();
  clearCol();
}

/*******************************************************************************
 * Create col output
*******************************************************************************/
void setCol(int row) {
  int i = 0;
  for (i = 0; i < MATRIX_SIZE; i++)
  {
    if (gameMatrix[row][i] == 0)
    {
      switch(i) {
          case 0:
            P6OUT |= BIT0; 
            break;
          case 1:
            P6OUT |= BIT1;
            break;
          case 2:
            P6OUT |= BIT2;
            break;
          case 3:
            P6OUT |= BIT3;
            break;
          case 4:
            P6OUT |= BIT4;
            break;
          case 5:
            P6OUT |= BIT5;
            break;
          case 6:
            P6OUT |= BIT6;
            break;
          case 7:
            P6OUT |= BIT7;
            break;
          default:
            break;
        } 
    }
  }
}

/*******************************************************************************
 * Print game to LED display
*******************************************************************************/
void printGame() {
  int i = 0;
  for (i = 0; i < MATRIX_SIZE; i++)
  {
    initJP9();
    setCol(i);
    switch(i) {
      case 0:
        P4DIR |= BIT0;
        P4OUT |= BIT0;
        break;
      case 1:
        P4DIR |= BIT1;
        P4OUT |= BIT1;
        break;
      case 2:
        P4DIR |= BIT2;
        P4OUT |= BIT2;
        break;
      case 3:
        P4DIR |= BIT3;
        P4OUT |= BIT3;
        break;
      case 4:
        P4DIR |= BIT4;
        P4OUT |= BIT4;
        break;
      case 5:
        P4DIR |= BIT5;
        P4OUT |= BIT5;
        break;
      case 6:
        P2DIR |= BIT6;
        P2OUT |= BIT6;
        break;
      case 7:
        P2DIR |= BIT7;
        P2OUT |= BIT7;
        break;
      default:
        break;
        }
  delay_ms(1);
  }
}

/*******************************************************************************
 * Handle timer A0 interrupt
*******************************************************************************/
// TIMER interrupt handler - COPY from "interrupt service routine (ISR) of timer A0 utilized for LED control"
// Author: Josef Strnadel <strnadel AT stud.fit.vutbr.cz>
interrupt (TIMERA0_VECTOR) Timer_A (void)
{
  flip_led_d6();        // flip led d6
  if (pause == 2) {     // debug (step mode)
    flip_led_d5();      // flip led d5
    createNextState();  // create new game state
    pause = 1;  
  }
  else if (pause == 0)
  {
    flip_led_d5();      // flip led d5
    createNextState();  // create new game state
  }
  CCR0 += 0x4000;       // set next interrupt tact (16384 = 0x4000, that is 1/2 s)
}

/*******************************************************************************
 * Main functions
*******************************************************************************/
int main(void)
{
  // Init HW and keyboard
  initialize_hardware();
  keyboard_init();

  // Init JP9 registers
  initJP9();

  // Turn on debug diods
  set_led_d6(1);                       
  set_led_d5(1);    

  // TIMER INITIALIZATION - COPY from "interrupt service routine (ISR) of timer A0 utilized for LED control"
  // Author: Josef Strnadel <strnadel AT stud.fit.vutbr.cz>
  // allow Timer A0 interrupt (rezim vystupni komparace) 
  CCTL0 = CCIE;    
  // set interrupt tact (16384 = 0x4000, that is 1/2 s)                         
  CCR0 = 0x4000;      
  // ACLK (f_tiku = 32768 Hz = 0x8000 Hz), nepretrzity rezim                      
  TACTL = TASSEL_1 + MC_2;                               

  // Main CPU loop
  while (1)
  {              
    // Print gameMatrix to LED display
    printGame();

    // Handle terminal and keyboard interrupt
    terminal_idle();                   
    keyboard_idle();                   
  }         
}
