/* fmpeg4_avcodec.h (same with driver definition fmpeg4.h) */
#ifndef _FMPEG4_AVCODEC_H_
#define _FMPEG4_AVCODEC_H_

#include "fmpeg4_version.h"

#define FMPEG4_IOCTL_DECODE_INIT    0x4170
#define FMPEG4_IOCTL_DECODE_FRAME   0x4172
#define FMPEG4_IOCTL_DECODE_422     0x4174

#define FMPEG4_IOCTL_ENCODE_INIT    0x4173
#define FMPEG4_IOCTL_ENCODE_FRAME   0x4175
#define FMPEG4_IOCTL_ENCODE_INFO	0x4176
#define FMPEG4_IOCTL_GET_DEBUG_TIMER 0x4180
#define FMPEG4_IOCTL_GET_JPG_BUF	0x4181
#define	FMPEG4_IOCTL_FREE_JPG_BUF	0x4182
#define FMPEG4_IOCTL_ENCODE_FLAG   0x4183
#define FMPEG4_IOCTL_SET_INTER_QUANT	0x4184
#define FMPEG4_IOCTL_SET_INTRA_QUANT	0x4185

typedef struct AVFrame {
    uint8_t *data[4];
} AVFrame;

typedef struct video_profile
{
    unsigned int bit_rate;
    unsigned int width;   //length per dma buffer
    unsigned int height;
    unsigned int framewidth;   //display screen size: use in decode  
    unsigned int frameheight;
    unsigned int framerate;
    unsigned int frame_rate_base;
    unsigned int gop_size;
    unsigned int qmax;
    unsigned int qmin;   
    unsigned int quant;
    unsigned int intra;
    unsigned int roi_enable;
    unsigned int roi_x;
    unsigned int roi_y;	
    unsigned int roi_width;
    unsigned int roi_height;	
    AVFrame *coded_frame;
    char *priv;
} video_profile;
typedef struct fmpeg4_parm{
    unsigned int input_va_y;       //input Y VA address or RGB address
    unsigned int input_va_u;       //input U VA address
    unsigned int input_va_v;       //input V VA address   
    unsigned int output_va_y;      //output Y VA address or RGB address
    unsigned int output_va_u;      //output U VA address
    unsigned int output_va_v;      //output V VA ddress
    unsigned int got_picture;
    unsigned int length;
}fmpeg4_parm_t;



typedef struct
{
    unsigned int u32API_version;
    unsigned int *pu32BaseAddr;
    void * pvSemaphore;
    unsigned int u32MaxWidth;
    unsigned int u32MaxHeight;
    unsigned int u32BSBufSize;
    unsigned char * pu8FrameBaseAddr_phy;	// output frame buffer, must 8 bytes align.
    unsigned char * pu8FrameBaseAddr_U_phy;	// frame buffer (U) if output format is yuv420, must 8 bytes align.
    unsigned char * pu8FrameBaseAddr_V_phy;	// frame buffer (V) if output format is yuv420, must 8 bytes align.
    unsigned int u32FrameWidth;			// output frame width, no matter decoded image size
    						// "set to 0", means equal to decoded image width
    unsigned int u32FrameHeight;		// output frame height, no matter decoded image size
    						// "set to 0", means equal to decoded image height
    unsigned int u32CacheAlign;
    unsigned char * pu8ReConFrameCur_phy;	// physical address. buffer for current reconstruct frame.
    unsigned char * pu8ReConFrameCur;		// virtual address.
    // Don't care when pfnDmaMalloc != NULL
    // 8 Byte boundary
    // byte size = ((width + 15)/16) x ((height + 15)/16) x 256 x 1.5
    unsigned char * pu8ReConFrameRef_phy;	// physical address. buffer for reference reconstruct frame.
    unsigned char * pu8ReConFrameRef;		// virtual address.
    // Don't care when pfnDmaMalloc != NULL
    // 8 Byte boundary
    // byte size = ((width + 15)/16) x ((height + 15)/16) x 256 x 1.5
    unsigned short * pu16ACDC_phy;			// physical address. buffer for ACDC predictor
    unsigned short * pu16ACDC;

    unsigned char * pu8DeBlock_phy;	// physical address. buffer for temp deblocking buffer
    unsigned char * pu8DeBlock;		// virtual address.
    // Don't care when pfnDmaMalloc != NULL
    // 8 Byte boundary
    // byte size = ((width + 15)/16) x 64
    unsigned char * pu8BitStream_phy;			// physical address. buffer for bitstream
    unsigned char * pu8BitStream;
    
    unsigned int u32EnableDeinterlace; // to enable our special deinterlace function for GM8120
                                   // And two limitations are required for this deinterlace feature :
                                   //   - The number of macroblock rows must be even number.
                                   //   - The image width and height must be equal to the display width and height.
  				// virtual address.
    // Don't care when pfnDmaMalloc != NULL
    // byte size = u32BSBufSize
#if 0
    DMA_MALLOC_PTR_dec pfnDmaMalloc;	// "set to NULL", means the 4 above buffers will be prepared enternallly
    DMA_FREE_PTR_dec pfnDmaFree;			// Don't care when pfnDmaMalloc == NULL
    MALLOC_PTR_dec pfnMalloc;
    FREE_PTR_dec pfnFree;
    SEM_WAIT_PTR pfnSemWait;
    SEM_SIGNAL_PTR pfnSemSignal;
    REQUEST_BS_PTR pfnRequestBS;
#endif
} FMP4_DEC_PARAM;

	
    /* Structure used to pass a frame to the encoder */
