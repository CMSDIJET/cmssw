// -*- C++ -*-
//
// Package:     FWCore/SharedMemory
// Class  :     WriteBuffer
//
// Implementation:
//     [Notes on implementation]
//
// Original Author:  Chris Jones
//         Created:  21/01/2020
//

// system include files

// user include files
#include "FWCore/SharedMemory/interface/WriteBuffer.h"
#include "FWCore/Utilities/interface/Exception.h"
//
// constants, enums and typedefs
//
using namespace edm::shared_memory;

//
// static data member definitions
//

//
// constructors and destructor
//

WriteBuffer::~WriteBuffer() {
  if (sm_) {
    sm_->destroy<char>(buffer_names::kBuffer);
    sm_.reset();
    boost::interprocess::shared_memory_object::remove(bufferNames_[bufferInfo_->index_].c_str());
  }
}
//
// member functions
//
void WriteBuffer::growBuffer(std::size_t iLength) {
  int newBuffer = (bufferInfo_->index_ + 1) % 2;
  if (sm_) {
    try {
      sm_->destroy<char>(buffer_names::kBuffer);
    } catch (boost::interprocess::interprocess_exception const& iExcept) {
      throw cms::Exception("SharedMemory")
          << "in growBuffer while destroying the shared memory object the following exception was caught\n"
          << iExcept.what();
    }
    sm_.reset();
    try {
      boost::interprocess::shared_memory_object::remove(bufferNames_[bufferInfo_->index_].c_str());
    } catch (boost::interprocess::interprocess_exception const& iExcept) {
      throw cms::Exception("SharedMemory")
          << "in growBuffer while removing the shared memory object named '" << bufferNames_[bufferInfo_->index_]
          << "' the following exception was caught\n"
          << iExcept.what();
    }
  }
  try {
    sm_ = std::make_unique<boost::interprocess::managed_shared_memory>(
        boost::interprocess::open_or_create, bufferNames_[newBuffer].c_str(), iLength + 1024);
  } catch (boost::interprocess::interprocess_exception const& iExcept) {
    throw cms::Exception("SharedMemory") << "in growBuffer while creating the shared memory object '"
                                         << bufferNames_[newBuffer] << "' of length " << iLength + 1024
                                         << " the following exception was caught\n"
                                         << iExcept.what();
  }
  assert(sm_.get());
  bufferSize_ = iLength;
  bufferInfo_->index_ = newBuffer;
  bufferInfo_->identifier_ = bufferInfo_->identifier_ + 1;
  buffer_ = sm_->construct<char>(buffer_names::kBuffer)[iLength](0);
  assert(buffer_);
}

//
// const member functions
//

//
// static member functions
//
