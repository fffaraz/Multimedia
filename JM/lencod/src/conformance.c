/*!
 **************************************************************************************
 * \file
 *    conformance.c
 * \brief
 *    Level & Profile related conformance functions
 * \author
 *  Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Karsten Suehring  
 *    - Alexis Michael Tourapis
 * \note
 *
 **************************************************************************************
 */

#include "global.h"
#include "conformance.h"

// Max Frame Size Limit
// Level Limit                          -  -  -  -  -  -  -  -  -  1b  10  11   12   13   -  -  -  -  -  -  20   21   22    -  -  -  -  -  -  -
static const unsigned int  MaxFs [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 99, 99, 396, 396, 396, 0, 0, 0, 0, 0, 0, 396, 792, 1620, 0, 0, 0, 0, 0, 0, 0,  
//                        30    31    32    -  -  -  -  -  -  -  40    41    42    -  -  -  -  -  -  -  50     51    52
                          1620, 3600, 5120, 0, 0, 0, 0, 0, 0, 0, 8192, 8192, 8704, 0, 0, 0, 0, 0, 0, 0, 22080, 36864, 36864 };
static const unsigned int  MinCR [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,  2,  2,   2,   2,   2, 0, 0, 0, 0, 0, 0,   2,   2,   2, 0, 0, 0, 0, 0, 0, 0,  
                             2,    4,    4, 0, 0, 0, 0, 0, 0, 0,    4,    2,    2, 0, 0, 0, 0, 0, 0, 0,     2,     2,     2 };
static const unsigned int  MaxBR [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 64,128, 192, 384, 768, 0, 0, 0, 0, 0, 0,2000,4000,4000, 0, 0, 0, 0, 0, 0, 0,  
                         10000,14000,20000, 0, 0, 0, 0, 0, 0, 0,20000,50000,50000, 0, 0, 0, 0, 0, 0, 0,135000, 240000, 240000 };
static const unsigned int  MaxCPB[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,175,350, 500,1000,2000, 0, 0, 0, 0, 0, 0,2000,4000,4000, 0, 0, 0, 0, 0, 0, 0,  
                         10000,14000,20000, 0, 0, 0, 0, 0, 0, 0,25000,62500,62500, 0, 0, 0, 0, 0, 0, 0,135000, 240000, 240000 };
// Max macroblock processing rate
static const unsigned int MaxMBPS[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1485, 1485, 3000, 6000, 11880, 0, 0, 0, 0, 0, 0, 11880, 19800, 20250, 0, 0, 0, 0, 0, 0, 0,  
                          40500, 108000, 216000, 0, 0, 0, 0, 0, 0, 0, 245760, 245760, 522240, 0, 0, 0, 0, 0, 0, 0, 589824, 983040, 2073600 };
// Vertical MV Limits (integer/halfpel/quarterpel)
// Currently only Integer Pel restrictions are used,
// since the way values are specified
// (i.e. mvlowbound = (levelmvlowbound + 1) and the way
// Subpel ME is performed, subpel will always be within range.
static const int LEVELVMVLIMIT[17][6] =
{
  {  -63,  63,  -128,  127,  -256,  255},
  {  -63,  63,  -128,  127,  -256,  255},
  { -127, 127,  -256,  255,  -512,  511},
  { -127, 127,  -256,  255,  -512,  511},
  { -127, 127,  -256,  255,  -512,  511},
  { -127, 127,  -256,  255,  -512,  511},
  { -255, 255,  -512,  511, -1024, 1023},
  { -255, 255,  -512,  511, -1024, 1023},
  { -255, 255,  -512,  511, -1024, 1023},
  { -511, 511, -1024, 1023, -2048, 2047},
  { -511, 511, -1024, 1023, -2048, 2047},
  { -511, 511, -1024, 1023, -2048, 2047},
  { -511, 511, -1024, 1023, -2048, 2047},
  { -511, 511, -1024, 1023, -2048, 2047},
  { -511, 511, -1024, 1023, -2048, 2047},
  { -511, 511, -1024, 1023, -2048, 2047},
  { -511, 511, -1024, 1023, -2048, 2047}
};

const int LEVELHMVLIMIT[6] =  { -2047, 2047, -4096, 4095, -8192, 8191};

