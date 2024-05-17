// Fill out your copyright notice in the Description page of Project Settings.

#include "SlimeBossAIController.h"
#include "EnemySpawnpoint.h"
#include "NiagaraComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SlimeBossAI.h"
#include "PlayerCharacter.h"
#include "Projectile.h"
#include "NiagaraFunctionLibrary.h"


ASlimeBossAIController::ASlimeBossAIController()
{
	OriginalLocation = FVector::ZeroVector;

}

void ASlimeBossAIController::BeginPlay()
{
	Super::BeginPlay();
	SetPlayer();
	SpawnPointArray = Boss->SpawnPointsArray;
	// Hämtar in slam effect niagara componenten.
	TArray<UNiagaraComponent*> NiagaraComponents;
	Boss->GetComponents<UNiagaraComponent>(NiagaraComponents);
	for (UNiagaraComponent* NiagaraComponent : NiagaraComponents)
	{
		if (NiagaraComponent->GetName() == "Slam Effect")
			{
				SlamEffect = NiagaraComponent; 
			}
		if (NiagaraComponent->GetName() == "Shoot Effect")
		{
			ShootEffect = NiagaraComponent; 
		}
	}

	
	if (Boss)
	{
		PawnMesh = Boss->FindComponentByClass<UStaticMeshComponent>();
		SlamMesh = Cast<UStaticMeshComponent>(Boss->FindComponentByTag(UStaticMeshComponent::StaticClass(), TEXT("SlamMesh")));
		SlamMesh->OnComponentBeginOverlap.AddDynamic(this, &ASlimeBossAIController::OnOverlapBegin);
		ProjectileSpawn = Boss->FindComponentByClass<USceneComponent>();
		StartScale = Boss->StartScale;
		EndScale = Boss->EndScale;
		SlamDamage = Boss->SlamDamage;
		if (ProjectileSpawn)
		{
			//UE_LOG(LogTemp, Warning, TEXT("SPAWN FOUND" ));
		}
	}
	//Sätter dens olika behaviors i Beginplay från behaviortree och blackboard
	//Hanna
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"),true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), false);
	}
	SetFocus(Player);
	ShootEffect->OnSystemFinished.AddDynamic(this, &ASlimeBossAIController::OnNiagaraSystemFinished);
}
void ASlimeBossAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LastShotTime += DeltaSeconds;
	LastSlamTime += DeltaSeconds;
	LastSpawnTime += DeltaSeconds;
	
	// JEREMY 
	if(bIsSlamming)
	{
		Alpha += DeltaSeconds * InterpolationSpeed;
		Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
		FVector CurrentScale = FMath::Lerp(StartScale, EndScale, Alpha);
		SlamMesh->SetRelativeScale3D(CurrentScale);
		if (Alpha >= 1.0f)
		{
			Alpha = 0.0f;
		}
	}
	// JEREMY SLUT

	SetFocus(Player);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());

