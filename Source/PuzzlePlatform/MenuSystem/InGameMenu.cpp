// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "MenuInterface.h"
#include "Components/Button.h"
#include "PuzzlePlatform/PuzzlePlatformGameInstance.h"

    bool UInGameMenu::Initialize()
    {
        bool Success = Super::Initialize();

        if (!ensure(InGameCancelButton!=nullptr))
        {
            return false;
        }
        InGameCancelButton->OnClicked.AddDynamic(this, &UInGameMenu::Cancel);

        if (!ensure(QuitButton!=nullptr))
        {
            return false;
        }
        QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::Quit);



        return true;
    }
    
    void UInGameMenu::Cancel()
    {
        Teardown();
    }
    
    void UInGameMenu::Quit()
    {
        if (MenuInterface != nullptr)
        {
            Teardown();
            MenuInterface->LoadMainMenu();
        }
    }
    
    
    

