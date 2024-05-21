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
	//Sätter dens ursprunliga location för att den ska kunna veta vilken position den ska återgå till efter slam
	//Hanna
	OriginalLocation = FVector::ZeroVector;
}

void ASlimeBossAIController::BeginPlay()
{
	Super::BeginPlay();
	//Sätter spelarens referems
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

	//Kontrollerar om Boss-referensen är giltig
	if (Boss)
	{
		//Hittar och sätter referenser till olika komponenter och attribut på bossen
		PawnMesh = Boss->FindComponentByClass<USkeletalMeshComponent>();
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
	//Kollar om AIBehavior är giltig 
	//Hanna
	if (AIBehavior != nullptr)
	{
		//Startar AI Behavior tree
		RunBehaviorTree(AIBehavior);
		//Initerar de olika blackboard värdena
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"),true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), false);
	}
	//Fokus på spelaren 
	SetFocus(Player);
	ShootEffect->OnSystemFinished.AddDynamic(this, &ASlimeBossAIController::OnNiagaraSystemFinished);
}
void ASlimeBossAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//Uppdaterar de olika tiderna för slam, skott och spawn
	LastSlamTime += DeltaSeconds;
	LastSpawnTime += DeltaSeconds;
	
	if (!bIsShooting)
	{
		LastShotTime += DeltaSeconds;
	}
	
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

	//Hämtar spelarens location och roterar bossen mot spelare
	//Hanna
	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		RotateHead(PlayerLocation);
	}
	//Uppdaterar Bossensfas
	UpdateBossPhase();
}
//Louis gjorde denna från början, jag optimerade den och gjorde det till en egen metod
//Hanna
void ASlimeBossAIController::Shoot()
{
	//Hämtar spawnposition och rotation för projektilen
	FVector SpawnLocation = ProjectileSpawn->GetComponentLocation();
	FRotator SpawnRotation = ProjectileSpawn->GetComponentRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	
	//Skapar och skjuter en projektil
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Projectile)
	{
		Projectile->SetDamage(ProjectileDamage);
	}
	
}
//Får dens huvud att rotera
//Hanna
void ASlimeBossAIController::RotateHead(FVector TargetLocation)
{
	//Beräknar ut rotationen för att hitta targetlocation
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PawnMesh->GetComponentLocation(), TargetLocation);
	//nollställer Pitch och roll, justera yaw så den kan kolla på spelaren hela tiden
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f;
	//Sätter meshens rotation till den beröknande rotationen
	PawnMesh->SetWorldRotation(LookAtRotation);
}
//Sätter spelarens referns och bossens referens
void ASlimeBossAIController::SetPlayer()
{
	Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	Boss = Cast<ASlimeBossAI>(GetPawn());
}
//Uppdatera vilken del av fasen bossen är i beroende på Bossens Health
//Hämtar health från ASlimeBossAI
//Hanna
void ASlimeBossAIController::UpdateBossPhase()
{
	//Hittar slimebossens health
	ASlimeBossAI* SlimeBoss = Cast<ASlimeBossAI>(GetPawn());
	if (SlimeBoss)
	{
		float Health = SlimeBoss->GetHealth();
		float MaxHealth = SlimeBoss->MaxHealth;
		//Kontrollerar vilken fas den är i baserat på dens hälsa
		if (Health <= MaxHealth && Health > (MaxHealth - MaxHealth/3))
		{
			BossPhaseOne(); //kallar på fas 1
		}
		else if (Health <= (MaxHealth - MaxHealth/3) && Health > MaxHealth/3)
		{
			if(bActivatePhaseTwo)
			{
				bShouldSpawnEnemies = true;
				bActivatePhaseTwo = false;
			}
			BossPhaseTwo(); //kallar på fas 2
		}
		else if (Health <= MaxHealth/3)
		{
			BossPhaseThree(); //kallar på fas 3
		}
	}
}
//Bossens första fas, skjuter endast på spelaren 
void ASlimeBossAIController::BossPhaseOne()
{
	//Hämtar dess blackboard värden och skjuter på spelaren i dess första fas
	//Hanna
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), true);
	if (LastShotTime >= ShootCooldown)
	{
		if (ShootEffect)
		{
			ShootEffect->Activate();
			bIsShooting = true;

		}
		LastShotTime = 0;
	}
}
//Andra bossfasen, den skjuter och spawnar in fiender
//Hanna & Jeremy
void ASlimeBossAIController::BossPhaseTwo()
{
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
			bIsShooting = true;
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
	//Hanna & Jeremy
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
//Slamattacken
//Hanna 
void ASlimeBossAIController::SlamAttack()
{
	//Sätter bossens sköld som bara kan förstöras av slimeskott
	if(Boss)
	{
		Boss->SetShield(true);
	}
	//Kolla om slamattacken redan pågår
	//Hanna
	if(!bIsSlamming)
	{
		//Sätter att den slammar
		bIsSlamming = true;
		//Sparar ner bossens ursprunliga värde
		OriginalLocation = Boss->GetActorLocation();
		//Sätter värdet för vart bossen ska slama någonstans
		FVector GroundLocation = FVector(OriginalLocation.X, OriginalLocation.Y, 1200.0f);
		//Flyttar sedan bossen till marknivå 
		Boss->SetActorLocation(GroundLocation);
		//Sätter igång slameffect(Jeremy)
		if(SlamEffect)
		{
			SlamEffect->Activate();
		}
		//Sätter in en timemanager för att avsluta slamattacken efter en viss tid
		GetWorldTimerManager().SetTimer(SlamAttackTimerHandle, this, &ASlimeBossAIController::EndSlamAttack, 2.0f, false);
		}
	
		// JEREMY Show slam mesh. (For debugging)
		//SlamMesh->SetHiddenInGame(false);
		
	}
//Avslutar slamattacken
//Hanna
void ASlimeBossAIController::EndSlamAttack()
{
	//Sätter skölden
	if(Boss)
	{
		Boss->SetShield(false);
	}
	bIsSlamming = false;
	bSlamDealDamage = true;
	//Flyttar tillbaka bossen till sin ursprungliga position
	if (Boss)
	{
		Boss->SetActorLocation(OriginalLocation);
	}
	//Återställer slamattacken efter en viss tid 
	GetWorldTimerManager().SetTimer(SlamCooldownTimerHandle, this, &ASlimeBossAIController::ResetSlamAttack, SlamCooldown, false);
	
	// JEREMY Reset slam mesh
	SlamMesh->SetRelativeScale3D(StartScale);
	SlamMesh->SetHiddenInGame(true);
	Alpha = 0;
}
//Återställer slamattackens timer
//Hanna
void ASlimeBossAIController::ResetSlamAttack()
{
	//Återställer timern
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
	bIsShooting = false;
 	Shoot();
}

void ASlimeBossAIController::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter && bSlamDealDamage)
	{
		PlayerCharacter->TakeDamage(SlamDamage, FDamageEvent(), nullptr, this);
		bSlamDealDamage = false;
	}
}

bool ASlimeBossAIController::GetIsSlamming()
{
	return bIsSlamming;
}

bool ASlimeBossAIController::GetIsShooting()
{
	return bIsShooting;
}

