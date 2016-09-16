#ifndef __FMJPEG_AVCODEC_h
  #define __FMJPEG_AVCODEC_h


#define   	FMJPEG_IOCTL_DECODE_CREATE		0x5171
#define 	FMJPEG_IOCTL_DECODE_ONE			0x5178
#define     FMJPEG_IOCTL_DECODE_PASSHUF          0x5180

#define  	FMJPEG_IOCTL_ENCODE_CREATE    	0x5182
#define   	FMJPEG_IOCTL_ENCODE_ONE			0x5187
#define     FMJPEG_IOCTL_ENCODE_DEVBUF		0x5194

/**
 *  By using typedef to create a type name for DMA memory allocation function.
 *  And user can allocate memory on a specified alignment memory 
 *  by using the parameter align_size.
 *
 *  @param size is the bytes to allocate.
 *  @param align_size is the alignment value which must be power of 2.
 *  @param reserved_size is the specifed cache line.
 *  @param phy_ptr is used to return the physical address of allocated aligned memory block.
 *  @return return a void pointer to virtual address of the allocated memory block.
 *  @see FJPEG_DEC_PARAM
 *  @see DMA_FREE_PTR
 */
typedef void *(* DMA_MALLOC_PTR)(unsigned int size,unsigned char align_size,unsigned char reserved_size, void ** phy_ptr);
typedef void *(* MALLOC_PTR)(unsigned int size, unsigned char align_size, unsigned char reserved_size);
/**
 *  By using typedef to create a type name for DMA memory free function.
 *  And user can use this type of function to free a block of memory that
 *  was allocated by the DMA_MALLOC_PTR type of function.
 *
 *  @param virt_ptr is a pointer to the memory block with virtual address that was returned
 *                  previously by the type of DMA_MALLOC_PTR function.
 *  @param phy_ptr is a pointer to the memory block with physical address that was returned
 *                 previously by the type of DMA_MALLOC_PTR function.
 *  @return return void.
 *  @see FJPEG_DEC_PARAM
 *  @see DMA_MALLOC_PTR
 */
typedef void (* DMA_FREE_PTR)(void * virt_ptr, void * phy_ptr);
typedef void (* FREE_PTR)(void * virt_ptr);

/// The Faraday JPEG Decoder Parameters Structure.
/**
 *  While creating jpeg decoder object by using FJpegDecCreate() operation, FJPEG_DEC_PARAM
 *  pointer is served as FJpegDecCreate()'s parameter to internally initialize related JPEG decoder
 *  object settings. The data member pu32BaseAddr and pu8BitstreamAddr must be set before
 *  invoking the FJpegDecCreate() operation.\n
 *
 *  This data structure is also used for FJpegDecDecode() operation where the data member
 *  u8NumComponents and pu8YUVAddr must be set before invoking the FJpegDecDecode() operation.
 *  
 *
 *  See  @ref jpeg_decoder_ops_grp \n
 *  @see FJpegDecCreate
 *  
 */

