// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#pragma once

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
	virtual float Eval(float Position) const;

	/**
	* @return The float curve on this section
	*/
	FRichCurve& GetFloatCurve() { return FloatCurve; }
	const FRichCurve& GetFloatCurve() const { return FloatCurve; }

public:

	//~ IKeyframeSection interface

	void AddKey(float Time, const float& Value, EMovieSceneKeyInterpolation KeyInterpolation) override;
	bool NewKeyIsNewData(float Time, const float& Value) const override;
	bool HasKeys(const float& Value) const override;
	void SetDefault(const float& Value) override;
	virtual void ClearDefaults() override;

public:

	//~ UMovieSceneSection interface

	virtual void MoveSection(float DeltaPosition, TSet<FKeyHandle>& KeyHandles) override;
	virtual void DilateSection(float DilationFactor, float Origin, TSet<FKeyHandle>& KeyHandles) override;
	virtual void GetKeyHandles(TSet<FKeyHandle>& OutKeyHandles, TRange<float> TimeRange) const override;
	virtual TOptional<float> GetKeyTime(FKeyHandle KeyHandle) const override;
	virtual void SetKeyTime(FKeyHandle KeyHandle, float Time) override;

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
