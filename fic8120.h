/*
    fic8120.h
    Justin modified 2005/8/9 01:35pm
*/

#ifndef     _FIC8120_H_
#define     _FIC8120_H_

//#define CONFIG_FIQ_TO_IRQ  1

/******************************************************************
    AHB/APB device register mapping
 *****************************************************************/
#define CPE_AHB_BASE                    0x90100000
#define CPE_SRAMC_BASE                  0x90200000
#define CPE_SDRAMC_BASE                 0x90300000
#define CPE_APBDMA_BASE                 0x90500000
#define CPE_LCD_BASE                    0x90600000
#define CPE_XPEG_BASE                   0x90700000

#define CPE_COLORTRANS_BASE             0x90800000
#define CPE_INPUTOSD_BASE               0x90a00000

#define CPE_FTMAC_BASE                  0x90900000
#define CPE_AHBDMA_BASE                 0x90C00000
#define CPE_PCI_BASE                    0x90e00000
#define CPE_FOTG200_BASE  		        0x91300000 
#define CPE_HOST20_BASE                 0x91300000
#define CPE_IDE_BASE                	0x91400000
#define CPE_CAPTURE_BASE                0x96800000
#define CPE_PMU_BASE                    0x98100000
#define CPE_UART1_BASE                  0x98200000
#define CPE_UART2_BASE                  0x98300000				   // none
#define CPE_TIMER1_BASE                 0x98400000
#define CPE_TIMER2_BASE                 0x98400010
#define CPE_TIMER3_BASE                 0x98400020
#define CPE_WDT_BASE		            0x98500000
#define CPE_RTC_BASE		            0x98600000
#define CPE_GPIO_BASE                   0x98700000
#define CPE_IC_BASE                     0x98800000
#define CPE_UART3_BASE                  0x98900000
#define CPE_I2C_BASE                    0x98a00000
#define CPE_SSP1_BASE                   0x98b00000
#define CPE_SD_BASE                    	0x98e00000   //SD
#define CPE_UART4_BASE                  0x99600000
#define CPE_PCI_MEM                     0xa0000000


/*********************************************************************
  virtual address range 0xf9xx0000 
 *********************************************************************/

#define CPE_AHB_VA_BASE                 IO_ADDRESS(CPE_AHB_BASE)
#define CPE_SRAMC_VA_BASE               IO_ADDRESS(CPE_SRAMC_BASE)
#define CPE_SDRAMC_VA_BASE              IO_ADDRESS(CPE_SDRAMC_BASE)
#define CPE_APBDMA_VA_BASE              IO_ADDRESS(CPE_APBDMA_BASE)
#define CPE_LCD_VA_BASE                 IO_ADDRESS(CPE_LCD_BASE)
#define CPE_COLORTRANS_VA_BASE          IO_ADDRESS(CPE_COLORTRANS_BASE)
#define CPE_INPUTOSD_VA_BASE            IO_ADDRESS(CPE_INPUTOSD_BASE)
#define CPE_AHBDMA_VA_BASE              IO_ADDRESS(CPE_AHBDMA_BASE)
#define CPE_CAPTURE_VA_BASE             IO_ADDRESS(CPE_CAPTURE_BASE)
#define CPE_FOTG200_VA_BASE   			IO_ADDRESS(CPE_FOTG200_BASE)
#define CPE_IDE_VA_BASE   			    IO_ADDRESS(CPE_IDE_BASE)
#define CPE_PMU_VA_BASE                 IO_ADDRESS(CPE_PMU_BASE)
#define CPE_UART1_VA_BASE               IO_ADDRESS(CPE_UART1_BASE)
#define CPE_UART2_VA_BASE               IO_ADDRESS(CPE_UART2_BASE)
#define CPE_UART3_VA_BASE               IO_ADDRESS(CPE_UART3_BASE)
#define CPE_UART4_VA_BASE               IO_ADDRESS(CPE_UART4_BASE)
#define CPE_TIMER1_VA_BASE              IO_ADDRESS(CPE_TIMER1_BASE)
#define CPE_TIMER2_VA_BASE              IO_ADDRESS(CPE_TIMER2_BASE)
#define CPE_TIMER3_VA_BASE              IO_ADDRESS(CPE_TIMER3_BASE)
#define CPE_WDT_VA_BASE		        	IO_ADDRESS(CPE_WDT_BASE)
#define CPE_RTC_VA_BASE		        	IO_ADDRESS(CPE_RTC_BASE)
#define CPE_GPIO_VA_BASE                IO_ADDRESS(CPE_GPIO_BASE)
#define CPE_IC_VA_BASE                  IO_ADDRESS(CPE_IC_BASE)
#define CPE_I2C_VA_BASE                 IO_ADDRESS(CPE_I2C_BASE)
#define CPE_SSP1_VA_BASE                IO_ADDRESS(CPE_SSP1_BASE)
#define CPE_FTMAC_VA_BASE               IO_ADDRESS(CPE_FTMAC_BASE)
#define CPE_PCI_VA_BASE                 IO_ADDRESS(CPE_PCI_BASE)
#define CPE_PCI_VA_MEM                  IO_ADDRESS(CPE_PCI_MEM)
#define CPE_HOST20_VA_BASE              IO_ADDRESS(CPE_HOST20_BASE)
#define CPE_SD_VA_BASE                  IO_ADDRESS(CPE_SD_BASE)   //SD


