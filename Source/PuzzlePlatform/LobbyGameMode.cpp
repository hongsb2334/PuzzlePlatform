// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "TimerManager.h"

#include "PuzzlePlatformGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ++CurrentPlayerCount;

    
    if (CurrentPlayerCount >= 2)
    {
        GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &ALobbyGameMode::StartGame, 10);
    }
    
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    --CurrentPlayerCount;

    if (CurrentPlayerCount == 3)
    {
        UE_LOG(LogTemp, Warning, TEXT("Now 3 Players in the Lobby"));
    }
    
}

void ALobbyGameMode::StartGame()
{
    auto GameInstance = Cast<UPuzzlePlatformGameInstance>(GetGameInstance());
    if (GameInstance == nullptr)
    {
        return;
    }

    GameInstance->StartSession();

    UWorld *World = GetWorld();
        if (!ensure(World !=nullptr))
        {
            return;
        }
        
        bUseSeamlessTravel = true;
        World->ServerTravel("/Game/PuzzlePlatform/Maps/Game?listen");
}