/*!
 ***********************************************************************
 * \brief
 *    Get maximum frame size (in MBs) supported given a level
 ***********************************************************************
 */
unsigned int getMaxFs (unsigned int levelIdc)
{
  unsigned int ret;

  if ( (levelIdc < 9) || (levelIdc > 52))
    error ("getMaxFs: Unknown LevelIdc", 500);

  // in Baseline, Main and Extended: Level 1b is specified with LevelIdc==11 and constrained_set3_flag == 1

  ret = MaxFs[levelIdc];

  if ( 0 == ret )
    error ("getMaxFs: Unknown LevelIdc", 500);

  return ret;
}

/*!
 ***********************************************************************
 * \brief
 *    Get maximum processing rate (in MB/s) supported given a level
 ***********************************************************************
 */
unsigned int getMaxMBPS (unsigned int levelIdc)
{
  unsigned int ret;

  if ( (levelIdc < 9) || (levelIdc > 52))
    error ("getMaxMBPS: Unknown LevelIdc", 500);

  // in Baseline, Main and Extended: Level 1b is specified with LevelIdc==11 and constrained_set3_flag == 1

  ret = MaxMBPS[levelIdc];

  if ( 0 == ret )
    error ("getMaxMBPS: Unknown LevelIdc", 500);

  return ret;
}

/*!
 ***********************************************************************
 * \brief
 *    Get minimum compression ratio supported given a level
 ***********************************************************************
 */
unsigned int getMinCR (unsigned int levelIdc)
{
  unsigned int ret;

  if ( (levelIdc < 9) || (levelIdc > 52))
    error ("getMinCR: Unknown LevelIdc", 500);

  // in Baseline, Main and Extended: Level 1b is specified with LevelIdc==11 and constrained_set3_flag == 1

  ret = MinCR[levelIdc];

  if ( 0 == ret )
    error ("getMinCR: Unknown LevelIdc", 500);

  return ret;
}

/*!
 ***********************************************************************
 * \brief
 *    Get maximum bit rate (in bits/s) supported given a level
 ***********************************************************************
 */
unsigned int getMaxBR (unsigned int levelIdc)
{
  unsigned int ret;

  if ( (levelIdc < 9) || (levelIdc > 52))
    error ("getMaxBR: Unknown LevelIdc", 500);

  // in Baseline, Main and Extended: Level 1b is specified with LevelIdc==11 and constrained_set3_flag == 1

  ret = MaxBR[levelIdc];

  if ( 0 == ret )
    error ("getMaxBR: Unknown LevelIdc", 500);

  return ret;
}

/*!
 ***********************************************************************
 * \brief
 *    Get maximum coded buffer size (in bits) supported given a level
 ***********************************************************************
 */
unsigned int getMaxCPB (unsigned int levelIdc)
{
  unsigned int ret;

  if ( (levelIdc < 9) || (levelIdc > 52))
    error ("getMaxCPB: Unknown LevelIdc", 500);

  // in Baseline, Main and Extended: Level 1b is specified with LevelIdc==11 and constrained_set3_flag == 1

  ret = MaxCPB[levelIdc];

  if ( 0 == ret )
    error ("getMaxCPB: Unknown LevelIdc", 500);

  return ret;
}

/*!
 ***********************************************************************
 * \brief
 *    Check Profile conformance
 ***********************************************************************
 */
