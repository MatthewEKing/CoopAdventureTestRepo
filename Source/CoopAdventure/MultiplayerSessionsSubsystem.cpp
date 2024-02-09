// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"




void PrintString(const FString& string)
{
    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, string);
}



UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
    //PrintString("Multiplayer Subsystem Constructor");
    CreateServerAfterDestroyed = false;
    DestroyServerName = "";
    ServerNameToFind = "";
    MySessionName = FName("Co-op Adventure Session Name");
}



void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    //PrintString("Multiplayer Subsystem Initialization");

    //Gets the Online Subsystem
    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
        PrintString(SubsystemName);
        
        SessionInterface = OnlineSubsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
        }
    }
}



void UMultiplayerSessionsSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Warning, TEXT("Multiplayer Subsystem Deinitialization"));
}



//function called on UI Blueprint
void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
    PrintString("Created Server");

    if (ServerName.IsEmpty())
    {
        PrintString("Server Name Cannot Be Empty");
        //fires the custom delegate with broadcast function, all functions bound to delegate wll be called
        ServerCreateDel.Broadcast(false);
        return;
    }

    //Searches Steam (Or other subsystem) for this session and se
    FNamedOnlineSession *ExistingSession = SessionInterface->GetNamedSession(MySessionName);
    if (ExistingSession)
    {
        //Destroys the current server with the same FName and creates a new one with the delegate function
        CreateServerAfterDestroyed = true;
        DestroyServerName = ServerName;
        SessionInterface->DestroySession(MySessionName);
        return;
    }

    FOnlineSessionSettings SessionSettings;

    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.NumPublicConnections = 2;
    SessionSettings.bUseLobbiesIfAvailable = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.bAllowJoinViaPresence = true;

    bool isLAN = false;
    if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
    {
        isLAN = true;
    }
    SessionSettings.bIsLANMatch = isLAN;

    SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}


//function called on UI Blueprint
void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
    PrintString("Finding Server");

    if (ServerName.IsEmpty())
    {
        PrintString("ServerName cannot be empty");
        ServerJoinDel.Broadcast(false);
        return;
    }

    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    bool IsLAN = false;
    if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
    {
        IsLAN = true;
    }
    SessionSearch->bIsLanQuery = IsLAN;
    SessionSearch->MaxSearchResults = 9999;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    ServerNameToFind = ServerName;

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}



void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    //built in delegate function that is automatically called when a session is created.
    PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), bWasSuccessful));

    ServerCreateDel.Broadcast(bWasSuccessful);

    if (bWasSuccessful)
    {
        //the ? Listen launches this file as a listen server
        FString Path = "/Game/ThirdPerson/Maps/ThirdPersonMap?Listen";

        if (!GameMapPath.IsEmpty())
        {
            Path = FString::Printf(TEXT("%s?listen"), *GameMapPath);
        }
        GetWorld()->ServerTravel(Path);
    }
}



void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    //built in delegate function that is automatically called when a session is destroyed.
    PrintString(FString::Printf(TEXT("OnDestroySessionComplete: %d, SessionName %s"), bWasSuccessful, *SessionName.ToString()));

    if (CreateServerAfterDestroyed)
    {
        CreateServer(DestroyServerName);
        CreateServerAfterDestroyed = false;
    }
}



void UMultiplayerSessionsSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
    //built in delegate function that is automatically called when a session is found.
    if (!bWasSuccessful) {return;}
    if (ServerNameToFind.IsEmpty()) {return;}

    //Array of the sessions found through FindSessions funcion (Search Results when looking for servers/sessions)
    TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
    FOnlineSessionSearchResult* CorrectResult = 0;

    if (Results.Num() > 0)
    {
        PrintString(FString::Printf(TEXT("%d Sessions Found"), Results.Num()));

        for (FOnlineSessionSearchResult Result : Results)
        {
            if (Result.IsValid())
            {
                //if this result is valid, we get the server name from the existing session and its session settings that were set when the server was created.
                FString ServerName = "No Name"; //Default value
                Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);



                if (ServerName.Equals(ServerNameToFind))
                {
                    CorrectResult = &Result;
                    PrintString(FString::Printf(TEXT("Found Server With Name: %s"), *ServerName));
                    break;
                }
            }
        }
        
        if (CorrectResult)
        {
            SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
        }
        else
        {
            PrintString(FString::Printf(TEXT("Couldnt Find Server: %s"), *ServerNameToFind));
            ServerNameToFind = "";
            ServerJoinDel.Broadcast(false);
        }
    }
    else
    {
        PrintString("Zero Sessions Found");
        ServerJoinDel.Broadcast(false);
        return;
    }
}



void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    //built in delegate function that is automatically called when a session is joined.
    ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);

    //If we successfully joined the server
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        PrintString(FString::Printf(TEXT("Successfully joined session: %s"), *SessionName.ToString()));


        //Obtains the IP Address of the session/server you want to connect to
        FString Address = "";
        bool Success = SessionInterface->GetResolvedConnectString(SessionName, Address);

        if (Success)
        {   
            PrintString(FString::Printf(TEXT("Address: %s"), *Address));

            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
        }
        else
        {
            PrintString("GetResolvedConnectString returned false, IP not found");
        }
    }
    else
    {
        PrintString("OnJoinSessionComplete Failed");
    }
}