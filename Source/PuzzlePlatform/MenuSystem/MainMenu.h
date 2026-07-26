// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT(BlueprintType)
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORM_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	

public:
	UMainMenu(const FObjectInitializer &ObjectInitializer);

	void SetServerList(TArray<FServerData> ServerNames);

	void SelectIndex(uint32 Index);

	void UpdateChildren();

protected:
	virtual bool Initialize() override;

	

private:
	UPROPERTY(meta = (BindWidget))
	class UButton *HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton *JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton *CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton *IPJoinButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton *ExitButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher *MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget *JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget *MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget *HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox *ServerHostName;

	UPROPERTY(meta = (BindWidget))
	class UButton *ConfirmHostMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton *CancelHostMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget *ServerList;

	

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void Exit();

	TSubclassOf<class UUserWidget> ServerRowClass;

	TOptional<uint32> SelectedIndex;                 //? TOptional의 좋은 점은 얻는 값이 null이 될 수 있음을 알려줌.
};