/*********************************************************************
  user setting VA address
 *********************************************************************/
#define CPE_FLASH_SZ                SZ_32M
#define CPE_FLASH_VA_BASE           0xf0400000  //0xf0400000-0xf2400000
#define CPE_FLASH_BOOT_VA_BASE      0xf3000000  //0xf8000000-0xfa000000

#define CPE_XPEG_SZ                 SZ_1M
#define CPE_XPEG_VA_BASE            0xf9700000  //0xf9700000-0xf9800000


/*****************************************************************
    Clock Setting
*****************************************************************/
#define AHB_CLK                     	CONFIG_SYS_CLK
#define APB_CLK                     	(CONFIG_SYS_CLK/2)	

#define CPE_BAUD_115200                 (CONFIG_UART_CLK / 1843200)
#define CPE_BAUD_57600                  (CONFIG_UART_CLK / 921600) 
#define CPE_BAUD_38400			        (CONFIG_UART_CLK / 614400)
#define CPE_BAUD_19200                  (CONFIG_UART_CLK / 307200)
#define CPE_BAUD_14400                  (CONFIG_UART_CLK / 230400)
#define CPE_BAUD_9600                   (CONFIG_UART_CLK / 153600)
#define CPE_BAUD_4800                   (CONFIG_UART_CLK / 76800)
#define CPE_BAUD_2400                   (CONFIG_UART_CLK / 38400)
#define CPE_BAUD_1200                   (CONFIG_UART_CLK / 19200)


/*****************************************************************
    IRQ
*****************************************************************/
/*
interrupt:
 0-31       A320 irq
 32-63      A320 fiq
 64-95      A321 irq
 96-127     A321 fiq
 150-159    Virtual IRQ (PCI)
 160-169    AHB dma
 170-179    APB dma
 180-199    Virtual IRQ (reserved)
 */
#define NR_IRQS                         200
#define CPE_NR_IRQS                     32
#define CPE_NR_FIQS                     32
#define CPE_FIQ_START                   32
#define CPE_FIQ_END                     63
#define CPE_A321_IRQ_START              64

#define CPE_VIRQ_PCI_START		150
#define CPE_VIRQ_PCI_END                154
#define CPE_VFIQ_PCI_START		155
#define CPE_VFIQ_PCI_END                159

#define CPE_VIRQ_AHBDMA_START           160
#define CPE_VIRQ_AHBDMA_END             169
#define CPE_VIRQ_APBDMA_START           170
#define CPE_VIRQ_APBDMA_END             179