void profile_check(InputParameters *p_Inp)
{
  if((p_Inp->ProfileIDC != BASELINE ) &&
     (p_Inp->ProfileIDC != MAIN ) &&
     (p_Inp->ProfileIDC != EXTENDED ) &&
     (p_Inp->ProfileIDC != FREXT_HP    ) &&
     (p_Inp->ProfileIDC != FREXT_Hi10P ) &&
#if (MVC_EXTENSION_ENABLE)
     (p_Inp->ProfileIDC != MULTIVIEW_HIGH )         &&  // MVC multiview high profile
     (p_Inp->ProfileIDC != STEREO_HIGH )         &&  // MVC stereo high profile
#endif
     (p_Inp->ProfileIDC != FREXT_Hi422 ) &&
     (p_Inp->ProfileIDC != FREXT_Hi444 ) &&
     (p_Inp->ProfileIDC != FREXT_CAVLC444 ))
  {
#if (MVC_EXTENSION_ENABLE)
    snprintf(errortext, ET_SIZE, "Profile must be in\n\n  66 (Baseline),\n  77 (Main),\n  88 (Extended),\n 100 (High),\n 110 (High 10 or High 10 Intra)\n"
      " 122 (High 4:2:2 or High 4:2:2 Intra),\n 244 (High 4:4:4 predictive or High 4:4:4 Intra),\n  44 (CAVLC 4:4:4 Intra)\n 118 (MVC profile)\n");
#else
    snprintf(errortext, ET_SIZE, "Profile must be in\n\n  66 (Baseline),\n  77 (Main),\n  88 (Extended),\n 100 (High),\n 110 (High 10 or High 10 Intra)\n"
      " 122 (High 4:2:2 or High 4:2:2 Intra),\n 244 (High 4:4:4 predictive or High 4:4:4 Intra),\n  44 (CAVLC 4:4:4 Intra)\n");
#endif
    error (errortext, 500);
  }

  if ((p_Inp->partition_mode) && (p_Inp->symbol_mode==CABAC))
  {
    snprintf(errortext, ET_SIZE, "Data partitioning and CABAC is not supported in any profile.");
    error (errortext, 500);
  }

  if (p_Inp->redundant_pic_flag)
  {
    if (p_Inp->ProfileIDC != BASELINE)
    {
      snprintf(errortext, ET_SIZE, "Redundant pictures are only allowed in Baseline profile (ProfileIDC = 66).");
      error (errortext, 500);
    }
  }

  if ((p_Inp->partition_mode) && (p_Inp->ProfileIDC!=EXTENDED))
  {
    snprintf(errortext, ET_SIZE, "Data partitioning is only allowed in Extended profile (ProfileIDC = 88).");
    error (errortext, 500);
  }

  if (p_Inp->ChromaIntraDisable && p_Inp->FastCrIntraDecision)
  {
    fprintf( stderr, "\n Warning: ChromaIntraDisable and FastCrIntraDecision cannot be combined together.\n Using only Chroma Intra DC mode.\n");
    p_Inp->FastCrIntraDecision=0;
  }
  
  if ((p_Inp->sp_periodicity) && (p_Inp->ProfileIDC != EXTENDED ))
  {
    snprintf(errortext, ET_SIZE, "SP pictures are only allowed in Extended profile (ProfileIDC = 88).");
    error (errortext, 500);
  }

  // baseline
  if (p_Inp->ProfileIDC == BASELINE )
  {
    if ((p_Inp->NumberBFrames || p_Inp->BRefPictures==2) && p_Inp->PReplaceBSlice == 0)
    {
      snprintf(errortext, ET_SIZE, "B slices are not allowed in Baseline profile (ProfileIDC = 66).");
      error (errortext, 500);
    }
    if (p_Inp->WeightedPrediction)
    {
      snprintf(errortext, ET_SIZE, "Weighted prediction is not allowed in Baseline profile (ProfileIDC = 66).");
      error (errortext, 500);
    }
    if (p_Inp->WeightedBiprediction)
    {
      snprintf(errortext, ET_SIZE, "Weighted prediction is not allowed in Baseline profile (ProfileIDC = 66).");
      error (errortext, 500);
    }
    if (p_Inp->symbol_mode == CABAC)
    {
      snprintf(errortext, ET_SIZE, "CABAC is not allowed in Baseline profile (ProfileIDC = 66).");
      error (errortext, 500);
    }
    if ((p_Inp->PicInterlace) ||(p_Inp->MbInterlace))
    {
      snprintf(errortext, ET_SIZE, "Interlace tools are not allowed in Baseline profile (ProfileIDC = 66).");
      error (errortext, 500);
    }
    if (p_Inp->GenerateMultiplePPS != 0)
    {
      snprintf(errortext, ET_SIZE, "GenerateMultiplePPS is not supported for Baseline profile because it requires enabling Weighted prediction.\n");
      error (errortext, 400);
    }
  }

  // main
  if (p_Inp->ProfileIDC == MAIN )
  {
    if (p_Inp->num_slice_groups_minus1)
    {
      snprintf(errortext, ET_SIZE, "num_slice_groups_minus1>0 (FMO) is not allowed in Main profile (ProfileIDC = 77).");
      error (errortext, 500);
    }
  }

  // extended
  if (p_Inp->ProfileIDC == EXTENDED )
  {
    if (!p_Inp->directInferenceFlag)
    {
      snprintf(errortext, ET_SIZE, "direct_8x8_inference flag must be equal to 1 in Extended profile (ProfileIDC = 88).");
      error (errortext, 500);
    }

    if (p_Inp->symbol_mode == CABAC)
    {
      snprintf(errortext, ET_SIZE, "CABAC is not allowed in Extended profile (ProfileIDC = 88).");
      error (errortext, 500);
    }
  }

  //FRExt
  if ( p_Inp->separate_colour_plane_flag )
  {
    if( p_Inp->yuv_format!=3 )
    {
      fprintf( stderr, "\nWarning: SeparateColourPlane has only effect in 4:4:4 chroma mode (YUVFormat=3),\n         disabling SeparateColourPlane.");
      p_Inp->separate_colour_plane_flag = 0;
    }

    if ( p_Inp->ChromaMEEnable )
    {
      snprintf(errortext, ET_SIZE, "\nChromaMEEnable is not allowed when SeparateColourPlane is enabled.");
      error (errortext, 500);
    }
  }

  // CAVLC 4:4:4 Intra
  if ( p_Inp->ProfileIDC == FREXT_CAVLC444 )
  {
    if ( p_Inp->symbol_mode != CAVLC )
    {
      snprintf(errortext, ET_SIZE, "\nCABAC is not allowed in CAVLC 4:4:4 Intra profile (ProfileIDC = 44).");
      error (errortext, 500);
    }
    if ( !p_Inp->IntraProfile )
    {
      fprintf (stderr, "\nWarning: ProfileIDC equal to 44 implies an Intra only profile, setting IntraProfile = 1.");
      p_Inp->IntraProfile = 1;
    }
  }

  // Intra only profiles
  if (p_Inp->IntraProfile && ( p_Inp->ProfileIDC<FREXT_HP && p_Inp->ProfileIDC!=FREXT_CAVLC444 ))
  {
    snprintf(errortext, ET_SIZE, "\nIntraProfile is allowed only with ProfileIDC %d to %d.", FREXT_HP, FREXT_Hi444);
    error (errortext, 500);
  }

  if (p_Inp->IntraProfile && !p_Inp->idr_period) 
  {
    snprintf(errortext, ET_SIZE, "\nIntraProfile requires IDRPeriod >= 1.");
    error (errortext, 500);
  }

  if (p_Inp->IntraProfile && p_Inp->intra_period != 1) 
  {
    snprintf(errortext, ET_SIZE, "\nIntraProfile requires IntraPeriod equal 1.");
    error (errortext, 500);
  }

  if (p_Inp->IntraProfile && p_Inp->num_ref_frames) 
  {
    fprintf( stderr, "\nWarning: Setting NumberReferenceFrames to 0 in IntraProfile.");
    p_Inp->num_ref_frames = 0;
  }

  if (p_Inp->IntraProfile == 0 && p_Inp->num_ref_frames == 0) 
  {
    snprintf(errortext, ET_SIZE, "\nProfiles other than IntraProfile require NumberReferenceFrames > 0.");
    error (errortext, 500);
  }
}

