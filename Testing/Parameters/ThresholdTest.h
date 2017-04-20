/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: TestBase.h,v $
  Language:  C++
  Date:      $Date: 2009/11/14 16:19:56 $
  Version:   $Revision: 1.2 $
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "GlobalUIModel.h"

#pragma once
int thresholdTest(GlobalUIModel *gui, const char *mainFilePath, int lowerThreshold, int upperThreshold, float smoothness, double imageNumber, SnakeParameters snakeParams);