//irq number
#define IRQ_LCD_ALL                     0  
//#define IRQ_CFC_CD                      1
#define IRQ_SSP1                        2
#define IRQ_I2C                         3
//#define IRQ_MMSC                        4
#define IRQ_SDC                         5
//#define IRQ_SSP2                        6
#define IRQ_LCD_NEXT_BASE               7
#define IRQ_PMU                         8
#define IRQ_LCD_V_ST                    9
#define IRQ_UART1                       10
#define IRQ_LCD_FIFO_UND                11
#define IRQ_UART2                       12
//#define IRQ_SMMC                        12
#define IRQ_GPIO                        13
#define IRQ_TIMER2                      14
#define IRQ_TIMER3                      15
#define IRQ_WATCHDOG                    16
#define IRQ_RTCALARM                    17
#define IRQ_RTCSEC                      18
#define IRQ_TIMER1                      19
#define IRQ_LCD_BUS_ERR                 20

#define IRQ_XPEG                        21
#define IRQ_CAPTURE                     22
//#define IRQ_USBDEV_VBUS                 23
#define IRQ_UART3                       23
#define IRQ_APB_BRIDGE                  24
#define IRQ_CPE_APB_DMA                 24
#define IRQ_UART4                       25
#define IRQ_CPE_AHB_DMA                 26
#define IRQ_IDE                      27
//#define IRQ_TOUCH                       28 hardware not support on FIC8120

#ifdef CONFIG_FIQ_TO_IRQ
#define IRQ_MAC                         1
#define IRQ_FOTG200   			4
#define IRQ_USB_HOST20                  4
#define IRQ_PCI   			6
#else
#define IRQ_MAC                         (3+CPE_NR_FIQS)
#define IRQ_FOTG200   			(5+CPE_NR_FIQS)
#define IRQ_USB_HOST20                  (5+CPE_NR_FIQS)   // USB Host 200
#define IRQ_PCI   			(13+CPE_NR_FIQS)
#endif

//#define IRQ_EXT_A321                    30
//#define IRQ_SECU                        29   // security AES/DES/3DES

//#define IRQ_A321_PCI                    (28+CPE_A321_IRQ_START)
//#define IRQ_A321_KBD                    (4+CPE_A321_IRQ_START)
//#define IRQ_A321_MOUSE                  (9+CPE_A321_IRQ_START)
//#define IRQ_A321_MAC2                   (25+CPE_A321_IRQ_START)  //2nd MAC
//#define IRQ_A321_MAC                    (25+CPE_A321_IRQ_START)

#ifdef CONFIG_FIQ_TO_IRQ
#define VIRQ_PCI_A                      (0+CPE_VIRQ_PCI_START)
#define VIRQ_PCI_B                      (1+CPE_VIRQ_PCI_START)
#define VIRQ_PCI_C                      (2+CPE_VIRQ_PCI_START)
#define VIRQ_PCI_D                      (3+CPE_VIRQ_PCI_START)
#else
#define VIRQ_PCI_A                      (0+CPE_VFIQ_PCI_START)
#define VIRQ_PCI_B                      (1+CPE_VFIQ_PCI_START)
#define VIRQ_PCI_C                      (2+CPE_VFIQ_PCI_START)
#define VIRQ_PCI_D                      (3+CPE_VFIQ_PCI_START)
#endif

#define LEVEL                           0
#define EDGE                            1
#define H_ACTIVE                        0
#define L_ACTIVE                        1

#define IRQ_SOURCE_REG                  0
#define IRQ_MASK_REG                    0x04
#define IRQ_CLEAR_REG                   0x08
#define IRQ_MODE_REG                    0x0c
#define IRQ_LEVEL_REG                   0x10
#define IRQ_STATUS_REG                  0x14

