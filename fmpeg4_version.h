#ifndef _FMPEG4_VERSION_H_
#define _FMPEG4_VERSION_H_
/*
Version 1.0: Add H263 support, fix encoder ME copy timming issue
Version 1.1: Add motion dection and ROI setting
Version 1.2: optimized motion dection
             fix consistent_free parameter bug
Version 1.3: add key check function
Version 1.5: Update architecture
Version 1.6: Update from TC for GM8180
Version 1.9: Add module_time_base
Version 2.0: Upgrade to GM Version, add check version type
Version 2.1: Add special mpeg4 deinterlace display feature for GM8120
Version 2.2: Update for data sync issue
Version 2.3: Update for data sync issue 2
 */
#define MP4VER          0x00020003
#define MP4VER_MAJOR    (MP4VER>>16)
#define MP4VER_MINOR    (MP4VER&0xffff)

#endif
