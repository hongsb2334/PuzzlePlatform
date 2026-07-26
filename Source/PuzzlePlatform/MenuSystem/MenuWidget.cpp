// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"


void UMenuWidget::OnLevelRemovedFromWorld(ULevel * Inlevel, UWorld * InWorld)
{
    Super::OnLevelRemovedFromWorld(Inlevel, InWorld);

    UWorld *World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController *PlayerController = World->GetFirstPlayerController();
    if (!PlayerController)
    {
        return;
    }    

    FInputModeGameOnly InputModeData;
    PlayerController->SetInputMode(InputModeData);
    PlayerController->bShowMouseCursor = false;
}

void UMenuWidget::Setup()
{
    this->AddToViewport();

    UWorld *World = GetWorld();
    if (!ensure(World != nullptr))
    {
        return;
    }

    APlayerController *PlayerController = World->GetFirstPlayerController();

    if (!ensure(PlayerController !=nullptr))
    {
        return;
    }

    FInputModeUIOnly InputData;
    this->bIsFocusable = true; //?   UE4.27에서는 Inputdata.SetWidgetToFocus(this->TakeWidget());이 아닌 이 코드를 사용
    InputData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    
    PlayerController->SetInputMode(InputData);
    PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::Teardown()
{
    this->RemoveFromViewport();

    UWorld *World = GetWorld();
    if (!ensure(World != nullptr))
    {
        return;
    }

    APlayerController *PlayerController = World->GetFirstPlayerController();

    if (!ensure(PlayerController !=nullptr))
    {
        return;
    }

    FInputModeGameOnly InputData;               //? GameOnly는 게임 조작만, UIOnly는 UI 조작만, GameAndUI는 둘다 가능
    PlayerController->SetInputMode(InputData);
    PlayerController->bShowMouseCursor = false;
}

void UMenuWidget::SetMenuInterface(IMenuInterface *MMenuInterface)
{
    MenuInterface = MMenuInterface;
}