#ifndef UNARYVALUETOVALUEFILTER_H
#define UNARYVALUETOVALUEFILTER_H

#include "itkProcessObject.h"
#include "itkSimpleDataObjectDecorator.h"
#include "SNAPCommon.h"

/**
 * An ITK pipeline filter that applies transformations to atomic values. The
 * filter can be used to cast a value from one type to another, or to scale
 * the value by a constant, etc. The filter is used with objects of type
 * itk::SimpleDataObjectDecorator. It's main use in SNAP is to manipluate
 * objects generated by filters like the itk::MinimumMaximumImageFilter. For
 * example, we can take the minimum of an image (represented as a short),
 * cast it to double, scale it by some value, and pass it in as an input
 * to another filter.
 */
template <class TFunctor>
class UnaryValueToValueFilter : public itk::ProcessObject
{
public:
  irisITKObjectMacro(UnaryValueToValueFilter<TFunctor>, itk::ProcessObject)

  typedef typename TFunctor::InputType InputType;
  typedef typename TFunctor::OutputType OutputType;

  typedef itk::SimpleDataObjectDecorator<InputType> InputObjectType;
  typedef itk::SimpleDataObjectDecorator<OutputType> OutputObjectType;

  using Superclass::SetInput;

  const InputObjectType *GetInput()
  {
    return static_cast<const InputObjectType *>(this->GetPrimaryInput());
  }

  void SetInput(const InputObjectType *object)
  {
    Superclass::SetPrimaryInput(object);
  }

  OutputObjectType *GetOutput()
  {
    return static_cast<OutputObjectType *>(Superclass::GetPrimaryOutput());
  }

  typename Superclass::DataObjectPointer
  MakeOutput(DataObjectPointerArraySizeType)
  {
    return static_cast<OutputObjectType *>(
          OutputObjectType::New().GetPointer());
  }

  void SetFunctor(const TFunctor &f)
  {
    m_Functor = f;
    this->Modified();
  }

  void GenerateData()
  {
    InputType in = this->GetInput()->Get();
    OutputType out = m_Functor(in);
    this->GetOutput()->Set(out);
  }

protected:
  TFunctor m_Functor;

};


#endif // UNARYVALUETOVALUEFILTER_H
