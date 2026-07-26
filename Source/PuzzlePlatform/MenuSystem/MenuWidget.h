// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORM_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetMenuInterface(IMenuInterface *MMenuInterface);								//? UUserWidget->UMenuWidget->UMainMenu, UIngameMenu로 상속되는 구조
	void Setup();
	void Teardown(); 

protected:
	virtual void OnLevelRemovedFromWorld(ULevel * Inlevel, UWorld * InWorld) override;		//레벨에서 벗어날 때 자동 호출됨.

	IMenuInterface *MenuInterface;

};
