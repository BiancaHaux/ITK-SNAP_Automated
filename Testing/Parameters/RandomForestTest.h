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
int randomForestTest(GlobalUIModel *gui, const char *mainFilePath, const char *labelFilePath, int forestSize, int treeDepth, bool useCoordinates, bool usePatch, int foregroundLabel, float bias, double imageNumber, SnakeParameters snakeParams);