if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		RotateHead(PlayerLocation);
	}
	UpdateBossPhase();
}
void ASlimeBossAIController::Shoot()
{
	//Louis gjorde denna jag optimerade den och gjorde det till en egen metod
	//Hanna
	FVector SpawnLocation = ProjectileSpawn->GetComponentLocation();
	FRotator SpawnRotation = ProjectileSpawn->GetComponentRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Projectile)
	{
		Projectile->SetDamage(ProjectileDamage);
	}
	
}
void ASlimeBossAIController::RotateHead(FVector TargetLocation)
{
	//Får dens huvud att rotera
	//Hanna
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PawnMesh->GetComponentLocation(), TargetLocation);
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f; 
	PawnMesh->SetWorldRotation(LookAtRotation);
}
void ASlimeBossAIController::SetPlayer()
{
	if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
	if (Player == nullptr)
	
	{

	}

	if (Cast<ASlimeBossAI>(GetPawn()))
	{
		Boss = Cast<ASlimeBossAI>(GetPawn());
	}

	if (Boss == nullptr)
	{
	}
}
void ASlimeBossAIController::UpdateBossPhase()
{
	//Uppdatera vilken del av fasen bossen är i beroende på Bossens Health
	//Hämtar health från ASlimeBossAI
	//Hanna
	ASlimeBossAI* SlimeBoss = Cast<ASlimeBossAI>(GetPawn());
	if (SlimeBoss)
	{
		float Health = SlimeBoss->GetHealth();
		float MaxHealth = SlimeBoss->MaxHealth;
		if (Health <= MaxHealth && Health > (MaxHealth - MaxHealth/3))
		{
			//UE_LOG(LogTemp, Warning, TEXT("nu börjar phase one"));
			BossPhaseOne();
		}
		else if (Health <= (MaxHealth - MaxHealth/3) && Health > MaxHealth/3)
		{
			//UE_LOG(LogTemp, Warning, TEXT("nu börjar phase two"));
			if(bActivatePhaseTwo)
			{
				bShouldSpawnEnemies = true;
				bActivatePhaseTwo = false;
			}
			BossPhaseTwo();
		}
		else if (Health <= MaxHealth/3)
		{
			BossPhaseThree();
		}
	}
}
void ASlimeBossAIController::BossPhaseOne()
{
	//Skjuter endast på spelaren i första fasen
	//Hanna
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), true);
	if (LastShotTime >= ShootCooldown)
	{
		if (ShootEffect)
		{
			ShootEffect->Activate();
		}
		//Shoot();
		LastShotTime = 0;
	}
}
void ASlimeBossAIController::BossPhaseTwo()
{
	//Andra bossfasen, den skjuter och spawnar in fiender
	//Hanna
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"),true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), true);
	if(bShouldSpawnEnemies)
	{
		SpawnEnemies();
		bShouldSpawnEnemies = false;
	}
	
	if (LastShotTime >= ShootCooldown)
	{
		if (ShootEffect)
		{
			ShootEffect->Activate();
		}
		LastShotTime = 0;
	}
	
	if (LastSpawnTime >= 14)
	{
		SpawnEnemies();
		LastSpawnTime = 0;
	}
}
void ASlimeBossAIController::BossPhaseThree()
{
	//Tredje och sista bossfasen, den har en slamattack där den spawnar in fiender
	//Hanna
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), true);
	
	if (LastSlamTime >= SlamCooldown)
	{
		LastSlamTime = 0;
		ResetSlamAttack();
		SlamAttack();
		SpawnEnemies();
	}
	//Göra en slam här där den spawnar in mer fiender 
}
void ASlimeBossAIController::SlamAttack()
{
	//Hanna
	//slamattacken
	if(Boss)
	{
		Boss->SetShield(true);
	}
	if(!bIsSlamming)
	{
		bIsSlamming = true;
		OriginalLocation = Boss->GetActorLocation();
		FVector GroundLocation = FVector(OriginalLocation.X, OriginalLocation.Y, 1200.0f);
		Boss->SetActorLocation(GroundLocation);
		if(SlamEffect)
		{
			SlamEffect->Activate();
		}
		GetWorldTimerManager().SetTimer(SlamAttackTimerHandle, this, &ASlimeBossAIController::EndSlamAttack, 2.0f, false);
		}
	
		// JEREMY Show slam mesh. (For debugging)
		//SlamMesh->SetHiddenInGame(false);
		
	}

void ASlimeBossAIController::EndSlamAttack()
{
	if(Boss)
	{
		Boss->SetShield(false);
	}
	bIsSlamming = false;
	bSlamDealDamage = true;
	if (Boss)
	{
		Boss->SetActorLocation(OriginalLocation);
	}
	//UE_LOG(LogTemp, Warning, TEXT("SlamCooldown: %f"), SlamCooldown);
	GetWorldTimerManager().SetTimer(SlamCooldownTimerHandle, this, &ASlimeBossAIController::ResetSlamAttack, SlamCooldown, false);
	
	// JEREMY Reset slam mesh
	SlamMesh->SetRelativeScale3D(StartScale);
	SlamMesh->SetHiddenInGame(true);
	Alpha = 0;
	UE_LOG(LogTemp, Warning, TEXT("Startscale: %s"), *StartScale.ToString());
	
}
void ASlimeBossAIController::ResetSlamAttack()
{
	LastSlamTime = 0;
}

void ASlimeBossAIController::SpawnEnemies()
{

	for (AEnemySpawnpoint* Spawnpoint : SpawnPointArray)
	{
		if (UNiagaraComponent* NiagaraComponent = Cast<UNiagaraComponent>(Spawnpoint->GetComponentByClass(UNiagaraComponent::StaticClass())))
		{
			NiagaraComponent->Activate();
		}
	}
}
void ASlimeBossAIController::OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent)
{
 	Shoot();
}

void ASlimeBossAIController::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter && bSlamDealDamage)
	{
		PlayerCharacter->TakeDamage(SlamDamage, FDamageEvent(), nullptr, this);
		UE_LOG(LogTemp, Warning, TEXT("TRÄFFAD AV SLAM!"));
		bSlamDealDamage = false;
	}
}


