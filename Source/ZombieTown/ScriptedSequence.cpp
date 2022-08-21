// Fill out your copyright notice in the Description page of Project Settings.

#include "ScriptedSequence.h"
#include "PrintHelper.h"

void FScriptedSequence::GenerateEvents(TArray<FScriptedEvent>& events)
{
	int count = 0;
	for (const FScriptSpeechElement& element : Script)
	{
		FScriptedEvent created = GenerateEvent(element);
		created.Id = Id + FString::FromInt(count);
		events.Add(created);
		count++;
	}

	for (int k = 0; k < events.Num() - 1; k++)
	{
		events[k].GoToIdNext = events[k + 1].Id;
	}
	if (!events.IsEmpty())
	{
		events.Last().GoToIdNext = "";
	}
}


FScriptedEvent  FScriptedSequence::GenerateEvent(const FScriptSpeechElement& element)
{
	for (const FScriptedEvent& templ : CharacterTemplates)
	{
		if (element.CharacterName.EqualTo(templ.TitleText))
		{
			FScriptedEvent out = templ;
			out.Text = element.Text;
			return out;
		}
	}
	LOGE("Failed to generate event for character with name %s", *(element.CharacterName.ToString()));
	return FScriptedEvent();
}
