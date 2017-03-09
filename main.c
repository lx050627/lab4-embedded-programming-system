/********************************************************
 Name          : main.c
 Author        : Xin Liu
 Copyright     : Not really
 Description   : EVK1100 template
 **********************************************************/

// Include Files


#include "board.h"
#include "compiler.h"
#include "preprocessor.h"
#include "gpio.h"
#include "pm.h"
#include "delay.h"
#include "spi.h"
#include <avr32/io.h>
#include "adc.h"
#include "intc.h"
#include "tc.h"
#include "dip204.h"
#include "flashc.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"
#include "sd_mmc_spi_mem.h"
#include "conf_sd_mmc_spi.h"
#include "fat.h"
#include "file.h"
#include "navigation.h"
#include "ctrl_access.h"
#include "usart.h"
#include "nlao_cpu.h"
#include "nlao_usart.h"
#include "conf_usb.h"
#include "usb_task.h"
#include "device_cdc_task.h"
#include "cdc.h"
#include "uart_usb_lib.h"
#include "usb_descriptors.h"
#include "usb_device_task.h"
#include <stdio.h>
#include <string.h>




#  define EXAMPLE_ADC_POTENTIOMETER_CHANNEL   1
#  define EXAMPLE_ADC_POTENTIOMETER_PIN       AVR32_ADC_AD_1_PIN
#  define EXAMPLE_ADC_POTENTIOMETER_FUNCTION  AVR32_ADC_AD_1_FUNCTION
#  define EXAMPLE_TC                  (&AVR32_TC)
#  define EXAMPLE_TC_IRQ_GROUP        AVR32_TC_IRQ_GROUP
#  define EXAMPLE_TC_IRQ              AVR32_TC_IRQ0
#  define TC_CHANNEL 0

#  define CPUf 60000000
#  define PBAf 12000000
# define APPLI_CPU_SPEED   60000000
# define APPLI_PBA_SPEED   12000000
pcl_freq_param_t pcl_freq_param =
{
  .cpu_f        = APPLI_CPU_SPEED,
  .pba_f        = APPLI_PBA_SPEED,
  .osc0_f       = FOSC0,
  .osc0_startup = OSC0_STARTUP
};

#ifndef FREERTOS_USED

  #if __GNUC__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 *
 * This version comes in replacement to the default one provided by the Newlib
 * add-ons library.
 * Newlib add-ons' _init_startup only calls init_exceptions, but Newlib add-ons'
 * exception and interrupt vectors are defined in the same section and Newlib
 * add-ons' interrupt vectors are not compatible with the interrupt management
 * of the INTC module.
 * More low-level initializations are besides added here.
 */
int _init_startup(void)
{
  // Import the Exception Vector Base Address.
  extern void _evba;

  // Load the Exception Vector Base Address in the corresponding system register.
  Set_system_register(AVR32_EVBA, (int)&_evba);

  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // Initialize the USART used for the debug trace with the configured parameters.
  set_usart_base( ( void * ) DBG_USART );

  // Don't-care value for GCC.
  return 1;
}

  #elif __ICCAVR32__

/*! \brief Low-level initialization routine called during startup, before the
 *         main function.
 */
int __low_level_init(void)
{
  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();

  // Initialize the USART used for the debug trace with the configured parameters.
  extern volatile avr32_usart_t *volatile stdio_usart_base;
  stdio_usart_base = DBG_USART;

  // Request initialization of data segments.
  return 1;
}

  #endif  // Compiler

#endif  // FREERTOS_USED


unsigned short value=0;
volatile int ci=0;
volatile avr32_adc_t *adc = &AVR32_ADC; // ADC IP registers address

       // Assign the on-board sensors to their ADC channel.
unsigned short adc_channel_pot = EXAMPLE_ADC_POTENTIOMETER_CHANNEL;
//*******************************************************
#if BOARD == EVK1100
#  define SHL_USART               (&AVR32_USART1)
#  define SHL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1101
#  define SHL_USART               (&AVR32_USART1)
#  define SHL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1104
#  define SHL_USART               (&AVR32_USART1)
#  define SHL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1105
#  define SHL_USART               (&AVR32_USART0)
#  define SHL_USART_RX_PIN        AVR32_USART0_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART0_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART0_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART0_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == UC3C_EK
#  define SHL_USART               (&AVR32_USART2)
#  define SHL_USART_RX_PIN        AVR32_USART2_RXD_0_1_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART2_RXD_0_1_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART2_TXD_0_1_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART2_TXD_0_1_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#endif

#if !defined(SHL_USART)             || \
    !defined(SHL_USART_RX_PIN)      || \
    !defined(SHL_USART_RX_FUNCTION) || \
    !defined(SHL_USART_TX_PIN)      || \
    !defined(SHL_USART_TX_FUNCTION) || \
    !defined(SHL_USART_BAUDRATE)
#  error The USART configuration to use in this example on your board is missing.
#endif
//! @}

/*! \name Shell Commands
 */
