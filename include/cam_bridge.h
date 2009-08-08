/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#ifndef CAM_BRIDGE_HH
#define CAM_BRIDGE_HH

#include "sensor_msgs/RawStereo.h"
#include "sensor_msgs/FillImage.h"
#include "image.h"

namespace cam_bridge
{
  void CamDataToRawStereo(cam::ImageData* im, sensor_msgs::Image& im_msg, sensor_msgs::CameraInfo& info_msg, uint8_t& type)
  {
    if (im->imRawType != COLOR_CODING_NONE)
    {
      fillImage(im_msg, sensor_msgs::Image::TYPE_8UC1, im->imHeight, im->imWidth, im->imWidth, im->imRaw);
      type = sensor_msgs::RawStereo::IMAGE_RAW;
    }
    else if (im->imType != COLOR_CODING_NONE)
    {
      fillImage(im_msg, sensor_msgs::Image::TYPE_8UC1, im->imHeight, im->imWidth, im->imWidth, im->im);
      type = sensor_msgs::RawStereo::IMAGE;
    }
    else if (im->imColorType != COLOR_CODING_NONE && im->imColorType == COLOR_CODING_RGBA8)
    {
      fillImage(im_msg, sensor_msgs::Image::TYPE_8UC4, im->imHeight, im->imWidth, 4 * im->imWidth, im->imColor);
      type = sensor_msgs::RawStereo::IMAGE_COLOR;
    }
    else if (im->imColorType != COLOR_CODING_NONE && im->imColorType == COLOR_CODING_RGB8)
    {
      fillImage(im_msg, sensor_msgs::Image::TYPE_8UC3, im->imHeight, im->imWidth, 3 * im->imWidth, im->imColor);
      type = sensor_msgs::RawStereo::IMAGE_COLOR;
    }
    else if (im->imRectType != COLOR_CODING_NONE)
    {
      fillImage(im_msg, sensor_msgs::Image::TYPE_8UC1, im->imHeight, im->imWidth, im->imWidth, im->imRect);
      type = sensor_msgs::RawStereo::IMAGE_RECT;
    }
    else if (im->imRectColorType != COLOR_CODING_NONE && im->imRectColorType == COLOR_CODING_RGBA8)
    {
      fillImage(im_msg, sensor_msgs::Image::TYPE_8UC4, im->imHeight, im->imWidth, 4 * im->imWidth, im->imRectColor);
      type = sensor_msgs::RawStereo::IMAGE_RECT_COLOR;
    }
    else if (im->imRectColorType != COLOR_CODING_NONE && im->imRectColorType == COLOR_CODING_RGB8)
    {
      fillImage(im_msg, sensor_msgs::Image::TYPE_8UC3, im->imHeight, im->imWidth, 3 * im->imWidth, im->imRectColor);
      type = sensor_msgs::RawStereo::IMAGE_RECT_COLOR;
    }

    info_msg.height = im->imHeight;
    info_msg.width  = im->imWidth;

    memcpy((char*)(&info_msg.D[0]), (char*)(im->D),  5*sizeof(double));
    memcpy((char*)(&info_msg.K[0]), (char*)(im->K),  9*sizeof(double));
    memcpy((char*)(&info_msg.R[0]), (char*)(im->R),  9*sizeof(double));
    memcpy((char*)(&info_msg.P[0]), (char*)(im->P), 12*sizeof(double));
  }

