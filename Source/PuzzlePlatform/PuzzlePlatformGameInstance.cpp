// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = TEXT("GameSession");                  //! Unreal 4.19 이후 버전에서 기본 세션 이름이 'Game'에서 'GameSession'으로 변경되었음.
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UPuzzlePlatformGameInstance::UPuzzlePlatformGameInstance(const FObjectInitializer &ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class !=nullptr))
        return;

    MenuClass = MenuBPClass.Class;

    ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class !=nullptr))
        return;

    InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformGameInstance::Init()
{
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (Subsystem != nullptr)   //! 기본적으로 언리얼에서 NULL 서브시스템을 제공하기 때문에 Subsystem은 nullptr이 아닌 NULL 서스시스템을 가지게 될 수 있음, 따라서 여기서 if문은 항상 실행 될 수 있음
    {
        UE_LOG(LogTemp, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())         //? SessionInterface는 Tsharedptr이기 때문에 nullptr이 아니라 .IsValid()로 체크
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnCreateSessionComplete);     //? 세션 생성 끝나면 자동 호출 함수 바인딩
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnDestroySessionComplete);   //? 세션 삭제 완료 시 자동 호출 함수 바인딩
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnFindSessionsComplete);       //? 세션 찾기 완료 시 
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformGameInstance::OnJoinSessionComplete); 
        }
        
    }
    else{
        UE_LOG(LogTemp, Warning, TEXT("Found No Subsystem"));
    }

    if (GEngine != nullptr)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformGameInstance::OnNetworkFailure);
    }
    
}



void UPuzzlePlatformGameInstance::LoadMenu()
{
    if (!ensure(MenuClass!=nullptr))
    {
        return;
    }

    Menu = CreateWidget<UMainMenu>(this, MenuClass);

    if (!ensure(Menu!=nullptr))
    {
        return;
    }

    Menu->Setup();

    Menu->SetMenuInterface(this);
}

void UPuzzlePlatformGameInstance::LoadInGameMenu()
{
    if (!ensure(InGameMenuClass != nullptr))
    {
        return;
    }

    UMenuWidget* InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);

    if (!ensure(InGameMenu != nullptr))
    {
        return;
    }

    InGameMenu->Setup();

    InGameMenu->SetMenuInterface(this);
}


void UPuzzlePlatformGameInstance::Host(FString ServerName)
{
    
    DesiredServerName = ServerName;
    if (SessionInterface.IsValid())
    {
        auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }
        else
        {
            CreateSession();
        }
    }
    
    
}

void UPuzzlePlatformGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{

    if (!Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
        return;
    }
    
    if (Menu != nullptr)
    {
        Menu->Teardown();
    }
    
    

    UEngine *Engine = GEngine;
    if (!ensure(Engine !=nullptr))
    {
        return;
    }

    Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

    UWorld *World = GetWorld();
    if (!ensure(World !=nullptr))
    {
        return;
    }

    World->ServerTravel("/Game/PuzzlePlatform/Maps/Lobby?listen");
}

void UPuzzlePlatformGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success)
    {
        CreateSession();
    }
    
}

void UPuzzlePlatformGameInstance::OnFindSessionsComplete(bool Success)
{
    if (Success && SessionSearch.IsValid() && Menu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Find Session Complete"));

        TArray<FServerData> ServerNames;
        for (const FOnlineSessionSearchResult &SearchResult : SessionSearch->SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("Find Session: %s"), *SearchResult.GetSessionIdStr());
            FServerData Data;
            
            Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;            //? 사용 가능한 연결 수
            Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;  //? 사용 가능한 연결 수 - 현재 접속중인 연결 수
            Data.HostUserName = SearchResult.Session.OwningUserName;
            FString ServerName;
            if(SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
            {
                Data.Name = ServerName;
            }
            else
            {
                Data.Name = "Could not find name.";
            }
            ServerNames.Add(Data);
        }

        Menu->SetServerList(ServerNames);
    }
    
    

        
}

void UPuzzlePlatformGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid())
    {
        return;
    }

    FString Address;
    if(!SessionInterface->GetResolvedConnectString(SessionName, Address))       //? call이 성공적이면 true, 아니면 false
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
        return;
    }

    UEngine *Engine = GEngine;
    if (!ensure(Engine !=nullptr))
    {
        return;
    }
    Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

    APlayerController *PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController !=nullptr))
    {
        return;
    }
    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    LoadMainMenu();     //? 호스트가 게임을 종료했을 때, 클라이언트를 메인메뉴로 이동
}

void UPuzzlePlatformGameInstance::CreateSession()
{
    if (SessionInterface.IsValid()) 
    {
        FOnlineSessionSettings SessionSettings;
        if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
        {
            SessionSettings.bIsLANMatch = true;
        }
        else
        {
            SessionSettings.bIsLANMatch = false; 
        }

        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;      //! Subsystem을 스팀으로 설정할 시 Host가 안되는 문제/ 이 코드 추가로 Host는 해결, 추후 다른 스팀 id를 이용해 Join 실험 해야함.
        SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
    }
    
    
}

void UPuzzlePlatformGameInstance::Join(uint32 Index)
{
    if (!SessionInterface.IsValid())
    {
        return;
    }
    if (!SessionSearch.IsValid())
    {
        return;
    }

    if (Menu != nullptr)
    {
        Menu->Teardown();
    }

    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

    
}

void UPuzzlePlatformGameInstance::StartSession()
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME);       //? 세션을 시작한 경우 서버 목록에 뜨지 않게 하는 방법
    }
    
}

void UPuzzlePlatformGameInstance::LoadMainMenu()
{
    APlayerController *PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController !=nullptr))
    {
        return;
    }
    PlayerController->ClientTravel("/Game/MenuSystem/Menu", ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformGameInstance::RefreshServer()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (SessionSearch.IsValid())
    {
        // SessionSearch->bIsLanQuery = true;        //?이 코드 제거하면 기본값인 false가 되어 LAN이 아니라 온라인 서버를 찾겠다는 뜻
        SessionSearch->MaxSearchResults = 100;      //? 현재 Steam의 AppId 480번은 모든 사람들이 공유하고 있으므로 로비를 검색하면 다른사람의 로비도 같이 뜨게 됨, 따라서 나의 로비를 찾지 못할 수 있으므로 높은 숫자인 100으로 설정해서 나의 로비를 찾을 수 있게 설정
        SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);    //강의에서는 프레젠스만 있었는데 로비도 해줘야됨
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
        UE_LOG(LogTemp, Warning, TEXT("Session Finding..."));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}
