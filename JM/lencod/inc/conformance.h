
/*!
 ************************************************************************
 * \file conformance.h
 *
 * \brief
 *   Level & Profile Related definitions  
 *
 * \author
 *    Alexis Michael Tourapis         <alexismt@ieee.org>       \n
 *
 ************************************************************************
 */

#ifndef _CONFORMANCE_H_
#define _CONFORMANCE_H_

extern void    profile_check        (InputParameters *p_Inp);
extern void    level_check          (VideoParameters *p_Vid, InputParameters *p_Inp);
extern void    update_mv_limits     (VideoParameters *p_Vid, byte is_field);
extern void    clip_mv_range        (VideoParameters *p_Vid, int search_range, MotionVector *mv, int res);
extern int     out_of_bounds_mvs    (VideoParameters *p_Vid, const MotionVector *mv);
extern void    test_clip_mvs        (VideoParameters *p_Vid, MotionVector *mv, Boolean write_mb);
extern Boolean CheckPredictionParams(Macroblock  *currMB, Block8x8Info *b8x8info, int mode);

extern unsigned int getMaxMBPS(unsigned int levelIdc);
extern unsigned int getMinCR  (unsigned int levelIdc);
extern unsigned int getMaxBR  (unsigned int levelIdc);
extern unsigned int getMaxCPB (unsigned int levelIdc);

#endif