  void StereoDataToRawStereo(cam::StereoData* stIm, sensor_msgs::RawStereo& raw_stereo)
  {
    raw_stereo.header.stamp = ros::Time().fromNSec(stIm->imLeft->im_time * 1000);

    if (stIm->hasDisparity)
    {
      fillImage(raw_stereo.disparity_image,
                sensor_msgs::Image::TYPE_16SC1,
                stIm->imWidth,
                stIm->imHeight,
                2 * stIm->imWidth,
                stIm->imDisp );
      
      raw_stereo.has_disparity = true;

      raw_stereo.disparity_info.height = stIm->imHeight;
      raw_stereo.disparity_info.width = stIm->imWidth;

      raw_stereo.disparity_info.dpp = stIm->dpp;
      raw_stereo.disparity_info.num_disp = stIm->numDisp;
      raw_stereo.disparity_info.im_Dtop = stIm->imDtop;
      raw_stereo.disparity_info.im_Dleft = stIm->imDleft;
      raw_stereo.disparity_info.im_Dwidth = stIm->imDwidth;
      raw_stereo.disparity_info.im_Dheight = stIm->imDheight;
      raw_stereo.disparity_info.corr_size = stIm->corrSize;
      raw_stereo.disparity_info.filter_size = stIm->filterSize;
      raw_stereo.disparity_info.hor_offset = stIm->horOffset;
      raw_stereo.disparity_info.texture_thresh = stIm->textureThresh;
      raw_stereo.disparity_info.unique_thresh = stIm->uniqueThresh;
      raw_stereo.disparity_info.smooth_thresh = stIm->smoothThresh;
      raw_stereo.disparity_info.speckle_diff = stIm->speckleDiff;
      raw_stereo.disparity_info.speckle_region_size = stIm->speckleRegionSize;
      raw_stereo.disparity_info.unique_check = stIm->unique_check;

    } else {
      clearImage(raw_stereo.disparity_image);
      raw_stereo.has_disparity = false;
    }

    raw_stereo.stereo_info.height = stIm->imHeight;
    raw_stereo.stereo_info.width = stIm->imWidth;
    memcpy((char*)(&raw_stereo.stereo_info.T[0]),  (char*)(stIm->T),   3*sizeof(double));
    memcpy((char*)(&raw_stereo.stereo_info.Om[0]), (char*)(stIm->Om),  3*sizeof(double));
    memcpy((char*)(&raw_stereo.stereo_info.RP[0]), (char*)(stIm->RP), 16*sizeof(double));

    CamDataToRawStereo(stIm->imLeft,  raw_stereo.left_image,  raw_stereo.left_info,  raw_stereo.left_type);
    CamDataToRawStereo(stIm->imRight, raw_stereo.right_image, raw_stereo.right_info, raw_stereo.right_type);
  }


  void extractImage(std::vector<uint8_t> data, size_t* sz, uint8_t **d)
  {
    size_t new_size = data.size();

    if (*sz < new_size)
    {
      MEMFREE(*d);
      *d = (uint8_t *)MEMALIGN(new_size);
      *sz = new_size;
    }
    memcpy((char*)(*d), (char*)(&data[0]), new_size);
  }
  void extractImage(std::vector<uint8_t> data, size_t* sz, int16_t **d)
  {
    size_t new_size = data.size();

    if (*sz < new_size)
    {
      MEMFREE(*d);
      *d = (int16_t *)MEMALIGN(new_size);
      *sz = new_size;
    }
    memcpy((char*)(*d), (char*)(&data[0]), new_size);
  }