//! @{
#define CMD_NONE              0x00
#define CMD_MOUNT             0x01
#define CMD_LS                0x02
#define CMD_CD                0x03
#define CMD_CAT               0x04
#define CMD_HELP              0x05
#define CMD_MKDIR             0x06
#define CMD_TOUCH             0x07
#define CMD_RM                0x08
#define CMD_APPEND            0x09
#define CMD_NB_DRIVE          0x0A
#define CMD_SET_ID            0x0B
#define CMD_GOTO_ID           0x0C
#define CMD_CP                0x0D
#define CMD_DF                0x0E
#define CMD_MV                0x0F
#define CMD_FORMAT            0x10
#define CMD_FAT               0x11
#define CMD_FORMAT32          0x12
#define CMD_START             0x13
#define CMD_STOP              0x14
//! @}

/*! \name Special Char Values
 */
//! @{
#define CR                    '\r'
#define LF                    '\n'
#define CTRL_C                0x03
#define CTRL_Q                0x11
#define BKSPACE_CHAR          '\b'
#define ABORT_CHAR            CTRL_C
#define QUIT_APPEND           CTRL_Q
#define HISTORY_CHAR          '!'
//! @}

/*! \name String Values for Commands
 */
//! @{
#define STR_CD                "cd"
#define STR_MOUNT             "mount"
#define STR_CP                "cp"
#define STR_LS                "ls"
#define STR_RM                "rm"
#define STR_DF                "df"
#define STR_MKDIR             "mkdir"
#define STR_TOUCH             "touch"
#define STR_APPEND            "append"
#define STR_CAT               "cat"
#define STR_DISK              "disk"
#define STR_MARK              "mark"
#define STR_GOTO              "goto"
#define STR_MV                "mv"
#define STR_A                 "a:"
#define STR_B                 "b:"
#define STR_C                 "c:"
#define STR_D                 "d:"
#define STR_HELP              "help"
#define STR_FORMAT            "format"
#define STR_FORMAT32          "format32"
#define STR_FAT               "fat"
#define STR_START             "start"
#define STR_STOP              "stop"
//! @}

/*! \name String Messages
 */
//! @{
#define MSG_PROMPT            "$>"
#define MSG_WELCOME           "\n" \
                              "-------------------------\n" \
                              "    ATMEL AVR32 Shell\n" \
                              "-------------------------\n"
#define MSG_ER_CMD_NOT_FOUND  "Command not found\n\r"
#define MSG_ER_MOUNT          "Unable to mount drive\n"
#define MSG_ER_DRIVE          "Drive does not exist\n"
#define MSG_ER_RM             "Can not erase; if the name is a directory, check it is empty\n\r"
#define MSG_ER_UNKNOWN_FILE   "Unknown file\n\r"
#define MSG_ER_MV             "Error during move\n"
#define MSG_ER_FORMAT         "Format fails\n"
#define MSG_APPEND_WELCOME    "\nSimple text editor, enter char to append, ^q to exit and save\n\r"
#define MSG_HELP              "Commands summary\n" \
                              " a:, b:... goto selected drive               mount disk(a, b...)\n\r" \
                              " cd dirname                                  ls\n\r" \
                              " mkdir dirname                               cat filename\n\r" \
                              " touch filename                              disk: get number of drives\n\r" \
                              " append filename                             goto: goto bookmark\n\r" \
                              " mark: bookmark current directory            df: get free space information\n\r" \
                              " cp filename: copy filename to bookmark      fat: get FAT type for current drive\n\r" \
                              " rm filename: erase file or EMPTY directory  format drivename, with drivename: a, b...\n\r" \
                              " mv src dst: move file or directory          format32 drivename, with drivename: a, b...\n\r" \
                              " help\n\r"
//! @}


//_____ D E C L A R A T I O N S ____________________________________________
//! flag for a command presence
static Bool cmd;
//! command number
static U8   cmd_type;
//! flag for first ls : mount if set
static Bool first_ls;
//! string length
static U8   i_str = 0;

static Bool write=FALSE;

static Bool adcnew=FALSE;

static Bool flag=TRUE;

//! string for command
static char cmd_str[10 + 2 * MAX_FILE_PATH_LENGTH];
//! string for first arg
static char par_str1[MAX_FILE_PATH_LENGTH];
//! string for second arg
static char par_str2[MAX_FILE_PATH_LENGTH];

//! buffer for command line
static char str_buff[MAX_FILE_PATH_LENGTH];


//_____ D E F I N I T I O N S ______________________________________________

/*! \brief Decodes full command line into command type and arguments.
 *
 * This function allows to set the \ref cmd_type variable to the command type
 * decoded with its respective arguments \ref par_str1 and \ref par_str2.
 */
static void PrintString(char* s)
{
    for(;*s!='\0';s++)
    {
    	uart_usb_putchar(*s);
    }
    uart_usb_flush();
}

static void PrintNumber(unsigned long n)
{
    char tmp[11];
    int i = sizeof(tmp) - 1;
    // Convert the given number to an ASCII decimal representation.
    tmp[i] = '\0';
    do
    {
        tmp[--i] = '0' + n % 10;
        n /= 10;
    } while (n);

    PrintString(tmp+i);
}



