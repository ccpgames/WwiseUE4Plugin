// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES

#include "MovieSceneAkAudioEventTemplate.generated.h"

class UMovieSceneAkAudioEventSection;

USTRUCT()
struct AKAUDIO_API FMovieSceneAkAudioEventTemplate
#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
	: public FMovieSceneEvalTemplate
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
{
	GENERATED_BODY()

	FMovieSceneAkAudioEventTemplate() {}

#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
	FMovieSceneAkAudioEventTemplate(const UMovieSceneAkAudioEventSection* InSection);

	virtual void Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;

	virtual UScriptStruct& GetScriptStructImpl() const override { return *StaticStruct(); }

	virtual void Setup(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const override;
	virtual void TearDown(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const override;
	virtual void SetupOverrides() override { EnableOverrides(RequiresSetupFlag | RequiresTearDownFlag); }
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES

	UPROPERTY()
	const UMovieSceneAkAudioEventSection* Section = nullptr;
};
