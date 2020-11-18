#include <stdio.h>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkCurvatureAnisotropicDiffusionImageFilter.h"


int main()
{
    std::string path;
    std::cout << "ADPKDENHANCE" << std::endl;
    std::cout << "Enter the directory that contains volume.mha: ";
    std::cin >> path;

  const int Dim = 3;

  typedef float PixelType;
  typedef itk::Image<PixelType,Dim> ImageType;

  std::string imageFileName = path+"//volume.mha";
  std::string outputImageFileName = path+"//volume_enhanced.mha";

  std::cout<<"Reading image"<<std::endl;
  typedef itk::ImageFileReader<ImageType> ImageFileReaderType;
  ImageFileReaderType::Pointer imageReader = ImageFileReaderType::New();
  imageReader->SetFileName(imageFileName.c_str());
  try
    {
      imageReader->Update();
    }
  catch(itk::ExceptionObject & exp)
    {
      std::cerr << exp << std::endl;
    }

  std::cout << "Enhancing image. " << std::endl;

  double timeStep = 0.04;//0.0625
  unsigned int numberOfIterations = 5;
  double conductanceParameter = 1.0;

  std::cout << "Time step: " << timeStep << std::endl;
  std::cout << "Number of iterations: " << numberOfIterations << std::endl;
  std::cout << "Conductance parameter: " << conductanceParameter << std::endl;

  typedef itk::CurvatureAnisotropicDiffusionImageFilter<ImageType,ImageType> CurvatureAnisotropicDiffusionFilterType;
  CurvatureAnisotropicDiffusionFilterType::Pointer curvatureAnisotropicDiffusionFilter = CurvatureAnisotropicDiffusionFilterType::New();
  curvatureAnisotropicDiffusionFilter->SetInput(imageReader->GetOutput());
  curvatureAnisotropicDiffusionFilter->SetTimeStep(timeStep);
  curvatureAnisotropicDiffusionFilter->SetNumberOfIterations(numberOfIterations);
  curvatureAnisotropicDiffusionFilter->SetConductanceParameter(conductanceParameter);
  try
    {
      curvatureAnisotropicDiffusionFilter->Update();
    }
  catch(itk::ExceptionObject & exp)
    {
      std::cerr << exp << std::endl;
    }


  ImageType::Pointer outputImage = curvatureAnisotropicDiffusionFilter->GetOutput();

  std::cout << "Writing image. " << std::endl;
  typedef itk::ImageFileWriter<ImageType> ImageWriterType;
  ImageWriterType::Pointer imageWriter = ImageWriterType::New();
  imageWriter->SetInput(outputImage);
  imageWriter->SetFileName(outputImageFileName.c_str());
  imageWriter->UseCompressionOn();
  try
    {
      imageWriter->Update();
    }
  catch(itk::ExceptionObject & exp)
    {
      std::cerr << exp << std::endl;
    }

  return EXIT_SUCCESS;
}
