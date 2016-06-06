// Platform.cpp
// Copyright (C) 2010 Audiokinetic Inc
/// \file 
/// Contains definitions for functions declared in Platform.h

#include "stdafx.h"
#include <libdbg.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>

//#include <include_common/scebase_common/sscebase_target.h>
//include_common\scebase_common\scebase_target.h

void IntegrationDemoAssertHook( 
	const char * in_pszExpression,
    const char * in_pszFileName,
    int in_lineNumber
	)
{
	printf( "Assertion triggered in file %s at line %d. Expression: %s\n", in_pszFileName, in_lineNumber, in_pszExpression );	\
	__asm volatile ("int $0x41"); //_SCE_STOP;
	//*((int*)0) = 1;//__asm__ volatile("bkpt 0x0000");
}