  void RawStereoToCamData(sensor_msgs::Image& im_msg, sensor_msgs::CameraInfo& info_msg, uint8_t type, cam::ImageData* im)
  {

    im->imRawType = COLOR_CODING_NONE;
    im->imType = COLOR_CODING_NONE;
    im->imColorType = COLOR_CODING_NONE;
    im->imRectType = COLOR_CODING_NONE;
    im->imRectColorType = COLOR_CODING_NONE;

    if (type == sensor_msgs::RawStereo::IMAGE_RAW)
    {
      extractImage(im_msg.data, &im->imRawSize, &im->imRaw);
      im->imRawType = COLOR_CODING_BAYER8_GRBG;
    }
    else if (type == sensor_msgs::RawStereo::IMAGE)
    {
      extractImage(im_msg.data, &im->imSize, &im->im);
      im->imType = COLOR_CODING_MONO8;
    }
    else if (type == sensor_msgs::RawStereo::IMAGE_COLOR && im_msg.type == sensor_msgs::Image::TYPE_8UC4)
    {
      extractImage(im_msg.data, &im->imColorSize, &im->imColor);
      im->imColorType = COLOR_CODING_RGBA8;
    }
    else if (type == sensor_msgs::RawStereo::IMAGE_COLOR && im_msg.type == sensor_msgs::Image::TYPE_8UC3)
    {
      extractImage(im_msg.data, &im->imColorSize, &im->imColor);
      im->imColorType = COLOR_CODING_RGB8;
    }
    else if (type == sensor_msgs::RawStereo::IMAGE_RECT)
    {
      extractImage(im_msg.data, &im->imRectSize, &im->imRect);
      im->imRectType = COLOR_CODING_MONO8;
    }
    else if (type == sensor_msgs::RawStereo::IMAGE_RECT_COLOR && im_msg.type == sensor_msgs::Image::TYPE_8UC4)
    {
      extractImage(im_msg.data, &im->imRectColorSize, &im->imRectColor);
      im->imRectColorType = COLOR_CODING_RGBA8;
    }
    else if (type == sensor_msgs::RawStereo::IMAGE_RECT_COLOR && im_msg.type == sensor_msgs::Image::TYPE_8UC3)
    {
      extractImage(im_msg.data, &im->imRectColorSize, &im->imRectColor);
      im->imRectColorType = COLOR_CODING_RGB8;
    }

    // FIXME: this to avoid segfault when right image empty (disparity image requested instead).
    //        this all seems kind of hacky
    if (im_msg.type == sensor_msgs::Image::TYPE_8UC1) {
      im->imHeight = im_msg.height;
      im->imWidth  = im_msg.width;
    } else {
      im->imHeight = info_msg.height;
      im->imWidth  = info_msg.width;
    }

    memcpy((char*)(im->D), (char*)(&info_msg.D[0]),  5*sizeof(double));
    memcpy((char*)(im->K), (char*)(&info_msg.K[0]),  9*sizeof(double));
    memcpy((char*)(im->R), (char*)(&info_msg.R[0]),  9*sizeof(double));
    memcpy((char*)(im->P), (char*)(&info_msg.P[0]),  12*sizeof(double));
    im->hasRectification = true;
  }

  void RawStereoToStereoData(sensor_msgs::RawStereo& raw_stereo, cam::StereoData* stIm)
  {
    stIm->imLeft->im_time = raw_stereo.header.stamp.toNSec() / 1000;
    stIm->imRight->im_time = raw_stereo.header.stamp.toNSec() / 1000;
    stIm->setSize(raw_stereo.stereo_info.width, raw_stereo.stereo_info.height);
   
    stIm->hasDisparity = false;
 
    if (raw_stereo.has_disparity)
    {
      extractImage(raw_stereo.disparity_image.data, &stIm->imDispSize, &stIm->imDisp);
      stIm->hasDisparity = true;

      stIm->dpp                 =     raw_stereo.disparity_info.dpp;
      stIm->numDisp             =     raw_stereo.disparity_info.num_disp;
      stIm->imDtop              =     raw_stereo.disparity_info.im_Dtop;
      stIm->imDleft             =     raw_stereo.disparity_info.im_Dleft;
      stIm->imDwidth            =     raw_stereo.disparity_info.im_Dwidth;
      stIm->imDheight           =     raw_stereo.disparity_info.im_Dheight;
      stIm->corrSize            =     raw_stereo.disparity_info.corr_size;
      stIm->filterSize          =     raw_stereo.disparity_info.filter_size;
      stIm->horOffset           =     raw_stereo.disparity_info.hor_offset;
      stIm->textureThresh       =     raw_stereo.disparity_info.texture_thresh;
      stIm->uniqueThresh        =     raw_stereo.disparity_info.unique_thresh;
      stIm->smoothThresh        =     raw_stereo.disparity_info.smooth_thresh;
      stIm->speckleDiff         =     raw_stereo.disparity_info.speckle_diff;
      stIm->speckleRegionSize   =     raw_stereo.disparity_info.speckle_region_size;
      stIm->unique_check        =     raw_stereo.disparity_info.unique_check;
    }

    memcpy((char*)(stIm->T),  (char*)(&raw_stereo.stereo_info.T[0]),  3*sizeof(double));
    memcpy((char*)(stIm->Om), (char*)(&raw_stereo.stereo_info.Om[0]), 3*sizeof(double));
    memcpy((char*)(stIm->RP), (char*)(&raw_stereo.stereo_info.RP[0]), 16*sizeof(double));

    RawStereoToCamData(raw_stereo.left_image,  raw_stereo.left_info,  raw_stereo.left_type, stIm->imLeft);
    RawStereoToCamData(raw_stereo.right_image, raw_stereo.right_info, raw_stereo.right_type, stIm->imRight);
  }
}

#endif