/*!
 ***********************************************************************
 * \brief
 *    Check if Level constraints are satisfied
 ***********************************************************************
 */
void level_check(VideoParameters *p_Vid, InputParameters *p_Inp)
{
  unsigned int PicSizeInMbs = ( (p_Inp->output.width[0] + p_Vid->auto_crop_right) * (p_Inp->output.height[0] + p_Vid->auto_crop_bottom) ) >> 8;
  unsigned int MBProcessingRate = (unsigned int) (PicSizeInMbs * p_Inp->output.frame_rate + 0.5);
  int cpbBrFactor = ( p_Inp->ProfileIDC >= FREXT_HP ) ? 1500 : 1200;
  
  if ( (p_Inp->LevelIDC>=30) && (p_Inp->directInferenceFlag==0))
  {
    fprintf( stderr, "\nWarning: LevelIDC 3.0 and above require direct_8x8_inference to be set to 1. Please check your settings.\n");
    p_Inp->directInferenceFlag=1;
  }
  if ( ((p_Inp->LevelIDC<21) || (p_Inp->LevelIDC>41)) && (p_Inp->PicInterlace > 0 || p_Inp->MbInterlace > 0) )
  {
    snprintf(errortext, ET_SIZE, "\nInterlace modes only supported for LevelIDC in the range of 21 and 41. Please check your settings.\n");
    error (errortext, 500);
  }

  if ( PicSizeInMbs > getMaxFs(p_Inp->LevelIDC) )
  {
    snprintf(errortext, ET_SIZE, "\nPicSizeInMbs exceeds maximum allowed size at specified LevelIdc %.1f\n", (float) p_Inp->LevelIDC / 10.0);
    error (errortext, 500);
  }
  
  if (p_Inp->IntraProfile && (PicSizeInMbs > 1620) && p_Inp->slice_mode != 1) 
  {
    error ("\nIntraProfile with PicSizeInMbs > 1620 requires SliceMode equal 1.", 500);
  }

  if (p_Inp->IntraProfile && (PicSizeInMbs > 1620) && ((unsigned int)p_Inp->slice_argument > (  getMaxFs(p_Inp->LevelIDC) >> 2 ) ) )
  {
    //when PicSizeInMbs is greater than 1620, the number of macroblocks in any coded slice shall not exceed MaxFS / 4
    snprintf(errortext, ET_SIZE, "\nIntraProfile requires SliceArgument smaller or equal to 1/4 MaxFs at specified LevelIdc %d.", p_Inp->LevelIDC);
    error (errortext, 500);
  }

  if ( MBProcessingRate > getMaxMBPS(p_Inp->LevelIDC) )
  {
    snprintf(errortext, ET_SIZE, "\nMB Processing Rate (%d) exceeds maximum allowed processing rate (%d) at specified LevelIdc %.1f\n", 
      MBProcessingRate, getMaxMBPS(p_Inp->LevelIDC), (float) p_Inp->LevelIDC / 10.0);
    error (errortext, 500);
  }

  if ( p_Inp->bit_rate > (int)(cpbBrFactor * getMaxBR(p_Inp->LevelIDC)) )
  {
    snprintf(errortext, ET_SIZE, "\nBit Rate (%d) exceeds maximum allowed bit rate (%d) at specified LevelIdc %.1f for NAL HRD\n", 
      p_Inp->bit_rate, cpbBrFactor * getMaxBR(p_Inp->LevelIDC), (float) p_Inp->LevelIDC / 10.0);
    error (errortext, 500);
  }
}

