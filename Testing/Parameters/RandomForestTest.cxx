/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: TutorialTest.cxx,v $
  Language:  C++
  Date:      $Date: 2006/12/02 04:22:20 $
  Version:   $Revision: 1.1 $
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// Borland compiler is very lazy so we need to instantiate the template
//  by hand 
#if defined(__BORLANDC__)
#include <SNAPBorlandDummyTypes.h>
#endif

#include "ImageIORoutines.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "LevelSetMeshPipeline.h"
#include "SNAPImageData.h"
#include "GuidedNativeImageIO.h"
#include "ImageIODelegates.h"
#include "EdgePreprocessingSettings.h"
#include "SNAPImageData.h"
#include "EdgePreprocessingSettingsRenderer.h"
#include "GlobalUIModel.h"
#include "SnakeWizardModel.h"

#include "ImageIORoutines.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "LevelSetMeshPipeline.h"
#include "SNAPImageData.h"
#include "SlicePreviewFilterWrapper.h"
#include "ColorMapPresetManager.h"

#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#include "RandomForestTest.h"

using namespace std;

int randomForestTest(GlobalUIModel *gui, const char *mainFilePath, const char *labelFilePath, int forestSize, int treeDepth, bool useCoordinates, bool usePatch, int foregroundLabel, float bias, double imageNumber, SnakeParameters snakeParams)
{

	IRISApplication *driver = gui->GetDriver();

	/**
	*Load main image.
	**/
	const char *sImageFileName = mainFilePath;
	IRISWarningList warnings;
	driver->LoadImageA(sImageFileName, MAIN_ROLE, warnings);

	/**
	*Load label image.
	**/
	const char *sLabelFileName = labelFilePath;
	//const char *sLabelFileName = "X:/Repository/source/src/Testing/TestData/MRIcrop-seg.label";
	//const char *sLabelFileName = "C:/Bachelor Thesis Repository/source/src/Testing/TestData/MRIcrop-seg.label";
	driver->GetColorLabelTable()->LoadFromFile(sLabelFileName);


	/**
	*Access snake mode (called ROI_MODE).
	**/
	driver->GetGlobalState()->SetToolbarMode(ROI_MODE);
	bool active = driver->IsSnakeModeActive();


	/**
	*Create reference to current image data.
	**/
	GenericImageData *imageData = driver->GetCurrentImageData();


	/**
	*Set curser in Crosshairs mode.
	*The Snake image wrapper contains a floating
	*point image whose positive voxels correspond to the pixels outside of the
	*segmentation boundary and whose negative valued pixels are inside.
	**/
	ImageWrapperBase *wrapImage = imageData->GetMain();
	ImageWrapperBase::Pointer imgGrey = wrapImage->GetImageBase();
	Vector3ui vCursor = wrapImage->GetSliceIndex();
	ImageCoordinateGeometry geometry = imageData->GetImageGeometry();
	vCursor -= Vector3ui(1, 0, 0);
	imageData->SetCrosshairs(vCursor);

	/**
	*Set current image slice.
	**/
	ImageWrapperBase::DisplaySlicePointer imgSlice = wrapImage->GetDisplaySlice(0);


	/**
	*Settings for the region of interest.
	*
	*First, set the index for the corner of the ROI (x, y, z).
	**/
	itk::IndexValueType index[3];
	index[0] = 227;
	index[1] = 117;
	index[2] = 0;
	itk::Index<3> indexValue;
	indexValue.SetIndex(index);

	/**
	*Then, set the size of the ROI (x, y, z).
	**/
	itk::SizeValueType size[3];
	size[0] = 90;
	size[1] = 120;
	size[2] = 94;
	itk::Size<3> sizeValue;
	sizeValue.SetSize(size);


	/**
	*Apply settings for region of interest.
	**/
	GlobalState::RegionType roi = imageData->GetImageRegion();
	//roi.SetIndex(indexValue);
	//roi.SetSize(sizeValue);

	SNAPSegmentationROISettings roiSettings = driver->GetGlobalState()->GetSegmentationROISettings();
	//roiSettings.SetROI(roi);
	driver->GetGlobalState()->SetSegmentationROISettings(roiSettings);
	driver->GetGlobalState()->SetShowROI(true);


	/**
	*Tell the IRISApplication to initialize the image data object with the ROI settings.
	**/
	driver->InitializeSNAPImageData(roiSettings);
	//driver->GetGlobalState()->SetShowROI(true);


	/**
	*Instruct the IRISApplication to switch to the auto segmentation mode.
	**/
	driver->SetCurrentImageDataToSNAP();


	/**
	*Load main image.
	**/
	SNAPImageData *snapData = driver->GetSNAPImageData();

	/**
	*Initialize speed image.
	**/
	snapData->InitializeSpeed();

	/**
	*Set current preprocessing mode.
	**/
	driver->GetGlobalState()->SetSnakeType(IN_OUT_SNAKE);
	driver->SetSnakeMode(IN_OUT_SNAKE);
	driver->EnterRandomForestPreprocessingMode();
	driver->EnterPreprocessingMode(PREPROCESS_RF);

	/**
	*Load classifier samples/labels that need to be classified.
	**/
	//driver->LoadImage("X:/Repository/results/SamplesRF.nii.gz", LABEL_ROLE, warnings);
	driver->LoadImage("C:/Uni/SamplesRF.nii.gz", LABEL_ROLE, warnings);

	/**
	*Specify Random Forest Classification settings.
	**/
	gui->GetSnakeWizardModel()->SetForestSize(forestSize);
	gui->GetSnakeWizardModel()->SetTreeDepth(treeDepth);
	gui->GetSnakeWizardModel()->SetClassifierUseCoordinates(useCoordinates);
	gui->GetSnakeWizardModel()->SetClassifierUsePatch(usePatch);
	gui->GetSnakeWizardModel()->SetClassifierBias(bias);

	/**
	*Optional settings.
	**/
	//gui->GetSnakeWizardModel()->SetClassifierBias();
	//gui->GetSnakeWizardModel()->SetClassifierPatchRadius();
	//gui->GetSnakeWizardModel()->SetClassifierLabelForeground();


	/**
	*Set foreground label that should be used for calculating speed image.
	**/
	driver->GetGlobalState()->SetDrawingColorLabel(foregroundLabel);

	/**
	*Train classifier using foreground label samples.
	**/
	gui->GetSnakeWizardModel()->TrainClassifier();


	/**
	*Do preprocessing.
	**/
	gui->GetSnakeWizardModel()->ApplyPreprocessing();
	gui->GetSnakeWizardModel()->CompletePreprocessing();


	/**
	*Save speed image.
	**/
	//stringstream stringStream;
	//stringStream << "X:/Repository/build/debug/SNAP_Results/RandomForestResults/speedRandomForest" << imageNumber << ".nii.gz";
	//string s = stringStream.str();
	//const char* pathSaveTo = s.c_str();

	//SpeedImageWrapper *wrapSpeed = snapData->GetSpeed();
	//SaveImageToFile(pathSaveTo, wrapSpeed->GetImage());


	/**
	*Set bubbles for Level Set Evolution.
	**/
	std::vector<Bubble> bubbles(11);
	bubbles[0].center = Vector3i(259, 172, 53); bubbles[0].radius = 3.4;
	bubbles[1].center = Vector3i(268, 181, 50); bubbles[1].radius = 4.4;
	bubbles[2].center = Vector3i(284, 179, 49); bubbles[2].radius = 5.0;
	bubbles[3].center = Vector3i(251, 172, 61); bubbles[3].radius = 5.0;
	bubbles[4].center = Vector3i(256, 186, 58); bubbles[4].radius = 4.0;
	bubbles[5].center = Vector3i(275, 199, 53); bubbles[5].radius = 4.0;
	bubbles[6].center = Vector3i(288, 192, 51); bubbles[6].radius = 4.0;
	bubbles[7].center = Vector3i(290, 192, 63); bubbles[7].radius = 4.0;
	bubbles[8].center = Vector3i(260, 193, 55); bubbles[8].radius = 4.0;
	bubbles[9].center = Vector3i(273, 172, 45); bubbles[9].radius = 4.0;

	/**
	*Set snake parameters.
	**/
	SnakeParameters parameters = snakeParams;


	/**
	*Initialize and run segmentation.
	**/
	snapData->InitializeSegmentation(
		parameters, bubbles, driver->GetGlobalState()->GetDrawingColorLabel());
	snapData->RunSegmentation(100);


	/**
	*Merge segmentation results with the multi-label segmentation image stores in
	*IRISImageData.
	**/
	driver->UpdateIRISWithSnapImageData();
	driver->SetCurrentImageDataToIRIS();

	/**
	*Release resources.
	**/
	driver->ReleaseSNAPImageData();


	/**
	*Save segmentation result as a 3d image.
	**/
	stringstream stringStream;
	//stringStream << "X:/Repository/build/debug/SNAP_Results/RandomForestResults/randomForestResult" << imageNumber << ".nii.gz";
	stringStream << "C:/Uni/SNAP_Results/RandomForestResults/RF0.0/randomForestResult" << imageNumber << ".nii.gz";
	string s = stringStream.str();
	const char* pathSaveTo = s.c_str();

	SaveImageToFile(pathSaveTo, imageData->GetSegmentation()->GetImage());

	driver->UnloadAllOverlays();
	driver->UnloadMainImage();

	return 0;
}

