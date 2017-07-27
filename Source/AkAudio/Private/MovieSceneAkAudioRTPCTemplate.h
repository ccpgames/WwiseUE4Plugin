// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES

#include "MovieSceneAkAudioRTPCTemplate.generated.h"


class UMovieSceneAkAudioRTPCSection;

USTRUCT()
struct FMovieSceneAkAudioRTPCSectionData
{
	GENERATED_BODY()

	FMovieSceneAkAudioRTPCSectionData() {}

#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
	FMovieSceneAkAudioRTPCSectionData(const UMovieSceneAkAudioRTPCSection& Section);
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES

	UPROPERTY()
	FString RTPCName;

	UPROPERTY()
	FRichCurve RTPCCurve;
};


USTRUCT()
struct AKAUDIO_API FMovieSceneAkAudioRTPCTemplate
#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
	: public FMovieSceneEvalTemplate
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
{
	GENERATED_BODY()

	FMovieSceneAkAudioRTPCTemplate() {}

#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
	FMovieSceneAkAudioRTPCTemplate(const UMovieSceneAkAudioRTPCSection& InSection);

	virtual void Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;

	virtual UScriptStruct& GetScriptStructImpl() const override { return *StaticStruct(); }

	virtual void Setup(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const override;
	virtual void SetupOverrides() override { EnableOverrides(RequiresSetupFlag); }
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES

	UPROPERTY()
	const UMovieSceneAkAudioRTPCSection* Section = nullptr;
};