typedef struct
{
    void *bitstream;                   /*< [out]
    								*
    								* Output MPEG4 bitstream buffer pointer (physical addr.) */
    int length;                        /*< [out]
    								*
    								* Output MPEG4 bitstream length (bytes) */
   								
    unsigned char *quant_intra_matrix; /*< [in]
    								*
    								* Custom intra quantization matrix when MPEG4 quant is enabled*/
    unsigned char *quant_inter_matrix; /*< [in]
    								*
    								* Custom inter quantization matrix when MPEG4 quant is enabled */
    								
    int quant;                         /*< [in]
    								*
    								* Frame quantizer :
    								* <ul>
    								* <li> 0 (zero) : Then the  rate controler chooses the right quantizer
    								*                 for you.  Typically used in ABR encoding, or first pass of a VBR
    								*                 encoding session.
    								* <li> !=  0  :  Then you  force  the  encoder  to use  this  specific
    								*                  quantizer   value.     It   is   clamped    in   the   interval
    								*                  [1..31]. Tipically used  during the 2nd pass of  a VBR encoding
    								*                  session. 
    								* </ul> */
    int intra;                         /*< [in/out]
    								*
    								* <ul>
    								* <li> [in] : tells Gm if the frame must be encoded as an intra frame
    								*     <ul>
    								*     <li> 1: forces the encoder  to create a keyframe. Mainly used during
    								*              a VBR 2nd pass.
    								*     <li> 0:  forces the  encoder not to  create a keyframe.  Minaly used
    								*               during a VBR second pass
    								*     <li> -1: let   the  encoder   decide  (based   on   contents  and
    								*              max_key_interval). Mainly  used in ABR  mode and during  a 1st
    								*              VBR pass. 
    								*     </ul>
    								* <li> [out] : When first set to -1, the encoder returns the effective keyframe state
    								*              of the frame. 
    								* </ul>
                                    */
    
    /// The base address for input Y frame buffer.
    unsigned char *pu8YFrameBaseAddr;  /**< To set input Y frame buffer's base address.\n
                              *   <B>N.B.</B> : the input frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                              *   @see pu8UFrameBaseAddr
                              *   @see pu8VFrameBaseAddr
                              *
                              *   Also, this variable can be set by utilizing the function FMpeg4EncSetYUVAddr().
                              *   @see FMpeg4EncSetYUVAddr
                              */
    /// The base address for input U frame buffer.	
    unsigned char *pu8UFrameBaseAddr;  /**< To set input U frame buffer's base address.\n
                              *   <B>N.B.</B> : the input frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                              *   @see pu8YFrameBaseAddr
                              *   @see pu8VFrameBaseAddr
                              *
                              *   Also, this variable can be set by utilizing the function FMpeg4EncSetYUVAddr().
                              *   @see FMpeg4EncSetYUVAddr
                              */
    /// The base address for input V frame buffer.
    unsigned char *pu8VFrameBaseAddr;  /**< To set input V frame buffer's base address.\n
                              *   <B>N.B.</B> : the input frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                              *   @see pu8YFrameBaseAddr
                              *   @see pu8UFrameBaseAddr
                              *
                              *   Also, this variable can be set by utilizing the function FMpeg4EncSetYUVAddr().
                              *   @see FMpeg4EncSetYUVAddr
                              */
     int    active0;        //the result of motion dection 
     int    active1;        //the result of motion dection 
     int    active2;        //the result of motion dection 
     int    frameindex;     //the frame index for motion dection
     unsigned int    roi_X; 
     unsigned int    roi_Y;	 
     int module_time_base; //VOP header module_time_base, using magic number 0x55aaXXXX, XXXX is the number
} GM_ENC_FRAME;