typedef struct {
  /// The base address of hardware core.
  unsigned int  *pu32BaseAddr;    /**< User can use this variable to set the base
                                   *   address of hardware core.
                                   *
                                   *   @see pu8BitstreamAddr
                                   */                                   
											 
  /// The input bitstream buffer physical address while decoding the jpeg bitstream.
  unsigned char *pu8BitstreamAddr;  /**< To set input bitstream buffer's physical address.\n
                                     *   This bitstream buffer was provided by user and used for JPEG decoder while
                                     *   decoding JPEG bitstream.\n
                                     *   <B>N.B.</B> : the input bitstream buffer address must be <B>physical address</B> with <B>4-byte aligned</B>.
                                     *
                                     *  @see pu32BaseAddr
                                     */
  
  unsigned int frame_width;     		/* width of frame buffer */
  unsigned int frame_hight;      		/* height of frame buffer */
  unsigned char * bs_buffer_phy;       /* input bitstream DMA buffer */
   /// The function pointer to user-defined DMA memory allocation function.
  DMA_MALLOC_PTR pfnDmaMalloc;  /**< This variable contains the function pointer to the user-defined 
                                 *   DMA malloc function since under OS environment, our hardware device
                                 *   may need the physical address instead of virtual address.
                                 *
                                 *   @see pfnDmaFree
                                 *   @see DMA_MALLOC_PTR
                                 *   @see DMA_FREE_PTR
                                 */
  /// The function pointer to user-defined DMA free allocation function.
  DMA_FREE_PTR   pfnDmaFree;    /**< This variable contains the function pointer to the user-defined 
                                 *   DMA free function since under OS environment, our hardware device
                                 *   may need the physical address instead of virtual address.
                                 *
                                 *   @see pfnDmaFree
                                 *   @see DMA_MALLOC_PTR
                                 *   @see DMA_FREE_PTR
                                 */
   MALLOC_PTR 	pfnMalloc;
   FREE_PTR  	         pfnFree;								 
   unsigned char        u32CacheAlign;     
   unsigned int static_jpg;				/* display one picture */
} FJPEG_DEC_PARAM;

typedef struct {
    unsigned char  *pu8YUVAddr[3];  
    unsigned char  * buf;
    unsigned int 	    buf_size;
    unsigned char  u8NumComponents; 	
}FJPEG_DEC_FRAME;


/// The Faraday JPEG Decoder Result Image Descriptor (YCbCr).
/**
 *  After reading the jpeg header , some side informations about the decoded YUV image
 *  were generated and reflected within FJPEG_DEC_RESULT structure after 
 *  calling FJpegDecReadHeader() operation. \n 
 *
 *  @see FJpegDecReadHeader
 *  
 */
typedef struct {
  /// The image width in pixels.
  unsigned int   u32ImageWidth;    /**< This member variable indicates the actual image width after 
                                    *   JPEG decoding, which is extracted from 'SOF' marker directly.
                                    */
  /// The image height in pixels.
  unsigned int   u32ImageHeight;   /**< This member variable indicates the actual image height after
                                    *   JPEG decoding, which is extracted from 'SOF' marker directly.
                                    */
  /// The number of components in the decoding image.
  unsigned char  u8NumComponents;  /**< This variable shows the number of components in the decoding
                                    *   image, which is also extracted from 'SOF' marker directly.
                                    *   The color space used for these components is <B>YCbCr</B>.
                                    *   If only one component is used, the component will be <B>Y</B> only.
                                    */
  /// The array of structures to describe each component information about decoded image.  
  struct {
    /// The horizontal sampling frequency of this component.
    unsigned char m_u8HSamplingFrequency;  /**< horizontal sampling frequency factor (1..4) for this component.*/
    /// The vertical sampling frequency of this component.
    unsigned char m_u8VSamplingFrequency;  /**< vertical sampling frequency factor (1..4) for this component.  */
    
    // The address where this component is stored.
    //unsigned char *m_pComponent;           /**< the address where this component is stored  */
    
    /// This component's width in pixels.
    unsigned int m_u32ComponentWidth;      /**< this component horizontal sample width in pixels */
    /// This component's height in pixels.
    unsigned int m_u32ComponentHeight;     /**< this component's vertical sample width in pixels */
        
    /// This component's total size in bytes = (m_u32ComponentWidth * m_u32ComponentHeight).
    unsigned int m_u32ComponentTotalSize;  /**< this component's total size in bytes , which is equal to (m_u32ComponentWidth * m_u32ComponentHeight) */    
  } rgComponentInfo[3];  /**<
                          *  According to JFIF standard, the color space is restricted to one or three components. \n
                          *  For three components, <B>YCbCr</B> is used. \n
                          *  If only one component is used, the component will be <B>Y</B>. \n
                          *  Thus, depending on the number of components (indicated by member vairable 'u8NumComponents'), 
                          *  the following array of component descriptor 'rgComponentInfo' structure are in the 
                          *  order <B>{ Y, Cb,Cr }</B> respectively.
                          *  
                          *  @see u8NumComponents
                          */
  
} FJPEG_DEC_RESULT; 

