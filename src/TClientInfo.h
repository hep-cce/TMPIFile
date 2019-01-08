
//A TMemFile that utilizes MPI Libraries to create and Merge ROOT Files


 // @(#)root/io:$Id$
 // Author: Amit Bashyal, August 2018
 
 /*************************************************************************
  * Copyright (C) 1995-2009, Rene Brun and Fons Rademakers.               *
  * All rights reserved.                                                  *
  *                                                                       *
  * For the licensing terms see $ROOTSYS/LICENSE.                         *
  * For the list of contributors see $ROOTSYS/README/CREDITS.             *
  *************************************************************************/
//Client Information (To handle the worker information)
#ifndef ROOT_TClientInfo
#define ROOT_TClientInfo

#include "TMemFile.h"
#include "TTimeStamp.h"
#include "TFile.h"
class TClientInfo{

 public:
  TFile *fFile;
  TString fLocalName;
  UInt_t fContactsCount;
  TTimeStamp fLastContact;
  Double_t fTimeSincePrevContact;

  TClientInfo();//default constructor
  TClientInfo(const char *filename, UInt_t clientID);//another constructor
  virtual ~TClientInfo();

  void Set(TFile *file);
  void R__MigrateKey(TDirectory *destination, TDirectory *source);
  void R__DeleteObject(TDirectory *dir, Bool_t withReset);

ClassDef(TClientInfo,0);
};
#endif
