// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "MovieSceneTrackEditor.h"
#include "MovieSceneAkAudioEventTrack.h"


/**
 * Tools for AkAudioEvent tracks
 */
class FMovieSceneAkAudioEventTrackEditor
	: public FMovieSceneTrackEditor
{
public:

	/**
	* Constructor
	*
	* @param InSequencer	The sequencer instance to be used by this tool
	*/
	FMovieSceneAkAudioEventTrackEditor(TSharedRef<ISequencer> InSequencer);

	/**
	* Creates an instance of this class.  Called by a sequencer
	*
	* @param OwningSequencer The sequencer instance to be used by this tool
	* @return The new instance of this class
	*/
	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer);

public:

	// ISequencerTrackEditor interface

	virtual void BuildObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, const FGuid& ObjectBinding, const UClass* ObjectClass) override;
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params) override;
	virtual bool HandleAssetAdded(UObject* Asset, const FGuid& TargetObjectGuid) override;

	virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;

	virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;

#if AK_MATINEE_TO_LEVEL_SEQUENCE_MODULE_MODIFICATIONS
	virtual void BuildTrackContextMenu(FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track) override;
#endif

	virtual const FSlateBrush* GetIconBrush() const override;

protected:

	/** Delegate for AnimatablePropertyChanged in HandleAssetAdded for master sounds */
	bool AddNewMasterSound(float KeyTime, UAkAudioEvent* Event);

	/** Delegate for AnimatablePropertyChanged in HandleAssetAdded for attached sounds */
	bool AddNewAttachedSound(float KeyTime, UAkAudioEvent* Event, TArray<TWeakObjectPtr<UObject>> ObjectsToAttachTo);

private:

	/** Audio sub menu */
	TSharedRef<SWidget> BuildAudioSubMenu(UMovieSceneTrack* Track);

	/** Audio asset selected */
	void OnAudioAssetSelected(const FAssetData& AssetData, UMovieSceneTrack* Track);
};