typedef struct {
	int	act0;
	int   	act1;
	int 	act2;
} FJPEG_MD_RESULT;
/// The Faraday JPEG Encoder Parameters Structure.
/**
 *  While creating jpeg encoder object by using FJpegEncCreate() operation, FJPEG_ENC_PARAM 
 *  pointer is served as FJpegEncCreate()'s parameter to internally initialize related JPEG encoder
 *  object settings.\n
 *  See  @ref jpeg_encoder_ops_grp \n
 *  @see FJpegEncCreate
 *  
 */
typedef struct {
  /// The base address of hardware core.
  unsigned int  *pu32BaseAddr;    /**< User can use this variable to set the base
                                   *   address of hardware core.
                                   */                                     
  /// The base address for input YUV buffer.
  unsigned char *pu8YUVAddr[3];  /**< To set input YUV frame buffer's base address.\n
                                  *   <B>N.B.</B> : the address must be physical address with <B>8-byte aligned</B>.
                                  *   @see pu32BaseAddr
                                  *   @see pu8BitstreamAddr
                                  */
  /// The number of components in the input YUV image.
  unsigned char  u8NumComponents;  /**< This variable tells the number of components in the input
                                    *   YUV image. The color space used for these components
                                    *   is <B>YCbCr</B>. If only one component is used, the
                                    *   component will be <B>Y</B> only.
                                    *   @see rgComponentInfo
                                    */
  /// The array of structures to describe each component information about input YUV image.  
  struct {
    /// The horizontal sampling frequency of this component.
    unsigned char m_u8HSamplingFrequency;  /**< horizontal sampling frequency factor (1..4) for this component.*/
    /// The vertical sampling frequency of this component.
    unsigned char m_u8VSamplingFrequency;  /**< vertical sampling frequency factor (1..4) for this component.  */
  } rgComponentInfo[3];  /**<
                          *  According to JFIF standard, the color space is restricted to one or three components. \n
                          *  For three components, <B>YCbCr</B> is used. \n
                          *  If only one component is used, the component will be <B>Y</B>. \n
                          *  Thus, depending on the number of components (indicated by member vairable 'u8NumComponents'), 
                          *  the following array of component descriptor 'rgComponentInfo' structure are in the 
                          *  order <B>{ Y, Cb,Cr }</B> respectively.
                          *  
                          *  Currently, the YUV formats that JPEG encoder supports now are shown below :
                          *  -  Each component's horizontal and vertical smapling factors are representing
                          *     in the form '( <B>HxV</B> )' , where <B>H</B> is horizontal sampling factor
                          *     and <B>V</B> is vertical sampling factor:
                          *    - '420' :  Y ( <B>2x2</B> ) , U ( <B>1x1</B> ) , V ( <B>1x1</B> )
                          *    - '422' :  Y ( <B>4x1</B> ) , U ( <B>2x1</B> ) , V ( <B>2x1</B> )
                          *    - '211' :  Y ( <B>2x1</B> ) , U ( <B>1x1</B> ) , V ( <B>1x1</B> )
                          *    - '333' :  Y ( <B>3x1</B> ) , U ( <B>3x1</B> ) , V ( <B>3x1</B> )
                          *    - '222' :  Y ( <B>2x1</B> ) , U ( <B>2x1</B> ) , V ( <B>2x1</B> )
                          *    - '111' :  Y ( <B>1x1</B> ) , U ( <B>1x1</B> ) , V ( <B>1x1</B> )
                          *
                          *  @see u8NumComponents
                          */
  /// The image quality setting (0 ~ 100)
  unsigned int   u32ImageQuality;  /**< This variable can set the encoded image quality.
                                    *   The higher the image quality setting, the closer the encoded
                                    *   image will be to the original input YUV image, and , of course,
                                    *   the larger the JPEG file.
                                    *   The range for this quality setting is between 0 (worst) to 100 (best).
                                    */
  /// The restart marker interval.                                 
  unsigned int   u32RestartInterval; /**< This variable specifies the number of MCUs between
                                      *   restart markers within the compressed data.
                                      *   If the specified restart interval is zero, then
                                      *   the restart marker is not used.
                                      */										
  /// The image width in pixels.
  unsigned int   u32ImageWidth;    /**< This variable indicates the actual input image width.
                                    *   
                                    */
  /// The image height in pixels.
  unsigned int   u32ImageHeight;   /**< This variable indicates the actual input image height.
                                    *   
                                    */
                                    
  /// The output bitstream buffer address while encoding the input YUV image.
  unsigned char *pu8BitstreamAddr;  /**< To set output bitstream buffer's address.\n
                                     *   This bitstream buffer was provided by user and used
                                     *   for JPEG encoder while encoding input YUV image.\n
                                     *   <B>N.B.</B> : the input bitstream buffer address must be <B>physical address</B> with <B>4-byte aligned</B>.
                                     *                                     
                                     */
                                     
  /// The function pointer to user-defined DMA memory allocation function.
  DMA_MALLOC_PTR pfnDmaMalloc;  /**< This variable contains the function pointer to the user-defined 
                                 *   DMA malloc function since under OS environment, our hardware device
                                 *   may need the physical address instead of virtual address.
                                 *
                                 *   @see pfnDmaFree
                                 *   @see DMA_MALLOC_PTR
                                 *   @see DMA_FREE_PTR
                                 */
  /// The function pointer to user-defined DMA free allocation function.
  DMA_FREE_PTR   pfnDmaFree;    /**< This variable contains the function pointer to the user-defined 
                                 *   DMA free function since under OS environment, our hardware device
                                 *   may need the physical address instead of virtual address.
                                 *
                                 *   @see pfnDmaFree
                                 *   @see DMA_MALLOC_PTR
                                 *   @see DMA_FREE_PTR
                                 */
   MALLOC_PTR 	pfnMalloc;
   FREE_PTR  	pfnFree;		                              
   unsigned char u32CacheAlign;			
 
   unsigned int bitstream_size;			/* vitstream allocated size */
   unsigned char* bs_buffer_virt;			/* bitstream virtual address */
   unsigned char * bs_buffer_phy;			/* bitstream physical address */
   unsigned int u32ImageMotionDetection;	/* enable or disable motion detection function */
   unsigned char u8JPGPIC; 				/* take one jpeg picture */
   unsigned char u82D;			        /* 0 for MP4 2D, 1 for jpg sequencial, 2 for H.264 2D */
   unsigned int    mdt1;
   unsigned int    mdt2;
   unsigned int    mdt3;
   unsigned int    mdt4;
   unsigned int    mdt5;
   unsigned int    active0;					/* motion detection value */
   unsigned int    active1;
   unsigned int    active2;
   //for ROI data structure 
   // the flag to indicate whether the ROI encoding is enabled or not
   int      roi_enable;
   // ROI's (region of interest) upper-left corner coordinate(x,y) of captured frame.
   unsigned int roi_left_x;
   unsigned int roi_left_y;
   // ROI's (region of interest) lower-rigth corner coordinate(x,y) of captured frame.
   unsigned int roi_right_x;
   unsigned int roi_right_y;
   int rsvd[16]; // size must sync to MOTION_DET_INFO
} FJPEG_ENC_PARAM;

typedef struct {
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
  /// The threshold of delta deviation in region 0.
  int	delta_dev_th0;  /**< This variable specifies the threshold of delta deviation in region 0. */
  /// The threshold of delta deviation in region 1.
  int	delta_dev_th1;  /**< This variable specifies the threshold of delta deviation in region 1. */
  /// The threshold of delta deviation in region 2.
  int	delta_dev_th2;  /**< This variable specifies the threshold of delta deviation in region 2. */
  /// The interval of frame/time for motion detection operation.
  int	md_interval;  /**< This variable specifies the interval of frame/time for motion detection operation. */
} MOTION_DET_INFO;
#endif

