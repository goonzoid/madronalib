// madronalib: a C++ framework for DSP applications.
// Copyright (c) 2020 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLValue.h"

#include "MLTextUtils.h"

namespace ml
{
const Matrix Value::nullMatrix{};

Value::Value() : mType(kUndefinedValue), mFloatVal(0) {}

Value::Value(const Value& other) : mType(other.getType()), mFloatVal(0)
{
  switch (mType)
  {
    case kUndefinedValue:
      break;
    case kFloatValue:
      mFloatVal = other.getFloatValue();
      break;
    case kTextValue:
      mTextVal = other.getTextValue();
      break;
    case kBlobValue:
      _sizeInBytes = other._sizeInBytes;
      std::copy(other._data, other._data + _sizeInBytes, _data);
      break;
    case kMatrixValue:
      mMatrixVal = other.getMatrixValue();
      break;
    case kUnsignedLongValue:
      mUnsignedLongVal = other.getUnsignedLongValue();
      break;

  }
}

Value& Value::operator=(const Value& other)
{
  mType = other.getType();
  switch(mType)
  {
    case kUndefinedValue:
      break;
    case kFloatValue:
      mFloatVal = other.getFloatValue();
      break;
    case kTextValue:
      mTextVal = other.getTextValue();
      break;
    case kBlobValue:
      _sizeInBytes = other._sizeInBytes;
      std::copy(other._data, other._data + _sizeInBytes, _data);
      break;
    case kMatrixValue:
      // Matrix handles copy-in-place when possible
      mMatrixVal = other.getMatrixValue();
      break;
    case kUnsignedLongValue:
      mUnsignedLongVal = other.getUnsignedLongValue();
      break;
  }

  return *this;
}

Value::Value(float v) : mType(kFloatValue) { mFloatVal = v; }

Value::Value(int v) : mType(kFloatValue) { mFloatVal = v; }

Value::Value(bool v) : mType(kFloatValue) { mFloatVal = v; }

Value::Value(uint32_t v) : mType(kUnsignedLongValue) { mUnsignedLongVal = v; }

Value::Value(long v) : mType(kFloatValue) { mFloatVal = v; }

Value::Value(double v) : mType(kFloatValue) { mFloatVal = v; }

Value::Value(const ml::Text& t) : mType(kTextValue) { mTextVal = t; }

Value::Value(const char* t) : mType(kTextValue) { mTextVal = ml::Text(t); }

Value::Value(const ml::Matrix& s) : mType(kMatrixValue) { mMatrixVal = s; }

Value::Value(BlobPtr b) : mType(kBlobValue)
{
  _sizeInBytes = b._sizeInBytes;
  
  if(_sizeInBytes <= kBlobSizeBytes)
  {
    std::copy(b._data, b._data + _sizeInBytes, _data);
  }
  else
  {
    // TODO throw?
  }
}

Value::~Value()
{
  // delete any external data?
}

void Value::setValue(const float& v)
{
  mType = kFloatValue;
  mFloatVal = v;
}

void Value::setValue(const int& v)
{
  mType = kFloatValue;
  mFloatVal = v;
}

void Value::setValue(const bool& v)
{
  mType = kFloatValue;
  mFloatVal = v;
}

void Value::setValue(const uint32_t& v)
{
  mType = kUnsignedLongValue;
  mUnsignedLongVal = v;
}

void Value::setValue(const long& v)
{
  mType = kFloatValue;
  mFloatVal = v;
}

void Value::setValue(const double& v)
{
  mType = kFloatValue;
  mFloatVal = v;
}

void Value::setValue(const ml::Text& v)
{
  mType = kTextValue;
  mTextVal = v;
}

void Value::setValue(const char* const v)
{
  mType = kTextValue;
  mTextVal = v;
}

void Value::setValue(const Matrix& v)
{
  mType = kMatrixValue;
  mMatrixVal = v;
}

void Value::setValue(const Value& v) { *this = v; }

bool Value::operator==(const Value& b) const
{
  bool r = false;
  if (mType == b.getType())
  {
    switch (mType)
    {
      case kUndefinedValue:
        r = true;
        break;
      case kFloatValue:
        r = (getFloatValue() == b.getFloatValue());
        break;
      case kTextValue:
        r = (getTextValue() == b.getTextValue());
        break;
      case kMatrixValue:
        r = (getMatrixValue() == b.getMatrixValue());
        break;
      case kUnsignedLongValue:
        r = (getUnsignedLongValue() == b.getUnsignedLongValue());
        break;
      case kBlobValue:
        r = false;
        break;
    }
  }
  return r;
}

bool Value::operator!=(const Value& b) const { return !operator==(b); }

#pragma mark Value utilities

std::ostream& operator<<(std::ostream& out, const Value& r)
{
  switch (r.getType())
  {
    default:
    case Value::kUndefinedValue:
      out << "[undefined]";
      break;
    case Value::kFloatValue:
      out << r.getFloatValue();
      break;
    case Value::kTextValue:
      out << r.getTextValue();
      break;
    case Value::kMatrixValue:
      out << r.getMatrixValue();
      break;
    case Value::kUnsignedLongValue:
      out << r.getUnsignedLongValue();
      break;
    case Value::kBlobValue:
      out << "[blob]";
      break;
  }
  return out;
}

}  // namespace ml