typedef struct {
  /// The base address of hardware core.
  unsigned int   *pu32BaseAddr;    /**< User can use this variable to set the base
                                    *   address of hardware core.
                                    */
  /// The CPU cache alignment
  unsigned int u32CacheAlign;	/**< User needs to specify the CPU cache line in ,<B>bytes</B>.\n
                                    *   ex: The cache line is 16 when the CPU is FA526
                                    */
  /// The encoded bitrate in Kbps.
  unsigned int   u32BitRate;       /**< User can use this variable to set encoded bitrate in <B>Kbps</B>.\n
                                    *   Note that 'target bitrate = (u32BitRate * 1000 bits)'.\n
                                    *   And this option is only valid when rate control mechanism
                                    *   is enabled. (that is, when quantization value is equal to 0)
                                    *   @see u32Quant
                                    */

  /// The width of encoded frame in pels.
  unsigned int   u32FrameWidth;    /**< User can use this field to specify the <B>width</B> of 
                                    *   encoded frame in pels.
                                    */
  /// The height of encoded frame in pels.
  unsigned int   u32FrameHeight;   /**< User can use this field to specify the <B>height</B> of 
                                    *   encoded frame in pels.
                                    */
    /// To enable the function of encoding rectangular region of interest(ROI) within captured frame.
    int bROIEnable;  /**< If this parameter bROIEnable is enabled, the parameters u32ROIX , u32ROIY ,
                      *   u32ROIWidth and u32ROIHeight are valid.\n
                      *   If this parameter bROIEnable is disabled, the parameters u32ROIX , u32ROIY ,
                      *   u32ROIWidth and u32ROIHeight are ignored.\n
                      *
                      *   - 0: To disable the function of encoding rectangular region of interest.
                      *   - 1: To enable the function of encoding rectangular region of interest.
                      *
                      *   @see u32ROIX
                      *   @see u32ROIY
                      *   @see u32ROIWidth
                      *   @see u32ROIHeight
                      */
    /// The upper-left corner x coordinate of rectangular region of interest within captured frame.
    unsigned int u32ROIX;  /**< This parameter is valid only when bROIEnable is enabled.
                            *   User can use this parameter to specify the upper-left <B>x coordinate</B>
                            *   of region of interest within captured frame.
                            *   <B>N.B.</B> : the <B>x coordinate</B> must be a <B>multiple of 16 pixels</B> since
                            *   the picture is processed on a macroblock basis.
                            *   @see bROIEnable
                            */
    /// The upper-left corner coordinate y of region of interest within captured frame.
    unsigned int u32ROIY;  /**< This parameter is valid only when bROIEnable is enabled.
                            *   User can use this parameter to specify the upper-left <B>y coordinate</B>
                            *   of region of interest within captured frame.
                            *   <B>N.B.</B> : the <B>y coordinate</B> must be a <B>multiple of 16 pixels</B> since
                            *   the picture is processed on a macroblock basis.
                            *   @see bROIEnable
                            */
    /// The width of user-defined rectangular region of interest within the captured frame in pixel units.
    unsigned int u32ROIWidth;  /**< This parameter is valid only when bROIEnable is enabled.\n
                                *   This parameter specifies the width of retangular region of interest within
                                *   the captured frame in pixel units.\n
                                *   <B>N.B.</B> : the width needs to be a <B>multiple of 16 pixels</B> since
                                *   the picture is processed on a macroblock basis.
                                *   @see bROIEnable
                                */
	/// The height of user-defined rectangular region of interest within the captured frame in pixel units.
    unsigned int u32ROIHeight; /**< This parameter is valid only when bROIEnable is enabled.\n
                                *   This parameter specifies the height of retangular region of interest within
                                *   the captured frame in pixel units.\n
                                *   <B>N.B.</B> : the height needs be a <B>multiple of 16 pixels</B> since
                                *  the picture is processed on a macroblock basis.
                                *   @see bROIEnable
                                */

	/// The base frame rate.
  unsigned int fFrameRate;       /**< To set the encoded frame rate per second.
                                    */		                                
  /// The initial quantization value of I-frame while rate control mechanism is enabled.
  unsigned int   u32InitialQuant;  /**< While rate control mechanism is enabled , user can
                                    *   use this field to set the initial quantization value of
                                    *   I-frame. 
                                    *   @see u32Quant
                                    */
  /// The frame interval between I-frames.
  unsigned int   u32IPInterval;    /**< This variable was used to set the interval between I-frames.
                                    */
  /// The short header (H263) mode.
  int            bShortHeader;     /**< A flag of enabling short header (H.263) mode or not.
                                    *   - 0: disable short header.
                                    *   - 1: enable short header.
                                    */
  /// The 4 motion vectors (4MV) mode.
  int            bEnable4MV;       /**< To select 4MV (4 motion vectors) mode or 1MV (1 motion vector) mode.
                                    *   - 0: disable 4MV mode and use 1MV (1 motion vector) mode instead.
                                    *   - 1: enable 4MV mode and select 4MV mode.
                                    */
  /// The H.263 quantization method.		                            
  int            bH263Quant;       /**< To select H.263 quantization method or MPEG4 quantization method.
                                    *   - 0: select MPEG4 quantization method.
                                    *   - 1: select H.263 quantization method.
                                    *   - 2: select MPEG4 user defined quant tbl "quant.txt"
                                    */
  /// The resync marker option.
  int            bResyncMarker;    /**< A flag of enabling resync marker mechanism or not.
                                    *   - 0: disable resync marker.
                                    *   - 1: enable resync marker.
                                    */
  /// The maximum quantization value.
  unsigned int   u32MaxQuant;      /**< To set the maximum quantization value range.
                                    */
  /// The minimum quantization value.
  unsigned int   u32MinQuant;      /**< To set the minimum quantization value range.                             
                                    */
  /// The address of current reconstruct frame buffer.
  unsigned char *pu8ReConFrameCur; /**< 
                                    *  To specify the current reconstruct frame buffer address.\n
                                    *  In some occasions,user may want to provide his own reconstructed 
                                    *  buffer.\n
                                    *  Hence, if this variable is not set to NULL, the user-provided current
                                    *  reconstruct frame buffer is used and pointed by this variable.\n
                                    *  Otherwise, if this variable is set to NULL, encoder will internally
                                    *  use the installed @ref pfnDmaMalloc function to allocate the necessary
                                    *  current reconstruct frame buffer.\n\n
                                    *
                                    *  <B>value of @ref pu8ReConFrameCur</B>:
                                    *     <ul>
                                    *       <li> != <B>NULL</B> : Use user-provided buffer as current reconstruct
                                    *                             frame buffer. The requirement of the buffer size in bytes
                                    *                             is '( ((frame width + 15)/16) * (2+((frame height + 15)/16)) + 2) x 256 x 1.5'
                                    *       <li> == <B>NULL</B> : Use internally current reconstruct frame buffer
                                    *                             which is allocated by using installed @ref pfnDmaMalloc
                                    *                             function.
                                    *     </ul>
                                    *   <B>N.B.</B> : the current reconstruct frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                                    *                                    
                                    *   @see pfnDmaMalloc
                                    *   @see pfnDmaFree
                                    */
  /// The address of reference reconstruct frame buffer.
  unsigned char *pu8ReConFrameRef; /**< 
                                    *  To specify the reference reconstruct frame buffer address.\n
                                    *  In some occasions,user may want to provide his own reconstructed 
                                    *  buffer.\n
                                    *  Hence, if this variable is not set to NULL, the user-provided reference
                                    *  reconstruct frame buffer is used and pointed by this variable.\n
                                    *  Otherwise, if this variable is set to NULL, encoder will internally
                                    *  use the installed @ref pfnDmaMalloc function to allocate the necessary
                                    *  reference reconstruct frame buffer.\n\n
                                    *
                                    *  <B>value of @ref pu8ReConFrameRef</B>:
                                    *     <ul>
                                    *       <li> != <B>NULL</B> : Use user-provided buffer as reference reconstruct
                                    *                             frame buffer. The requirement of the buffer size in bytes
                                    *                             is '( ((frame width + 15)/16) * (2+((frame height + 15)/16)) + 2) x 256 x 1.5'
                                    *       <li> == <B>NULL</B> : Use internally reference reconstruct frame buffer
                                    *                             which is allocated by using installed @ref pfnDmaMalloc
                                    *                             function.
                                    *     </ul>
                                    *   <B>N.B.</B> : the reference reconstruct frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                                    *                                    
                                    *   @see pfnDmaMalloc
                                    *   @see pfnDmaFree
                                    */
  /// The address of internal AC DC predictor buffer.
  unsigned short *p16ACDC; /**< 
                   *  To specify the internal AC DC perdictor buffer address for encoder.\n
                   *  If this variable is not set to NULL, the user-provided buffer
                   *  pointed by this variable is used.\n
                   *  Otherwise, if this variable is set to NULL, encoder will internally
                   *  use the installed @ref pfnDmaMalloc function to allocate the necessary
                   *  AC DC predictor buffer.\n\n
                   *
                   *  <B>value of @ref p16ACDC</B>:
                   *     <ul>
                   *       <li> != <B>NULL</B> : Use user-provided buffer as AC DC predictor buffer.
                   *                             The requirement of the buffer size in bytes
                   *                             is '((frame width + 15)/16) * 64'
                   *       <li> == <B>NULL</B> : Use internally AC DC predictor buffer
                   *                             which is allocated by using installed @ref pfnDmaMalloc
                   *                             function.
                   *     </ul>
                   *   <B>N.B.</B> : the AC DC predictor buffer address must be <B>physical address</B> with <B>16-byte aligned</B>.
                   *
                   *   @see pfnDmaMalloc
                   *   @see pfnDmaFree
                   */
  
  /// The function pointer to user-defined DMA memory allocation function.
  void* pfnDmaMalloc;  /**< This variable contains the function pointer to the user-defined 
                                 *   DMA malloc function since under OS environment, our hardware device
                                 *   may need the physical address instead of virtual address.
                                 *
                                 *   @see pfnDmaFree
                                 *   @see DMA_MALLOC_PTR_enc
                                 *   @see DMA_FREE_PTR_enc
                                 */
  /// The function pointer to user-defined DMA free allocation function.
  void*   pfnDmaFree;    /**< This variable contains the function pointer to the user-defined 
                                 *   DMA free function since under OS environment, our hardware device
                                 *   may need the physical address instead of virtual address.
                                 *
                                 *   @see pfnDmaFree
                                 *   @see DMA_MALLOC_PTR_enc
                                 *   @see DMA_FREE_PTR_enc
                                 */
  void* pfnMalloc;
  void* pfnFree;
  // the size of bellow must sync to MOTION_INFO
  int resvd[22]; 
  /// The interval of frame/time for motion detection operation.
  int	md_interval;  /**< This variable specifies the interval of frame/time for motion detection operation. */
  unsigned int u32API_version;  /* this value is to check the driver version */
  int   ac_disable;  /** < This variable specifies the AC prediction Disable. */
} FMP4_ENC_PARAM;