/*!
 ***********************************************************************
 * \brief
 *    Update Motion Vector Limits
 ***********************************************************************
 */
void update_mv_limits(VideoParameters *p_Vid, byte is_field)
{
  InputParameters *p_Inp = p_Vid->p_Inp;
  memcpy(p_Vid->MaxVmvR, LEVELVMVLIMIT[p_Vid->LevelIndex], 6 * sizeof(int));
  memcpy(p_Vid->MaxHmvR, LEVELHMVLIMIT, 6 * sizeof(int));
  if (is_field)
  {
    int i;
    for (i = 0; i < 6; i++)
      p_Vid->MaxVmvR[i] = rshift_rnd(p_Vid->MaxVmvR[i], 1);
  }
  if (p_Inp->UseMVLimits)
  {
    p_Vid->MaxVmvR[0] = imax(p_Vid->MaxVmvR[0], -(p_Inp->SetMVYLimit));
    p_Vid->MaxVmvR[1] = imin(p_Vid->MaxVmvR[1],  (p_Inp->SetMVYLimit));
    p_Vid->MaxVmvR[2] = imax(p_Vid->MaxVmvR[2], -(p_Inp->SetMVYLimit << 1));
    p_Vid->MaxVmvR[3] = imin(p_Vid->MaxVmvR[3],  (p_Inp->SetMVYLimit << 1));
    p_Vid->MaxVmvR[4] = imax(p_Vid->MaxVmvR[4], -(p_Inp->SetMVYLimit << 2));
    p_Vid->MaxVmvR[5] = imin(p_Vid->MaxVmvR[5],  (p_Inp->SetMVYLimit << 2));

    p_Vid->MaxHmvR[0] = imax(p_Vid->MaxHmvR[0], -(p_Inp->SetMVXLimit));
    p_Vid->MaxHmvR[1] = imin(p_Vid->MaxHmvR[1],  (p_Inp->SetMVXLimit));
    p_Vid->MaxHmvR[2] = imax(p_Vid->MaxHmvR[2], -(p_Inp->SetMVXLimit << 1));
    p_Vid->MaxHmvR[3] = imin(p_Vid->MaxHmvR[3],  (p_Inp->SetMVXLimit << 1));
    p_Vid->MaxHmvR[4] = imax(p_Vid->MaxHmvR[4], -(p_Inp->SetMVXLimit << 2));
    p_Vid->MaxHmvR[5] = imin(p_Vid->MaxHmvR[5],  (p_Inp->SetMVXLimit << 2));
  }
}


