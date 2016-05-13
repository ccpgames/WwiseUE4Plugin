//////////////////////////////////////////////////////////////////////
//
// AkPlatformFuncs.h 
//
// Audiokinetic platform-dependent functions definition.
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <AK/Tools/Common/AkAssert.h>
#include <AK/SoundEngine/Common/AkTypes.h>

//#include <sys/atomics.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

namespace AKPLATFORM
{
	// Atomic Operations
    // ------------------------------------------------------------------

	/// Platform Independent Helper
	inline AkInt32 AkInterlockedIncrement( AkInt32 * pValue )
	{
		return __sync_add_and_fetch(pValue,1);
	}

	/// Platform Independent Helper
	inline AkInt32 AkInterlockedDecrement( AkInt32 * pValue )
	{
		return __sync_sub_and_fetch(pValue,1);
	}

	AkForceInline bool AkInterlockedCompareExchange( volatile AkInt32* io_pDest, AkInt32 in_newValue, AkInt32 in_expectedOldVal )
	{
		return __sync_bool_compare_and_swap(io_pDest, in_expectedOldVal, in_newValue);
	}

	AkForceInline bool AkInterlockedCompareExchange( volatile AkInt64* io_pDest, AkInt64 in_newValue, AkInt64 in_expectedOldVal )
	{
		return __sync_bool_compare_and_swap(io_pDest, in_expectedOldVal, in_newValue);
	}

	inline void AkMemoryBarrier()
	{
		__sync_synchronize();
	}

    // Time functions
    // ------------------------------------------------------------------

	/// Platform Independent Helper
    inline void PerformanceCounter( AkInt64 * out_piLastTime )
	{
		*out_piLastTime = clock();
	}

	/// Platform Independent Helper
	inline void PerformanceFrequency( AkInt64 * out_piFreq )
	{
		// TO DO ANDROID ... is there something better
		*out_piFreq = CLOCKS_PER_SEC;
	}

	template<class destType, class srcType>
	inline size_t AkSimpleConvertString( destType* in_pdDest, const srcType* in_pSrc, size_t in_MaxSize, size_t destStrLen(const destType *),  size_t srcStrLen(const srcType *) )
	{ 
		size_t i;
		size_t lenToCopy = srcStrLen(in_pSrc);
		
		lenToCopy = (lenToCopy > in_MaxSize-1) ? in_MaxSize-1 : lenToCopy;
		for(i = 0; i < lenToCopy; i++)
		{
			in_pdDest[i] = (destType) in_pSrc[i];
		}
		in_pdDest[lenToCopy] = (destType)0;
		
		return lenToCopy;
	}

	#define CONVERT_UTF16_TO_CHAR( _astring_, _charstring_ ) \
		_charstring_ = (char*)AkAlloca( (1 + AKPLATFORM::AkUtf16StrLen((const AkUtf16*)_astring_)) * sizeof(char) ); \
		AK_UTF16_TO_CHAR( _charstring_, (const AkUtf16*)_astring_, AKPLATFORM::AkUtf16StrLen((const AkUtf16*)_astring_)+1 ) 

    #define AK_UTF16_TO_CHAR(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, strlen, AKPLATFORM::AkUtf16StrLen )
    #define AK_UTF16_TO_OSCHAR(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, strlen, AKPLATFORM::AkUtf16StrLen )
    #define AK_UTF16_TO_WCHAR(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, wcslen, AKPLATFORM::AkUtf16StrLen )
    #define AK_CHAR_TO_UTF16(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, AKPLATFORM::AkUtf16StrLen, strlen )
    #define AK_OSCHAR_TO_UTF16(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, AKPLATFORM::AkUtf16StrLen, strlen )
    #define AK_WCHAR_TO_UTF16(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, AKPLATFORM::AkUtf16StrLen, wcslen )

	/// Stack allocations.
	#define AkAlloca( _size_ ) __builtin_alloca( _size_ )
}
