//////////////////////////////////////////////////////////////////////
//
// AkFilePackageLowLevelIODeferred.h
//
// Extends the CAkDefaultIOHookDeferred low level I/O hook with File 
// Package handling functionality. 
//
// See AkDefaultIOHookBlocking.h for details on using the deferred 
// low level I/O hook. 
// 
// See AkFilePackageLowLevelIO.h for details on using file packages.
// 
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_PACKAGE_LOW_LEVEL_IO_DEFERRED_H_
#define _AK_FILE_PACKAGE_LOW_LEVEL_IO_DEFERRED_H_

#include "../Common/AkFilePackageLowLevelIO.h"
#include "AkDefaultIOHookDeferred.h"

class CAkFilePackageLowLevelIODeferred
	: public CAkFilePackageLowLevelIO<CAkDefaultIOHookDeferred>
{
public:
	CAkFilePackageLowLevelIODeferred() {}
	virtual ~CAkFilePackageLowLevelIODeferred() {}
};

#endif //_AK_FILE_PACKAGE_LOW_LEVEL_IO_DEFERRED_H_