#define FIQ_SOURCE_REG                  0x20
#define FIQ_MASK_REG                    0x24
#define FIQ_CLEAR_REG                   0x28
#define FIQ_MODE_REG                    0x2c
#define FIQ_LEVEL_REG                   0x30
#define FIQ_STATUS_REG                  0x34


/*****************************************************************
    Flash
*****************************************************************/
#define CPE_FLASH_BASE                  0x80400000
#define CPE_FLASH_BOOT_BASE             0x80000000

/*****************************************************************
    IO Mapping
*****************************************************************/
#define IO_BASE	                        0xf0000000
#define IO_ADDRESS(x)                   (((x>>4)&0xffff0000)+(x&0xffff)+IO_BASE) 
#define PHY_ADDRESS(x)                  (((x<<4)&0xfff00000)+(x&0xffff))


/*****************************************************************
    PCI
*****************************************************************/
#define PCI_IO_VA_BASE                  (CPE_PCI_VA_BASE+SZ_4K)
#define PCI_IO_VA_SIZE                  (SZ_64K-SZ_4K)
#define PCI_IO_VA_END                   (CPE_PCI_VA_BASE+SZ_64K)
#define PCI_IO_SIZE			            SZ_64K
#define PCI_MEM_BASE                    CPE_PCI_MEM
#define PCI_MEM_SIZE                    SZ_64M
#define PCI_MEM_END                     (CPE_PCI_MEM+SZ_64M)

#define PCI_BRIDGE_DEVID                0x4321
#define PCI_BRIDGE_VENID                0x159b

/*****************************************************************
    PMU
*****************************************************************/
#define AHB_DMA_CHANNEL_NUM             4
#define APB_DMA_CHANNEL_NUM             4
#define PMU_SSP_TX_AHB_DMA_CHANNEL      0x0
#define PMU_SSP_RX_AHB_DMA_CHANNEL      0x1

#define PMU_IDE_TX_AHB_DMA_CHANNEL      0x4
#define PMU_IDE_RX_AHB_DMA_CHANNEL      0x5

//#define PMU_USBDEV_DMA_CHANNEL			0x2

#define PMU_SD_APB_DMA_CHANNEL          0x1
//#define PMU_MS_APB_DMA_CHANNEL          0x0
//#define PMU_CF_APB_DMA_CHANNEL          0x2
//#define PMU_SMMC_APB_DMA_CHANNEL        0x3

#define VIRQ_SSP_AHB_TX_DMA             (CPE_VIRQ_AHBDMA_START+PMU_SSP_TX_AHB_DMA_CHANNEL)
#define VIRQ_SSP_AHB_RX_DMA             (CPE_VIRQ_AHBDMA_START+PMU_SSP_RX_AHB_DMA_CHANNEL)
#define VIRQ_SD_APB_DMA                 (CPE_VIRQ_APBDMA_START+PMU_SD_APB_DMA_CHANNEL)
//#define VIRQ_MS_APB_DMA                 (CPE_VIRQ_APBDMA_START+PMU_MS_APB_DMA_CHANNEL)
//#define VIRQ_CF_APB_DMA                 (CPE_VIRQ_APBDMA_START+PMU_CF_APB_DMA_CHANNEL)
//#define VIRQ_SMMC_APB_DMA               (CPE_VIRQ_APBDMA_START+PMU_SMMC_APB_DMA_CHANNEL)

//#define VIRQ_USBDEV_AHB_DMA             (CPE_VIRQ_AHBDMA_START+PMU_USBDEV_DMA_CHANNEL)

//#define REQ_SMMC_NUM                    0xc
#define PMU_IDE_READ_DMA_CHANNEL		0x5
#define VIRQ_IDE_READ_AHB_DMA			(CPE_VIRQ_AHBDMA_START+PMU_IDE_READ_DMA_CHANNEL)
#define PMU_IDE_WRITE_DMA_CHANNEL		0x4
#define VIRQ_IDE_WRITE_AHB_DMA			(CPE_VIRQ_AHBDMA_START+PMU_IDE_WRITE_DMA_CHANNEL)

#endif
