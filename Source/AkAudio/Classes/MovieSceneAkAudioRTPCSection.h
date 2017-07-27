// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "AkInclude.h"
#include "AkAudioEvent.h"
#if AK_SUPPORTS_LEVEL_SEQUENCER
#include "Curves/RichCurve.h"
#include "MovieSceneSection.h"
#include "IKeyframeSection.h"
#endif // AK_SUPPORTS_LEVEL_SEQUENCER

#include "MovieSceneAkAudioRTPCSection.generated.h"

/**
* A single floating point section
*/
UCLASS(MinimalAPI)
class UMovieSceneAkAudioRTPCSection
	: public UMovieSceneSection
#if AK_SUPPORTS_LEVEL_SEQUENCER
	, public IKeyframeSection<float>
#endif
{
	GENERATED_BODY()

#if AK_SUPPORTS_LEVEL_SEQUENCER
public:
	/**
	* Updates this section
	*
	* @param Position	The position in time within the movie scene
	*/
	AKAUDIO_API virtual float Eval(float Position) const;

	/**
	* @return The float curve on this section
	*/
	FRichCurve& GetFloatCurve() { return FloatCurve; }
	const FRichCurve& GetFloatCurve() const { return FloatCurve; }

public:

	//~ IKeyframeSection interface

	AKAUDIO_API void AddKey(float Time, const float& Value, EMovieSceneKeyInterpolation KeyInterpolation) override;
	AKAUDIO_API bool NewKeyIsNewData(float Time, const float& Value) const override;
	AKAUDIO_API bool HasKeys(const float& Value) const override;
	AKAUDIO_API void SetDefault(const float& Value) override;
	AKAUDIO_API virtual void ClearDefaults() override;

public:

	//~ UMovieSceneSection interface

	AKAUDIO_API virtual void MoveSection(float DeltaPosition, TSet<FKeyHandle>& KeyHandles) override;
	AKAUDIO_API virtual void DilateSection(float DilationFactor, float Origin, TSet<FKeyHandle>& KeyHandles) override;
	AKAUDIO_API virtual void GetKeyHandles(TSet<FKeyHandle>& OutKeyHandles, TRange<float> TimeRange) const override;
	AKAUDIO_API virtual TOptional<float> GetKeyTime(FKeyHandle KeyHandle) const override;
	AKAUDIO_API virtual void SetKeyTime(FKeyHandle KeyHandle, float Time) override;
#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
	virtual FMovieSceneEvalTemplatePtr GenerateTemplate() const override;
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES

	/** @return the name of the RTPC being modified by this track */
	const FString& GetRTPCName() const { return Name; }

	/**
	* Sets the name of the RTPC being modified by this track
	*
	* @param InRTPCName The RTPC being modified
	*/
	void SetRTPCName(const FString& InRTPCName) { Name = InRTPCName; }
#endif // AK_SUPPORTS_LEVEL_SEQUENCER
protected:

	/** Name of the RTPC to modify. */
	UPROPERTY(EditAnywhere, Category = "AkAudioRTPC")
	FString Name;

	/** Curve data */
	UPROPERTY()
	FRichCurve FloatCurve;
};