/*!
 ***********************************************************************
 * \brief
 *    Clip motion vector range given encoding level
 ***********************************************************************
 */
void clip_mv_range(VideoParameters *p_Vid, int search_range, MotionVector *mv, int res)
{
  res <<= 1;

  mv->mv_x = (short) iClip3( p_Vid->MaxHmvR[0 + res] + search_range, p_Vid->MaxHmvR[1 + res] - search_range, mv->mv_x);
  mv->mv_y = (short) iClip3( p_Vid->MaxVmvR[0 + res] + search_range, p_Vid->MaxVmvR[1 + res] - search_range, mv->mv_y);
}

/*!
 ***********************************************************************
 * \brief
 *    Clip motion vector range given encoding level
 ***********************************************************************
 */
void test_clip_mvs(VideoParameters *p_Vid, MotionVector *mv, Boolean write_mb)
{
  if ((mv->mv_x < p_Vid->MaxHmvR[4]) || (mv->mv_x > p_Vid->MaxHmvR[5]) || (mv->mv_y < p_Vid->MaxVmvR[4]) || (mv->mv_y > p_Vid->MaxVmvR[5]))
  {
    if (write_mb == TRUE)
      printf("Warning MVs (%d %d) were out of range x(%d %d) y(%d %d). Clipping mvs before writing\n", mv->mv_x, mv->mv_y, p_Vid->MaxHmvR[4], p_Vid->MaxHmvR[5], p_Vid->MaxVmvR[4], p_Vid->MaxVmvR[5]);
    mv->mv_x = (short) iClip3( p_Vid->MaxHmvR[4], p_Vid->MaxHmvR[5], mv->mv_x);
    mv->mv_y = (short) iClip3( p_Vid->MaxVmvR[4], p_Vid->MaxVmvR[5], mv->mv_y);
  }
}
  
/*!
 ***********************************************************************
 * \brief
 *    Clip motion vector range given encoding level
 ***********************************************************************
 */
int out_of_bounds_mvs(VideoParameters *p_Vid, const MotionVector *mv)
{
  return ((mv->mv_x < p_Vid->MaxHmvR[4]) || (mv->mv_x > p_Vid->MaxHmvR[5]) || (mv->mv_y < p_Vid->MaxVmvR[4]) || (mv->mv_y > p_Vid->MaxVmvR[5]));
}