static void fat_example_parse_cmd(void)
{
  U8 i, j;

  // Get command type.
  for (i = 0; cmd_str[i] != ' ' && i < i_str; i++);

  if (i)
  {
    cmd = TRUE;
    // Save last byte
    j = cmd_str[i];
    // Reset vars
    cmd_str[i] = '\0';
    par_str1[0] = '\0';
    par_str2[0] = '\0';

    // Decode command type.
    if      (!strcmp(cmd_str, STR_CD      )) cmd_type = CMD_CD;
    else if (!strcmp(cmd_str, STR_MOUNT   )) cmd_type = CMD_MOUNT;
    else if (!strcmp(cmd_str, STR_FAT     )) cmd_type = CMD_FAT;
    else if (!strcmp(cmd_str, STR_CP      )) cmd_type = CMD_CP;
    else if (!strcmp(cmd_str, STR_LS      )) cmd_type = CMD_LS;
    else if (!strcmp(cmd_str, STR_RM      )) cmd_type = CMD_RM;
    else if (!strcmp(cmd_str, STR_DF      )) cmd_type = CMD_DF;
    else if (!strcmp(cmd_str, STR_MKDIR   )) cmd_type = CMD_MKDIR;
    else if (!strcmp(cmd_str, STR_TOUCH   )) cmd_type = CMD_TOUCH;
    else if (!strcmp(cmd_str, STR_APPEND  )) cmd_type = CMD_APPEND;
    else if (!strcmp(cmd_str, STR_CAT     )) cmd_type = CMD_CAT;
    else if (!strcmp(cmd_str, STR_DISK    )) cmd_type = CMD_NB_DRIVE;
    else if (!strcmp(cmd_str, STR_MARK    )) cmd_type = CMD_SET_ID;
    else if (!strcmp(cmd_str, STR_GOTO    )) cmd_type = CMD_GOTO_ID;
    else if (!strcmp(cmd_str, STR_MV      )) cmd_type = CMD_MV;
    else if (!strcmp(cmd_str, STR_A       )) cmd_type = CMD_MOUNT, par_str1[0] = 'a', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_B       )) cmd_type = CMD_MOUNT, par_str1[0] = 'b', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_C       )) cmd_type = CMD_MOUNT, par_str1[0] = 'c', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_D       )) cmd_type = CMD_MOUNT, par_str1[0] = 'd', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_HELP    )) cmd_type = CMD_HELP;
    else if (!strcmp(cmd_str, STR_FORMAT  )) cmd_type = CMD_FORMAT;
    else if (!strcmp(cmd_str, STR_FORMAT32)) cmd_type = CMD_FORMAT32;
    else if (!strcmp(cmd_str, STR_START))    cmd_type = CMD_START;
    else if (!strcmp(cmd_str, STR_STOP))     cmd_type = CMD_STOP;
    else
    {
      // error : command not found
      //print(SHL_USART, MSG_ER_CMD_NOT_FOUND);
      PrintString(MSG_ER_CMD_NOT_FOUND);
      cmd = FALSE;
    }
    // restore last byte
    cmd_str[i] = j;
  }
  // if command isn't found, display prompt
  if (!cmd)
  {
    //print(SHL_USART, MSG_PROMPT);
    uart_usb_putchar('$');
    uart_usb_putchar('>');
    uart_usb_flush();
    return;
  }

  // Get first arg (if any).
  if (++i < i_str)
  {
    j = 0;
    // remove " if used
    if (cmd_str[i] == '"')
    {
      i++;
      for (; cmd_str[i] != '"' && i < i_str; i++, j++)
      {
        par_str1[j] = cmd_str[i];
      }
      i++;
    }
    // get the arg directly
    else
    {
      for(; cmd_str[i] != ' ' && i < i_str; i++, j++)
      {
        par_str1[j] = cmd_str[i];
      }
    }
    // null terminated arg
    par_str1[j] = '\0';
  }

  // Get second arg (if any).
  if (++i < i_str)
  {
    j = 0;
    // remove " if used
    if (cmd_str[i] == '"')
    {
      i++;
      for (; cmd_str[i] != '"' && i < i_str; i++, j++)
      {
        par_str2[j] = cmd_str[i];
      }
      i++;
    }
    // get the arg directly
    else
    {
      for (; cmd_str[i] != ' ' && i < i_str; i++, j++)
      {
        par_str2[j] = cmd_str[i];
      }
    }
    // null terminated arg
    par_str2[j] = '\0';
  }
}


/*! \brief Gets the full command line on RS232 input to be interpreted.
 * The cmd_str variable is built with the user inputs.
 */
static void fat_example_build_cmd(void)
{
  int c;

  //usart_reset_status(SHL_USART);

  if (uart_usb_test_hit())
  {
    c=uart_usb_getchar();

//    if(flag)
//     {
//                	  uart_usb_putchar('$');
//                	  uart_usb_putchar('>');
//                	  flag=FALSE;
//      }



    switch (c)
    {
    case CR:
      // Add LF.
      //print_char(SHL_USART, LF);
      uart_usb_putchar('\n');
      uart_usb_putchar('\r');
      uart_usb_flush();
      // Add NUL char.
      cmd_str[i_str] = '\0';
      // Decode the command.
      fat_example_parse_cmd();
      i_str = 0;
      break;
    // ^c abort cmd.
    case ABORT_CHAR:
      // Reset command length.
      i_str = 0;
      // Display prompt.
      //print(SHL_USART, "\n" MSG_PROMPT);
      uart_usb_putchar('\n');
      uart_usb_putchar('\r');
      uart_usb_putchar('$');
      uart_usb_putchar('>');
      uart_usb_flush();
      break;
    // Backspace.
    case BKSPACE_CHAR:
      if (i_str > 0)
      {
        // Replace last char.
        //print(SHL_USART, "\b \b");
    	 PrintString("\b \b");
        // Decraese command length.
        i_str--;
      }
      break;
    default:
      // Echo.
     // print_char(SHL_USART, c);
      uart_usb_putchar(c);
      uart_usb_flush();
      // Append to cmd line.
      cmd_str[i_str++] = c;
      break;
    }
  }
}
/*! \brief Minimalist file editor to append char to a file.
 *
 * \note Hit ^q to exit and save file.
 */