typedef struct {
	// for the sake of motion detection
 	 int motion_dection_enable;  //1:enable 0:disable
  	/// The x-component of coordinate of motion detection region 0, upper-left point
  	int 	range_mb_x0_LU; /**< The x-component of coordinate of motion detection region 0, upper-left point. */
  	/// The y-component of coordinate of motion detection region 0, upper-left point.
  	int 	range_mb_y0_LU; /**< The y-component of coordinate of motion detection region 0, upper-left point. */
  	/// The x-component of coordinate of motion detection region 0, lower-right point.
  	int 	range_mb_x0_RD; /**< The x-component of coordinate of motion detection region 0, lower-right point. */
  	/// The y-component of coordinate of motion detection region 0, lower-right point.
  	int 	range_mb_y0_RD; /**< The y-component of coordinate of motion detection region 0, lower-right point. */
  	/// The x-component of coordinate of motion detection region 1, upper-left point.
  	int 	range_mb_x1_LU; /**< The x-component of coordinate of motion detection region 1, upper-left point. */
  	/// The y-component of coordinate of motion detection region 1, upper-left point.
  	int 	range_mb_y1_LU; /**< The y-component of coordinate of motion detection region 1, upper-left point. */
  	/// The x-component of coordinate of motion detection region 1, lower-right point.
  	int 	range_mb_x1_RD; /**< The x-component of coordinate of motion detection region 1, lower-right point. */
  	/// The y-component of coordinate of motion detection region 1, lower-right point.
  	int 	range_mb_y1_RD; /**< The y-component of coordinate of motion detection region 1, lower-right point. */
  	/// The x-component of coordinate of motion detection region 2, upper-left point.
  	int 	range_mb_x2_LU; /**< The x-component of coordinate of motion detection region 2, upper-left point. */
  	/// The y-component of coordinate of motion detection region 2, upper-left point.
  	int 	range_mb_y2_LU; /**< The y-component of coordinate of motion detection region 2, upper-left point. */
  	/// The x-component of coordinate of motion detection region 2, lower-right point.
  	int 	range_mb_x2_RD; /**< The x-component of coordinate of motion detection region 2, lower-right point. */
  	/// The y-component of coordinate of motion detection region 2, lower-right point
  	int 	range_mb_y2_RD; /**< The y-component of coordinate of motion detection region 2, lower-right point. */
  	/// The threshold of motion vector in region 0.
  	int	MV_th0;         /**< This variable specifies the threshold of motion vector in region 0. */
  	/// The threshold of sad in region 0.
  	int	sad_th0;        /**< This variable specifies the threshold of sad in region 0. */
  	/// The threshold of delta deviation in region 0.
  	int	delta_dev_th0;  /**< This variable specifies the threshold of delta deviation in region 0. */
  	/// The threshold of motion vector in region 1.
  	int	MV_th1;         /**< This variable specifies the threshold of motion vector in region 1. */
  	/// The threshold of sad in region 1.
  	int	sad_th1;        /**< This variable specifies the threshold of sad in region 1. */
  	/// The threshold of delta deviation in region 1.
  	int	delta_dev_th1;  /**< This variable specifies the threshold of delta deviation in region 1. */
  	/// The threshold of motion vector in region 2.
  	int	MV_th2;         /**< This variable specifies the threshold of motion vector in region 2. */
  	/// The threshold of sad in region 2.
  	int	sad_th2;        /**< This variable specifies the threshold of sad in region 2. */
  	/// The threshold of delta deviation in region 2.
  	int	delta_dev_th2;  /**< This variable specifies the threshold of delta deviation in region 2. */
  	/// The interval of frame/time for motion detection operation.
  	int	md_interval;  /**< This variable specifies the interval of frame/time for motion detection operation. */
} MOTION_INFO;