int InvalidWeightsForBiPrediction(Slice *currSlice, Block8x8Info* b8x8info, int mode)
{
  seq_parameter_set_rbsp_t *active_sps = currSlice->active_sps;
  int cur_blk, cur_comp;
  int best8x8l0ref, best8x8l1ref;
  int weight_sum = 0;
  int invalid_mode = 0;
  short *wbp0, *wbp1;
  for (cur_blk = 0; cur_blk < 4; cur_blk ++)
  {
    if (b8x8info->best[mode][cur_blk].pdir == 2)
    { 
      best8x8l0ref = (int) b8x8info->best[mode][cur_blk].ref[LIST_0];
      best8x8l1ref = (int) b8x8info->best[mode][cur_blk].ref[LIST_1];
      wbp0 = &currSlice->wbp_weight[LIST_0][best8x8l0ref][best8x8l1ref][0];
      wbp1 = &currSlice->wbp_weight[LIST_1][best8x8l0ref][best8x8l1ref][0];

      for (cur_comp = 0; cur_comp < (active_sps->chroma_format_idc == YUV400 ? 1 : 3) ; cur_comp ++)
      {
        weight_sum = *wbp0++ + *wbp1++;

        if (weight_sum < -128 ||  weight_sum > 127) 
        {
          invalid_mode = 1;
          break;
        }
      }
      if (invalid_mode == 1)
        break;
    }
  }
  return invalid_mode;
}

int InvalidMotionVectors(VideoParameters *p_Vid, Slice *currSlice, Block8x8Info* b8x8info, int mode)
{
  int cur_blk;
  int l0ref, l1ref;
  int invalid_mode = 0;
  int i, j;

  if (mode > P8x8)
    return invalid_mode;

  // Brute force method. Note that this ignores currently subpartitions in 8x8 modes
  for (cur_blk = 0; cur_blk < 4; cur_blk ++)
  {
    i = (cur_blk & 0x01) << 1;
    j = (cur_blk >> 1) << 1;
    switch (b8x8info->best[mode][cur_blk].pdir)
    {
    case 0:
      l0ref = (int) b8x8info->best[mode][cur_blk].ref[LIST_0];
      if (out_of_bounds_mvs(p_Vid, &currSlice->all_mv [LIST_0][l0ref][mode][j][i]))
      {
        invalid_mode = 1;
        return invalid_mode;
      }
      break;
    case 1:
      l1ref = (int) b8x8info->best[mode][cur_blk].ref[LIST_1];
      if (out_of_bounds_mvs(p_Vid, &currSlice->all_mv [LIST_1][l1ref][mode][j][i]))
      {
        invalid_mode = 1;
        return invalid_mode;
      }
      break;
    case 2:
      l0ref = (int) b8x8info->best[mode][cur_blk].ref[LIST_0];
      l1ref = (int) b8x8info->best[mode][cur_blk].ref[LIST_1];
      if (out_of_bounds_mvs(p_Vid, &currSlice->all_mv [LIST_0][l0ref][mode][j][i]))
      {
        invalid_mode = 1;
        return invalid_mode;
      }
      if (out_of_bounds_mvs(p_Vid, &currSlice->all_mv [LIST_1][l1ref][mode][j][i]))
      {
        invalid_mode = 1;
        return invalid_mode;
      }
      break;
    default:
      break;
    }
  }

  return invalid_mode;
}

Boolean CheckPredictionParams(Macroblock  *currMB, Block8x8Info *b8x8info, int mode)
{
  Slice *currSlice = currMB->p_Slice;
  // check if all sub-macroblock partitions can be used with 8x8 transform
  if (mode == P8x8 && currMB->luma_transform_size_8x8_flag == TRUE)
  {
    int i;

    for (i = 0; i < 4; i++)
    {
      if (b8x8info->best[P8x8][i].mode != 4 &&  b8x8info->best[P8x8][i].mode != 0)
      {
        return FALSE;
      }
    }
  }

  if (InvalidMotionVectors(currSlice->p_Vid, currSlice, b8x8info, mode))
    return FALSE;

  if (currSlice->slice_type == B_SLICE)
  {
    // check if weights are in valid range for biprediction.
    if (currSlice->weighted_prediction == 1 &&  mode < P8x8) 
    {
      if (InvalidWeightsForBiPrediction(currSlice, b8x8info, mode))
        return FALSE;
    }

    // Check if direct mode can be utilized for this partition
    if (mode==0)
    {
      int i, j;
      for (j = currMB->block_y; j < currMB->block_y + 4;j++)
      {
        for (i = currMB->block_x; i < currMB->block_x + 4;i++)
        {
          if (currSlice->direct_pdir[j][i] < 0)  // direct_pdir should be moved and become part of p_Vid parameters
            return FALSE;
        }
      }
    }

  }

  return TRUE;
}