static void fat_example_append_file(void)
{
  int c;

  print(SHL_USART, MSG_APPEND_WELCOME);

  // Wait for ^q to quit.
  while (TRUE)
  {
    // If something new in the USART
    usart_reset_status(SHL_USART);
    if (usart_read_char(SHL_USART, &c) == USART_SUCCESS)
    {
      // if this is not the quit char
      if (c != QUIT_APPEND)
      {
        // Echo the char.
        print_char(SHL_USART, c);
        // Add it to the file.
        file_putc(c);
        // if it is a carriage return.
        if (c == CR)
        {
          // Echo line feed.
          print_char(SHL_USART, LF);
          // Add line feed to the file.
          file_putc(LF);
        }
      }
      // Quit char received.
      else
      {
        // Exit the append function.
        break;
      }
    }
  }
}

int BinaryDivide(int a, int b, int* rest){//implement division and mod by bit operations

    int msb = 0;

    for(msb = 0; msb < 64; msb++) {
        if((b << msb) >= a)
            break;
    }
    int q = 0;
    int i;
    for(i = msb; i >= 0; i--) {
        if((b << i) > a)
            continue;
        q |= (1 << i);
        a -= (b << i);
    }

    *rest = a;
    return q;
}
//****************************************
static void tc_irq(void)
{
  // get value for the potentiometer adc channel
	adc_start(adc);
	value=adc_get_value(adc, adc_channel_pot);
	adcnew=TRUE;
     tc_read_sr(EXAMPLE_TC, TC_CHANNEL);
     gpio_get_pin_interrupt_flag(GPIO_PUSH_BUTTON_0);
	// Clear the interrupt flag. This is a side effect of reading the TC SR
}
//*********************************************************
void sd_mmc_resources_init(void)
{
   //GPIO pins used for SD/MMC interface
  static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
  {
    {SD_MMC_SPI_SCK_PIN,  SD_MMC_SPI_SCK_FUNCTION },  // SPI Clock.
    {SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},  // MISO.
    {SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},  // MOSI.
    {SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
  };

	//SPI options.
	  spi_options_t spiOptions =
	  {
	    .reg          = SD_MMC_SPI_NPCS,
	    .baudrate     = PBAf,  // Defined in conf_sd_mmc_spi.h.
	    .bits         = SD_MMC_SPI_BITS,          // Defined in conf_sd_mmc_spi.h.
	    .spck_delay   = 0,
	    .trans_delay  = 0,
	    .stay_act     = 1,
	    .spi_mode     = 0,
	    .modfdis      = 1
	  };

  // Assign I/Os to SPI.
  gpio_enable_module(SD_MMC_SPI_GPIO_MAP,sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(SD_MMC_SPI, &spiOptions);

  // Set SPI selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

  // Enable SPI module.
  spi_enable(SD_MMC_SPI);

  // Initialize SD/MMC driver with SPI clock (PBA).
  sd_mmc_spi_init(spiOptions, PBAf);
}
//****************************************
int main(void)
{
	  U8 i, j;
	  U16 file_size;
	  Fs_index sav_index;
	  static Fs_index mark_index;
	  const char *part_type;
	  U32 VarTemp;

	//use power manger driver for setting the frequency
	    pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
		pm_freq_param_t System_Clock = {
			    .cpu_f = CPUf,
			    .pba_f = PBAf,
			    .osc0_f = FOSC0,
			    .osc0_startup = OSC0_STARTUP
			};
		pm_configure_clocks(&System_Clock);
//*********************************************************
//*********************************************************
	 volatile avr32_tc_t *tc = EXAMPLE_TC;
	  // Options for waveform genration.

	  static const tc_waveform_opt_t WAVEFORM_OPT =
	  {
	    .channel  = TC_CHANNEL,                        // Channel selection.

	    .bswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOB.
	    .beevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOB.
	    .bcpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOB.
	    .bcpb     = TC_EVT_EFFECT_NOOP,                // RB compare effect on TIOB.

	    .aswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOA.
	    .aeevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOA.
	    .acpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOA: toggle.
	    .acpa     = TC_EVT_EFFECT_NOOP,                // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

	    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
	    .enetrg   = FALSE,                             // External event trigger enable.
	    .eevt     = 0,                                 // External event selection.
	    .eevtedg  = TC_SEL_NO_EDGE,                    // External event edge selection.
	    .cpcdis   = FALSE,                             // Counter disable when RC compare.
	    .cpcstop  = FALSE,                             // Counter clock stopped with RC compare.

	    .burst    = FALSE,                             // Burst signal selection.
	    .clki     = FALSE,                             // Clock inversion.
	    .tcclks   = TC_CLOCK_SOURCE_TC3                // Internal source clock 3, connected to fPBA / 8.
	  };

	  static const tc_interrupt_t TC_INTERRUPT =
	  {
	    .etrgs = 0,
	    .ldrbs = 0,
	    .ldras = 0,
	    .cpcs  = 1,
	    .cpbs  = 0,
	    .cpas  = 0,
	    .lovrs = 0,
	    .covfs = 0
	  };
//*********************************************************
	  // GPIO pin/adc-function map.
	static const gpio_map_t ADC_GPIO_MAP =
	 {
      {EXAMPLE_ADC_POTENTIOMETER_PIN, EXAMPLE_ADC_POTENTIOMETER_FUNCTION}
	 };

      // Assign and enable GPIO pins to the ADC function.
     gpio_enable_module(ADC_GPIO_MAP, sizeof(ADC_GPIO_MAP) / sizeof(ADC_GPIO_MAP[0]));

     // configure ADC
       // Lower the ADC clock to match the ADC characteristics (because we configured
       // the CPU clock to 12MHz, and the ADC clock characteristics are usually lower;
       // cf. the ADC Characteristic section in the datasheet).
       AVR32_ADC.mr |= 0x1 << AVR32_ADC_MR_PRESCAL_OFFSET;
       adc_configure(adc);

       // Enable the ADC channels.
       adc_enable(adc,adc_channel_pot);
//**************************************************************
       INTC_init_interrupts ();
//*********************************************************
       // Initialize the timer/counter.
         tc_init_waveform(tc, &WAVEFORM_OPT);         // Initialize the timer/counter waveform.
         // Set the compare triggers.
         // Remember TC counter is 16-bits, so counting second is not possible with fPBA = 12 MHz.
         // We configure it to count ms.
         // We want: (1/(fPBA/8)) * RC = 0.02 s, +hence RC = (fPBA/8)*0.02= 41250to get an interrupt every 20 ms.
         tc_write_rc(tc, TC_CHANNEL, (PBAf / 8)*0.02); // Set RC value.
         // tc_write_rc(tc, TC_CHANNEL, 60000);
          tc_configure_interrupts(tc, TC_CHANNEL, &TC_INTERRUPT);
          INTC_register_interrupt(&tc_irq, EXAMPLE_TC_IRQ, AVR32_INTC_INT1);
//*********************************************************
           Enable_global_interrupt ();
        // launch conversion on all enabled channels
//*********************************************************
        // Initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
          sd_mmc_resources_init();

           tc_start(tc, TC_CHANNEL);

           if (mem_test_unit_ready(LUN_ID_SD_MMC_SPI_MEM) == CTRL_GOOD)
             {
               // Get and display the capacity
               mem_read_capacity(LUN_ID_SD_MMC_SPI_MEM, &VarTemp);
              // print(SHL_USART, "OK:\t");
               //print_ulong(SHL_USART, (VarTemp + 1) >> (20 - FS_SHIFT_B_TO_SECTOR));
              // print(SHL_USART, " MB\n");
             }
             else
             {
               // Display an error message
               //print(SHL_USART, "Not initialized: Check if memory is ready...\n");
             }

        // reset vars
        cmd = FALSE;
        cmd_type = CMD_NONE;
        first_ls = TRUE;
        // reset all navigators

       #ifndef FREERTOS_USED
       # if __GNUC__
         // Give the used CPU clock frequency to Newlib, so it can work properly.
         set_cpu_hz(pcl_freq_param.pba_f);
       # endif
       #endif

         // Initialize USB clock.
         pcl_configure_usb_clock();

         nav_reset();

         // Initialize USB task
         usb_task_init();



       #if USB_DEVICE_FEATURE == ENABLED
         // Initialize device CDC USB task
         device_cdc_task_init();
       #endif

       #ifdef FREERTOS_USED
         // Start OS scheduler
         vTaskStartScheduler();
         portDBG_TRACE("FreeRTOS returned.");
         return 42;
       #else
         // No OS here. Need to call each task in round-robin mode.
         write=FALSE;

         while (TRUE)
         {
           usb_task();

           if(flag && uart_usb_tx_ready())
           {
        	   uart_usb_putchar('$');
               uart_usb_putchar('>');
               uart_usb_flush();
               flag=FALSE;
           }

           // While a usable user command on RS232 isn't received, build it
              if (!cmd)
              {
                uart_usb_flush();

                fat_example_build_cmd();
              }
              // perform the command
              else
              {
                switch (cmd_type)
                {
                // this is a "mount" command
                case CMD_MOUNT:
                  // Get drive number
                  i = par_str1[0] - 'a';
                  // If drive doesn't exist
                  if (i >= nav_drive_nb())
                  {
                    // Display error message.
                    //print(SHL_USART, MSG_ER_DRIVE);
                    PrintString(MSG_ER_DRIVE);
                  }
                  else
                  {
                    // Reset all navigators.
                    nav_reset();
                    // Select the desired drive.
                    nav_drive_set(i);
                    // Try to mount it.
                    if (!nav_partition_mount())
                    {
                      // Display error message.
                      //print(SHL_USART, MSG_ER_MOUNT);
                      PrintString(MSG_ER_MOUNT);
                      // Retry to mount at next "ls".
                      first_ls = TRUE;
                    }
                    else
                    {
                      // Clear flag, no need to remount at next "ls".
                      first_ls = FALSE;
                    }
                  }
                  break;
                // this is a "fat" information command
                case CMD_FAT:
                  // Regarding the partition type :
                  switch (nav_partition_type())
                  {
                  case FS_TYPE_FAT_12:
                    // Display FAT12.
                    part_type = "Drive uses FAT12\n\r";
                    break;
                  case FS_TYPE_FAT_16:
                    // Display FAT16.
                    part_type = "Drive uses FAT16\n\r";
                    break;
                  case FS_TYPE_FAT_32:
                    // Display FAT32.
                    part_type = "Drive uses FAT32\n\r";
                    break;
                  default:
                    // Display error message.
                    part_type = "Drive uses an unknown partition type\n\r";
                    break;
                  }
                  //print(SHL_USART, part_type);
                  PrintString(part_type);
                  break;
                // this is a "ls" command
                case CMD_LS:
                  // Check if params are correct or mount needed.
                  if (nav_drive_get() >= nav_drive_nb() || first_ls)
                  {
                    first_ls = FALSE;
                    // Reset navigators .
                    nav_reset();
                    // Use the last drive available as default.
                    nav_drive_set(nav_drive_nb() - 1);
                    // Mount it.
                    nav_partition_mount();
                  }
                  // Get the volume name
                  nav_dir_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH);
                  // Display general informations (drive letter and current path)
                  //print(SHL_USART, "\n\rVolume is ");
                  PrintString("\n\rVolume is");

                  //print_char(SHL_USART, 'A' + nav_drive_get());

                  uart_usb_putchar('A'+nav_drive_get());


                 // print(SHL_USART, ":\nDir name is ");
                  PrintString(":\n\rDir name is");

                  //print(SHL_USART, str_buff);
                  PrintString(str_buff);

                  //print_char(SHL_USART, LF);
                  uart_usb_putchar(LF);
                  uart_usb_putchar('\r');
                  uart_usb_flush();
                  // Try to sort items by folders
                  if (!nav_filelist_first(FS_DIR))
                  {
                    // Sort items by files
                    nav_filelist_first(FS_FILE);
                  }
                  // Display items informations
                  //print(SHL_USART, "\tSize (Bytes)\tName\n\r");
                  PrintString("\tSize (Bytes)\tName\n\r");
                  // reset filelist before to start the listing
                  nav_filelist_reset();
                  // While an item can be found
                  while (nav_filelist_set(0, FS_FIND_NEXT))
                  {
                    // Get and display current item informations
                   // print(SHL_USART, (nav_file_isdir()) ? "Dir\t" : "   \t");
                    PrintString((nav_file_isdir()) ? "Dir\t" : "   \t");

                   // print_ulong(SHL_USART, nav_file_lgt());
                    PrintNumber(nav_file_lgt());

                   // print(SHL_USART, "\t\t");
                    PrintString("\t\t");

                    nav_file_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH, FS_NAME_GET, TRUE);
                    //print(SHL_USART, str_buff);
                    PrintString(str_buff);

                   // print_char(SHL_USART, LF);
                    uart_usb_putchar(LF);
                    uart_usb_putchar('\r');
                    uart_usb_flush();
                  }
                  // Display the files number
//                  print_ulong(SHL_USART, nav_filelist_nb(FS_FILE));
//                  print(SHL_USART, "  Files\n\r");
                  PrintNumber(nav_filelist_nb(FS_FILE));
                  PrintString("  Files\n\r");
                  // Display the folders number
                  //print_ulong(SHL_USART, nav_filelist_nb(FS_DIR));
                  //print(SHL_USART, "  Dir\n\r");

                  PrintNumber(nav_filelist_nb(FS_DIR));
                  PrintString("  Dir\n\r");
                  break;
                // this is a "cd" command
                case CMD_CD:
                  // get arg1 length
                  i = strlen(par_str1);
                  // Append the '/' char for the nav_setcwd to enter the chosen directory.
                  if (par_str1[i - 1] != '/')
                  {
                    par_str1[i] = '/';
                    par_str1[i + 1] = '\0';
                  }
                  // Try to to set navigator on arg1 folder.
                  if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
                  {
                    // Display error message.
                    //print(SHL_USART, MSG_ER_UNKNOWN_FILE);
                    PrintString(MSG_ER_UNKNOWN_FILE);
                  }
                  break;
                // this is a "cat" command
                case CMD_CAT:
                  // Try to to set navigator on arg1 file.
                  if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
                  {
                    // Display error message.
                   // print(SHL_USART, MSG_ER_UNKNOWN_FILE);
                    PrintString(MSG_ER_UNKNOWN_FILE);
                  }
                  else
                  {
                    // Open the file.
                    file_open(FOPEN_MODE_R);
                    // While the end isn't reached
                    while (!file_eof())
                    {
                    	// Display next char from file.
                     // print_char(SHL_USART, file_getc());
                      uart_usb_putchar(file_getc());
                    }
                    // Close the file.
                    file_close();
                    //print_char(SHL_USART, LF);
                    uart_usb_putchar(LF);
                    uart_usb_flush();
                  }
                  break;
                // this is a "help" command
                case CMD_HELP:
                  // Display help on USART
                  //print(SHL_USART, MSG_HELP);
                  PrintString(MSG_HELP);
                  break;
                // this is a "mkdir" command
                case CMD_MKDIR:
                  // Create the folder;
                  nav_dir_make((FS_STRING)par_str1);
                  break;
                // this is a "touch" command
                case CMD_TOUCH:
                  // Create the file.
                  nav_file_create((FS_STRING)par_str1);
                  break;
                // this is a "rm" command
                case CMD_RM:
                  // Save current nav position.
                  sav_index = nav_getindex();
                  // Try to to set navigator on arg1 folder or file.
                  if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
                  {
                    // Display error message.
                    //print(SHL_USART, MSG_ER_UNKNOWN_FILE);
                    PrintString(MSG_ER_UNKNOWN_FILE);
                  }
                  // Try to delete the file
                  else if (!nav_file_del(FALSE))
                  {
                    // Display error message.
                   // print(SHL_USART, MSG_ER_RM);
                    PrintString(MSG_ER_RM);
                  }
                  // Restore nav position.
                  nav_gotoindex(&sav_index);
                  break;
                // this is a "append"  command: Append a char to selected file.
                case CMD_APPEND:
                  // Try to to set navigator on arg1 file.
                  if (!nav_setcwd((FS_STRING)par_str1, TRUE, TRUE))
                  {
                    // Display error message.
                    //print(SHL_USART, MSG_ER_UNKNOWN_FILE);
                    PrintString(MSG_ER_UNKNOWN_FILE);
                  }
                  else
                  {
                    // File exists, open it in append mode
                    file_open(FOPEN_MODE_APPEND);
                    // Append from USART
                    fat_example_append_file();
                    // Close the file
                    file_close();
                    // Display a line feed to user
                    //print_char(SHL_USART, LF);
                    uart_usb_putchar(LF);
                    uart_usb_flush();
                  }
                  break;
                // this is a "disk" command
                case CMD_NB_DRIVE:
                  // Display number of drives.
                  print(SHL_USART, "Nb Drive(s): ");
                  print_char(SHL_USART, '0' + nav_drive_nb());
                  print_char(SHL_USART, LF);
                  break;
                // this is a "mark" command
                case CMD_SET_ID:
                  // get marked index from current navigator location
                  mark_index = nav_getindex();
                  break;
                // this is a "goto" command
                case CMD_GOTO_ID:
                  // set navigator to the marked index
                  nav_gotoindex(&mark_index);
                  break;
                // this is a "cp" command: Copy file to other location.
                case CMD_CP:
                  // Try to set navigator on arg1 file.
                  if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
                  {
                    // Display error message.
                    print(SHL_USART, MSG_ER_UNKNOWN_FILE);
                  }
                  else
                  {
                    // Get name of source to be used as same destination name.
                    nav_file_name((FS_STRING)par_str1, MAX_FILE_PATH_LENGTH, FS_NAME_GET, TRUE);
                    // Get file size.
                    file_size = nav_file_lgtsector();
                    // Mark source.
                    nav_file_copy();
                    // Save current source position.
                    sav_index = nav_getindex();
                    // Goto destination.
                    nav_gotoindex(&mark_index);
                    // Free space check.
                    if (nav_partition_space() > file_size)
                    {
                      // Paste.
                      nav_file_paste_start((FS_STRING)par_str1);
                      // Restore previous nav position.
                      nav_gotoindex(&sav_index);
                      // Performs copy.
                      while (nav_file_paste_state(FALSE) == COPY_BUSY);
                    }
                    // Restore previous nav position.
                    nav_gotoindex(&sav_index);
                  }
                  break;
                // this is a "mv" command: Rename file.
                case CMD_MV:
                  // Save current nav position.
                  sav_index = nav_getindex();
                  // Try to to set navigator on arg1 folder or file.
                  if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
                  {
                    // Display error message.
                    print(SHL_USART, MSG_ER_UNKNOWN_FILE);
                  }
                  // Try to rename the file
                  else if (!nav_file_rename((FS_STRING)par_str2))
                  {
                    // Display error message.
                    print(SHL_USART, MSG_ER_MV);
                  }
                  // Restore nav position.
                  nav_gotoindex(&sav_index);
                  break;
                // this is a "df" command: Display free space information for all connected drives.
                case CMD_DF:
                  // Save current nav position.
                  sav_index = nav_getindex();
                  // For all available drives :
                  for (i = 0; i < nav_drive_nb(); i++)
                  {
                    // Select drive.
                    nav_drive_set(i);
                    // Try to mount.
                    if (nav_partition_mount())
                    {
                      // Display memory name.
                      print(SHL_USART, mem_name(i));
                      // Display drive letter name.
                      print(SHL_USART, " (");
                      print_char(SHL_USART, 'A' + i);
                      print(SHL_USART, ":) Free Space: ");
                      // Display free space.
                      print_ulong(SHL_USART, nav_partition_freespace() << FS_SHIFT_B_TO_SECTOR);
                      print(SHL_USART, " Bytes / ");
                      // Display available space.
                      print_ulong(SHL_USART, nav_partition_space() << FS_SHIFT_B_TO_SECTOR);
                      print(SHL_USART, " Bytes\n");
                    }
                  }
                  // Restore nav position.
                  nav_gotoindex(&sav_index);
                  break;
                // this is a "format" command : Format disk.
                case CMD_FORMAT:
                  // Get disk number.
                  i = par_str1[0] - 'a';
                  // if drive number isn't valid
                  if (i >= nav_drive_nb())
                  {
                    // Display error message.
                    print(SHL_USART, MSG_ER_DRIVE);
                  }
                  else
                  {
                    // Get the current drive in the navigator.
                    j = nav_drive_get();
                    // Select drive to format.
                    nav_drive_set(i);
                    // If format fails.
                    if (!nav_drive_format(FS_FORMAT_DEFAULT))
                    {
                      // Display error message.
                      print(SHL_USART, MSG_ER_FORMAT);
                      // Return to the previous.
                      nav_drive_set(j);
                    }
                    // Format succeds, if drives is the one we were navigating on
                    else if (i == j)
                    {
                      // Reset the navigators.
                      nav_reset();
                      // Set current drive.
                      nav_drive_set(j);
                      // If partition mounting fails.
                      if (!nav_partition_mount())
                      {
                        // Display error message.
                        print(SHL_USART, MSG_ER_MOUNT);
                        // this will be the first "ls"
                        first_ls = TRUE;
                      }
                      else
                      {
                        // this won't be the first "ls", system is already mounted
                        first_ls = FALSE;
                      }
                    }
                    // Format succeds, restore previous navigator drive.
                    else nav_drive_set(j);
                  }
                  break;
                // this is a "format32" command: Format disk as FAT 32 if possible.
                case CMD_FORMAT32:
                  // Get disk number.
                  i = par_str1[0] - 'a';
                  // if drive number isn't valid
                  if (i >= nav_drive_nb())
                  {
                    // Display error message.
                    print(SHL_USART, MSG_ER_DRIVE);
                  }
                  else
                  {
                    // Get the current drive in the navigator.
                    j = nav_drive_get();
                    // Select drive to format.
                    nav_drive_set(i);
                    // If format fails.
                    if (!nav_drive_format(FS_FORMAT_FAT32))
                    {
                      // Display error message.
                      print(SHL_USART, MSG_ER_FORMAT);
                      // Return to the previous.
                      nav_drive_set(j);
                    }
                    // Format succeds, if drives is the one we were navigating on
                    else if (i == j)
                    {
                      // Reset the navigators.
                      nav_reset();
                      // Set current drive.
                      nav_drive_set(j);
                      // If partition mounting fails.
                      if (!nav_partition_mount())
                      {
                        // Display error message.
                        print(SHL_USART, MSG_ER_MOUNT);
                        // this will be the first "ls"
                        first_ls = TRUE;
                      }
                      else
                      {
                        // this won't be the first "ls", system is already mounted
                        first_ls = FALSE;
                      }
                    }
                    // Format succeds, restore previous navigator drive.
                    else nav_drive_set(j);
                  }
                  break;
                case CMD_START:

                	 nav_file_create((FS_STRING)par_str1);
                	 PrintString("Start Recording");
                	 if (!nav_setcwd((FS_STRING)par_str1, TRUE, TRUE))
                	 {
                	         // Display error message.
                	    PrintString(MSG_ER_UNKNOWN_FILE);
                	  }
                	  else
                	  {
                	        // File exists, open it in append mode
                	    file_open(FOPEN_MODE_APPEND);
                	    write=TRUE;
                	    //print(SHL_USART," open file successfully\n");
                	    PrintString("\topen file successfully\n\r");
                      }
                	break;
                case CMD_STOP:
                    // print(SHL_USART, "Stop Recording");
                     PrintString("Stop Recording");
                     // Close the file
    	              file_close();
    	              write=FALSE;
    	             // Display a line feed to user
    	             // print(SHL_USART," close file successfully\n");
    	              PrintString("\tclose file successfully\n\r");

                     break;
                // Unknown command.
                default:
                  // Display error message.
                  //print(SHL_USART, MSG_ER_CMD_NOT_FOUND);
                  PrintString(MSG_ER_CMD_NOT_FOUND);
                  break;
                }
                uart_usb_putchar('$');
                uart_usb_putchar('>');
                uart_usb_flush();
                // Reset vars.
                cmd_type = CMD_NONE;
                cmd = FALSE;
                // Display prompt.
                //print(SHL_USART, MSG_PROMPT);

              }

              if(write && adcnew)//append adc sampling value into file digit by digit
              {
                 int digit[4]={0};
            	 int res = 0;
            	 int digitindex=3;

            	 int x=value;
            	 while(x)
            	 {
            	   x=BinaryDivide(x,10,&res);
            	   digit[digitindex--]=res;
            	 }
            	 int j;
            	 for(j=0;j<4;j++)
            	 {
            	   file_putc(digit[j]+48);
            	 }
            	  adcnew=FALSE;
            	  file_putc(LF);
              }



         }
       #endif  // FREERTOS_USED
       adc_disable(adc,adc_channel_pot);

	return 0;
}

