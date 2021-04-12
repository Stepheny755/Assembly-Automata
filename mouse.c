/* FPGA address macros */
#define BOARD                 "DE1-SoC"

/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_ONCHIP_END       0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define FPGA_BRIDGE           0xFFD0501C

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

/* processor mode constants */
#define		USER_MODE				0b10000
#define		FIQ_MODE				0b10001
#define		IRQ_MODE				0b10010
#define		SVC_MODE				0b10011
#define		ABORT_MODE				0b10111
#define		UNDEF_MODE				0b11011
#define		SYS_MODE				0b11111

#define 	KEYS_IRQ				73

#define		INT_ENABLE				0b01000000
#define		INT_DISABLE				0b11000000
#define		ENABLE					0x1


/* function prototypes */
void HEX_PS2(char, char, char);
/*******************************************************************************
* This program demonstrates use of the PS/2 port by displaying the last three
* bytes of data received from the PS/2 port on the HEX displays.
******************************************************************************/
int main(void) {
  /* Declare volatile pointers to I/O registers (volatile means that IO load
  and store instructions will be used to access these pointer locations,
  instead of regular memory loads and stores) */
  volatile int * PS2_ptr = (int *)PS2_BASE;
  int PS2_data, RVALID;
  char byte1 = 0, byte2 = 0, byte3 = 0;
  // PS/2 mouse needs to be reset (must be already plugged in)
  *(PS2_ptr) = 0xFF; // reset
  while (1) {
    PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
    RVALID = PS2_data & 0x8000; // extract the RVALID field
    if (RVALID) {
      /* shift the next data byte into the display */
      byte1 = byte2;
      byte2 = byte3;
      byte3 = PS2_data & 0xFF;
      HEX_PS2(byte1, byte2, byte3);
      if ((byte2 == (char)0xAA) && (byte3 == (char)0x00))
      // mouse inserted; initialize sending of data
        *(PS2_ptr) = 0xF4;
    }
  }
}
/****************************************************************************************
* Subroutine to show a string of HEX data on the HEX displays
****************************************************************************************/
void HEX_PS2(char b1, char b2, char b3) {
  volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
  volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;
  /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
  * a single 7-seg display in the DE1-SoC Computer, for the hex digits 0 - F
  */
  unsigned char seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
  0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
  unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned int shift_buffer, nibble;
  unsigned char code;
  int i;
  shift_buffer = (b1 << 16) | (b2 << 8) | b3;
  for (i = 0; i < 6; ++i) {
    nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
    code = seven_seg_decode_table[nibble];
    hex_segs[i] = code;
    shift_buffer = shift_buffer >> 4;
  }
  /* drive the hex displays */
  *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
  *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}
