// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "MenuInterface.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextBlock.h"
#include "ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer &ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class !=nullptr))
        return;

    ServerRowClass = ServerRowBPClass.Class;
}



bool UMainMenu::Initialize()                    //? UUserWidget::Initialize() 으로 선언되어 있어서 CreateWidget으로 생성될 때 마지막으로 Initialize를 호출함, 여기서 재정의하면 beginplay처럼 엔트리 포인트로 활용 가능
{
    bool Success = Super::Initialize();



    if (!Success)
    {
        return false;
    }

    if (!ensure(HostButton!=nullptr))
    {
        return false;
    }
    HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

    if (!ensure(CancelHostMenuButton!=nullptr))
    {
        return false;
    }
    CancelHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

    if (!ensure(ConfirmHostMenuButton!=nullptr))
    {
        return false;
    }
    ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

    if (!ensure(JoinButton!=nullptr))
    {
        return false;
    }
    JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

    if (!ensure(CancelButton!=nullptr))
    {
        return false;
    }
    CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

    if (!ensure(IPJoinButton!=nullptr))
    {
        return false;
    }
    IPJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

    if (!ensure(ExitButton!=nullptr))
    {
        return false;
    }
    ExitButton->OnClicked.AddDynamic(this, &UMainMenu::Exit);

    

    return true;
}


void UMainMenu::HostServer()
{
    if (MenuInterface != nullptr)
    {
        FString ServerName = ServerHostName->Text.ToString();
        MenuInterface->Host(ServerName);
    }
    
}

void UMainMenu::OpenHostMenu()
{
    MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
    UWorld* World = this->GetWorld();
    if (!ensure(World != nullptr))
    {
        return;
    }

    ServerList->ClearChildren();

    uint32 i = 0;
    for(const FServerData& ServerData : ServerNames)
    {
        UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);

        if (!ensure(Row != nullptr))
        {
            return;
        }

        Row->ServerName->SetText(FText::FromString(ServerData.Name));
        Row->HostUser->SetText(FText::FromString(ServerData.HostUserName));
        FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
        Row->ConnectionFraction->SetText(FText::FromString(FractionText));
        Row->Setup(this, i);
        ++i;

        ServerList->AddChild(Row);
    }
        
    
}

void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
    UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
    if (SelectedIndex.IsSet())
    {
        uint32 Index = SelectedIndex.GetValue(); 

        for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
        {
            auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));
            if (Row != nullptr)
            {
                Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);   //? 단락 연산자
            }
            
        }
        
    }
    
    
    
}

void UMainMenu::OpenJoinMenu()
{
    if (!ensure(MenuSwitcher != nullptr))
        return;

    if (!ensure(JoinMenu != nullptr))
        return;

    MenuSwitcher->SetActiveWidget(JoinMenu);

    if (MenuInterface != nullptr)
    {
        MenuInterface->RefreshServer();
    }
    

}

void UMainMenu::OpenMainMenu()
{
    if (!ensure(MenuSwitcher != nullptr))
        return;

    if (!ensure(MainMenu != nullptr))
        return;

    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::JoinServer()
{
    if (SelectedIndex.IsSet() && MenuInterface != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected Index setting: %d"), SelectedIndex.GetValue());         //? #61 서버 선택하기/  포맷 문자열 오타로 에디터 크래시 발생, 해결 완료
        MenuInterface->Join(SelectedIndex.GetValue());      //? SelectedIndex가 TOptional이므로 SelectedIndex.GetValue()값이 null일 수 있음, 따라서 SelectedIndex.IsSet()로 null이 아닐 때만 실행할 수 있게 설정
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected Index not setting"));
    }

    
}

void UMainMenu::Exit()
{
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

    PlayerController->ConsoleCommand("Quit");
}