union VECTOR1
{
	unsigned int u32num;
	struct
	{
		short s16y;
		short s16x;
	} vec;
};



//same as MACROBLOCK_E
typedef struct {
	int16_t quant;  /**< This variable indicates the macroblock level quantization value (1 ~ 31)
		*/
	int16_t mode;  /**< This variable indicates the mode of macroblock.
		*   Possible values are :
		*   <ul>
		*     <li> == INTER_MODE   : Indicate that this macroblock is encoded in INTER mode
		*                                   with 1MV.
		*     <li> == INTER4V_MODE : Indicate that this macroblock is encoded in INTER mode
		*                                   with 4MV.
		*     <li> == INTRA_MODE   : Indicate that this macroblock is encoded in INTRA mode.
		*   </ul>
		*
		*/
	union VECTOR1 mvs[4];/*
		*   - when mode = INTER_MODE :
		*			mvs[n]
		*			--n = 0 ~ 2: invalid.
		*			--n = 3: indicates the motion vector of whole macroblock.
		*   - when mode = INTER4V_MODE :
		*			mvs[n]
		*			--n = 0 ~3: indicates the motion vector of block n within this macroblock.
		*   - when mode = INTRA_MODE :
		*			mvs[n]
		*			--n = 0 ~ 3: invalid.
		*/
	int32_t sad16;        // SAD value for inter-VECTOR
} MACROBLOCK_INFO;


#endif
