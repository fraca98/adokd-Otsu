#include <stdio.h>

#include "itkImageFileReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkMaximumImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkChangeInformationImageFilter.h"
#include "itkImageFileWriter.h"


int main()
{
    std::string path;
    std::cout << "ADPKD_IMAGE_MASK" << std::endl;;
    std::cout << "Files must be: DICOM (folder), L.tif, R.tif, volume.mha, mask.mha" << std::endl;;
    std::cout << "Enter the directory that contains DICOM folder, .tif and .mha files: ";
    std::cin >> path;

  const int Dim = 3;

  typedef float PixelType;
  typedef itk::Image<PixelType,Dim> ImageType;

  typedef unsigned char MaskPixelType;
  typedef itk::Image<MaskPixelType,Dim> MaskImageType;

  std::string dicomDirectoryName = path+"//DICOM";
  std::string maskFileName1 = path + "//L.tif";
  std::string maskFileName2 = path + "//R.tif";
  std::string outputImageFileName1 = path + "//volume.mha";
  std::string outputImageFileName2 = path + "//mask.mha";

 // LEGGO LA SERIE DI IMMAGINI DICOM E CREO UNO STACK 3D

  std::cout<<"Reading image series"<<std::endl;
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();

  typedef itk::GDCMImageIO ImageIOType;
  ImageIOType::Pointer dicomIO = ImageIOType::New();

  reader->SetImageIO(dicomIO);
  
  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

  nameGenerator->SetUseSeriesDetails(true);

  nameGenerator->SetDirectory(dicomDirectoryName);

  typedef std::vector<std::string> SeriesIdContainer;

  const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

  SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
  SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
  while(seriesItr != seriesEnd)
    {
    std::cout << seriesItr->c_str() << std::endl;
    seriesItr++;
    }

  std::string seriesIdentifier;

  seriesIdentifier = seriesUID.begin()->c_str();

  typedef std::vector< std::string > FileNamesContainer;
  FileNamesContainer fileNames;

  fileNames = nameGenerator->GetFileNames( seriesIdentifier );

  reader->SetFileNames( fileNames );

  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex  << std::endl;
    return EXIT_FAILURE;
    }

// SCRIVO L'IMMAGINE 3D OTTENUTA (COMPRESSA) 

  typedef itk::ImageFileWriter< ImageType > WriterType; 
  WriterType::Pointer writer1 = WriterType::New();
  writer1->SetInput( reader->GetOutput() );
  writer1->SetFileName( outputImageFileName1.c_str() );  
  writer1->UseCompressionOn();
  writer1->Update();

// LEGGO LE DUE MASCHERE E NE COSTRUISCO UNA COMPLESSIVA 

  std::cout<<"Reading mask1"<<std::endl;
  typedef itk::ImageFileReader<MaskImageType> MaskImageFileReaderType;
  MaskImageFileReaderType::Pointer maskReader1 = MaskImageFileReaderType::New();
  maskReader1->SetFileName(maskFileName1.c_str());

  std::cout<<"Reading mask2"<<std::endl;
  typedef itk::ImageFileReader<MaskImageType> MaskImageFileReaderType;
  MaskImageFileReaderType::Pointer maskReader2 = MaskImageFileReaderType::New();
  maskReader2->SetFileName(maskFileName2.c_str());

  typedef itk::MaximumImageFilter< MaskImageType, MaskImageType, MaskImageType > MaximumFilterType;
  MaximumFilterType::Pointer maximumFilter = MaximumFilterType::New();
  maximumFilter->SetInput1(maskReader1->GetOutput() );
  maximumFilter->SetInput2(maskReader2->GetOutput() );

  try
    {
      maximumFilter->Update();
    }
  catch(itk::ExceptionObject & exp)
    {
      std::cerr << exp << std::endl;
    }

// FLIPPO LA MASCHERA NELLA DIREZIONE Z

  typedef itk::FlipImageFilter<MaskImageType> FlipImageFilterType;
  FlipImageFilterType::Pointer flipfilter = FlipImageFilterType::New();
  typedef FlipImageFilterType::FlipAxesArrayType FlipAxesArrayType;
  FlipAxesArrayType flipArray;
  flipArray[0] = 0;
  flipArray[1] = 0;
  flipArray[2] = 1;
  flipfilter->SetFlipAxes(flipArray);
  flipfilter->SetInput(maximumFilter->GetOutput() );

// APPLICO CASTING

typedef itk::CastImageFilter<ImageType,MaskImageType> CastingFilterType;
CastingFilterType::Pointer caster = CastingFilterType::New();
caster->SetInput(reader->GetOutput());

// CAMBIO LE INFORMAZIONI DELL'IMMAGINE DANDOGLI LO SPACING GIUSTO

typedef itk::ChangeInformationImageFilter<MaskImageType> ChangeFilterType;
ChangeFilterType::Pointer changer = ChangeFilterType::New();
changer->UseReferenceImageOn();
changer->SetReferenceImage(caster->GetOutput());
changer->SetInput(flipfilter->GetOutput());
changer->ChangeSpacingOn();
changer->ChangeOriginOn();
changer->ChangeDirectionOn();

// SCRIVO LA MASCHERA FINALE (COMPRESSA) 


  typedef itk::ImageFileWriter< MaskImageType > MaskWriterType; 
  MaskWriterType::Pointer writer2 = MaskWriterType::New();
  writer2->SetInput(changer->GetOutput() );
  writer2->SetFileName( outputImageFileName2.c_str() );  
  writer2->UseCompressionOn();
  writer2->Update();

}

