// Copyright 2020 ShatterPoint Games. All Rights Reserved.


#include "BTCharacterACH.h"

void ABTCharacterACH::HitDetection()
{
	ABTCharacterBase::HitDetection();
}

void ABTCharacterACH::UpdateCharacter(int32 CurrentInputs)
{
	ABTCharacterBase::UpdateCharacter(CurrentInputs);
}

void ABTCharacterACH::UpdatePosition()
{
	ABTCharacterBase::UpdatePosition();
}

void ABTCharacterACH::DrawCharacter()
{
	ABTCharacterBase::DrawCharacter();
}

bool ABTCharacterACH::NonKnockdownLanding()
{
	return ABTCharacterBase::NonKnockdownLanding();
}

bool ABTCharacterACH::ActiveTransitions()
{
	if (BlitzCancel())
		return true;
	//Break Triggers/Supers
	//Special Attacks
	//Normal Attacks

	return ABTCharacterBase::ActiveTransitions();
}

bool ABTCharacterACH::ConditionalTransitions()
{
	return ABTCharacterBase::ConditionalTransitions();
}

bool ABTCharacterACH::PassiveTransitions()
{
	return ABTCharacterBase::PassiveTransitions();
}

bool ABTCharacterACH::ExitTimeTransitions()
{
	return ABTCharacterBase::ExitTimeTransitions();
}

void ABTCharacterACH::AnimationEvents()
{
	ABTCharacterBase::AnimationEvents();
}

void ABTCharacterACH::CreateMaterials()
{
	ABTCharacterBase::CreateMaterials();

	if (Main != nullptr)
		DynamicBodyMain = UMaterialInstanceDynamic::Create(Main, this);

	if (Metallic != nullptr)
	{
		DynamicBodyMetallic = UMaterialInstanceDynamic::Create(Metallic, this);
		DynamicSpearMetallic = UMaterialInstanceDynamic::Create(Metallic, this);
	}

	if (Specular != nullptr)
	{
		DynamicBodySpec = UMaterialInstanceDynamic::Create(Specular, this);
		DynamicSpearSpec = UMaterialInstanceDynamic::Create(Specular, this);
	}

	if (Seals != nullptr)
		DynamicSeals = UMaterialInstanceDynamic::Create(Seals, this);

	if (RightEye != nullptr)
		DynamicRightEye = UMaterialInstanceDynamic::Create(RightEye, this);

	if (SpearEdge != nullptr)
		DynamicSpearEdge = UMaterialInstanceDynamic::Create(SpearEdge, this);

	//set texture parameters and set materials
	if (DynamicOutline != nullptr)
		BaseMesh->SetMaterial(3, DynamicOutline);

	if (DynamicBodyMain != nullptr)
	{
		BaseMesh->SetMaterial(0, DynamicBodyMain);

		if (BodyBC != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodyILM != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodySpec != nullptr)
	{
		BaseMesh->SetMaterial(1, DynamicBodySpec);

		if (BodyBC != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodyILM != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodyMetallic != nullptr)
	{
		BaseMesh->SetMaterial(2, DynamicBodyMetallic);

		if (BodyBC != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodyILM != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSeals != nullptr)
	{
		BaseMesh->SetMaterial(4, DynamicSeals);

		if (SealsBC != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("BaseColor"), SealsBC);
		if (BodyILM != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicRightEye != nullptr)
	{
		BaseMesh->SetMaterial(5, DynamicRightEye);

		if (BodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (AltBodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("WCBaseColor"), AltBodyBC);
		if (BodyILM != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("ILM"), BodyILM);
		if (BodyLines != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("Lines"), BodyLines);
		if (BodySSS != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSpearSpec != nullptr)
	{
		BaseMesh->SetMaterial(8, DynamicSpearSpec);

		if (SpearBC != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearMetallic != nullptr)
	{
		BaseMesh->SetMaterial(9, DynamicSpearMetallic);

		if (SpearBC != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearEdge != nullptr)
	{
		BaseMesh->SetMaterial(10, DynamicSpearEdge);

		if (SpearBC != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

}

void ABTCharacterACH::SetColor(uint8 ColorID)
{
	ABTCharacterBase::SetColor(ColorID);

	struct FACHFilePaths
	{
		TCHAR* BodyBaseColor;
		TCHAR* BodyShading;
		TCHAR* SpearBaseColor;
		TCHAR* SpearShading;
		TCHAR* SealBaseColor;
		TCHAR* AltBodyBaseColor;
	};

	FACHFilePaths TexPaths;

	switch (ColorID) //set parameter colors for character, blitzimage, and sigil based on colorID
	{
	case 1:
		TexPaths.BodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_BC.T_ACH_01_BC'");
		TexPaths.BodyShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_SSS.T_ACH_01_SSS'");
		TexPaths.SpearBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_BC.T_ACH_Spear_01_BC'");
		TexPaths.SpearShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_SSS.T_ACH_Spear_01_SSS'");
		TexPaths.SealBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Seal_01_BC.T_ACH_Seal_01_BC'");
		TexPaths.AltBodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_WC_01_BC.T_ACH_WC_01_BC'");
		Sigils[0]->SigilColor = FVector(1, .25f, 1); //Change sigil colors based on color id
		Sigils[1]->SigilColor = Sigils[0]->SigilColor;
		Sigils[0]->EchoColor = FVector(1, 0, 1);
		Sigils[1]->EchoColor = Sigils[0]->EchoColor;
		//BlitzImage->BlitzColor = FVector(.9f, .2f, .75f);
		break;

	default:
		TexPaths.BodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_BC.T_ACH_01_BC'");
		TexPaths.BodyShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_01_SSS.T_ACH_01_SSS'");
		TexPaths.SpearBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_BC.T_ACH_Spear_01_BC'");
		TexPaths.SpearShading = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Spear_01_SSS.T_ACH_Spear_01_SSS'");
		TexPaths.SealBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_Seal_01_BC.T_ACH_Seal_01_BC'");
		TexPaths.AltBodyBaseColor = TEXT("Texture'/Game/CharacterAssets/ACH/Textures/Color01/T_ACH_WC_01_BC.T_ACH_WC_01_BC'");
		Sigils[0]->SigilColor = FVector(1, .25f, 1); //Change sigil colors based on color id
		Sigils[1]->SigilColor = Sigils[0]->SigilColor;
		Sigils[0]->EchoColor = FVector(1, 0, 1);
		Sigils[1]->EchoColor = Sigils[0]->EchoColor;
		break;
	}

	BodyBC = LoadObject<UTexture>(NULL, TexPaths.BodyBaseColor);
	BodySSS = LoadObject<UTexture>(NULL, TexPaths.BodyShading);
	SpearBC = LoadObject<UTexture>(NULL, TexPaths.SpearBaseColor);
	SpearSSS = LoadObject<UTexture>(NULL, TexPaths.SpearShading);
	SealsBC = LoadObject<UTexture>(NULL, TexPaths.SealBaseColor);
	AltBodyBC = LoadObject<UTexture>(NULL, TexPaths.AltBodyBaseColor);

	//set texture parameters

	if (DynamicBodyMain != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodySSS != nullptr)
			DynamicBodyMain->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodySpec != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodySSS != nullptr)
			DynamicBodySpec->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicBodyMetallic != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (BodySSS != nullptr)
			DynamicBodyMetallic->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSeals != nullptr)
	{
		if (SealsBC != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("BaseColor"), SealsBC);
		if (BodySSS != nullptr)
			DynamicSeals->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicRightEye != nullptr)
	{
		if (BodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("BaseColor"), BodyBC);
		if (AltBodyBC != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("WCBaseColor"), AltBodyBC);
		if (BodySSS != nullptr)
			DynamicRightEye->SetTextureParameterValue(FName("SSS"), BodySSS);
	}

	if (DynamicSpearSpec != nullptr)
	{
		if (SpearBC != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearSSS != nullptr)
			DynamicSpearSpec->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearMetallic != nullptr)
	{
		if (SpearBC != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearSSS != nullptr)
			DynamicSpearMetallic->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (DynamicSpearEdge != nullptr)
	{
		if (SpearBC != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("BaseColor"), SpearBC);
		if (SpearILM != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("ILM"), SpearILM);
		if (SpearLines != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("Lines"), SpearLines);
		if (SpearSSS != nullptr)
			DynamicSpearEdge->SetTextureParameterValue(FName("SSS"), SpearSSS);
	}

	if (BlitzImage != nullptr)
	{
		ABlitzImageACH* ACHBlitz = Cast<ABlitzImageACH>(BlitzImage);

		if (ACHBlitz != nullptr)
		{
			if (ACHBlitz->DynamicBodyMaterial)
			{
				if (BodyBC != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("BaseColor"), BodyBC);
				if (BodyILM != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("ILM"), BodyILM);
				if (BodyLines != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("Lines"), BodyLines);
				if (BodySSS != nullptr)
					ACHBlitz->DynamicBodyMaterial->SetTextureParameterValue(FName("SSS"), BodySSS);
			}

			if (ACHBlitz->DynamicSpearMaterial)
			{
				if (SpearBC != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("BaseColor"), SpearBC);
				if (SpearILM != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("ILM"), SpearILM);
				if (SpearLines != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("Lines"), SpearLines);
				if (SpearSSS != nullptr)
					ACHBlitz->DynamicSpearMaterial->SetTextureParameterValue(FName("SSS"), SpearSSS);
			}

			if (ACHBlitz->DynamicSealMaterial)
			{
				if (SealsBC != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("BaseColor"), SealsBC);
				if (BodyILM != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("ILM"), BodyILM);
				if (BodyLines != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("Lines"), BodyLines);
				if (BodySSS != nullptr)
					ACHBlitz->DynamicSealMaterial->SetTextureParameterValue(FName("SSS"), BodySSS);
			}
		}
	}
}

void ABTCharacterACH::LightSettings()
{
	ABTCharacterBase::LightSettings();

	FVector MainLightValue = MainLight->GetComponentLocation() - GetActorLocation();

	if (DynamicBodyMain)
		DynamicBodyMain->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
	if (DynamicBodyMetallic)
		DynamicBodyMetallic->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
	if (DynamicBodySpec)
		DynamicBodySpec->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
	if (DynamicSeals)
		DynamicSeals->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
	if (DynamicRightEye)
		DynamicRightEye->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
	if (DynamicSpearMetallic)
		DynamicSpearMetallic->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
	if (DynamicSpearSpec)
		DynamicSpearSpec->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);
	if (DynamicSpearEdge)
		DynamicSpearEdge->SetVectorParameterValue(FName("MainLightVector"), MainLightValue);

	FVector FillLightValue = FillLight->GetComponentLocation() - GetActorLocation();

	if (DynamicBodyMain)
		DynamicBodyMain->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
	if (DynamicBodyMetallic)
		DynamicBodyMetallic->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
	if (DynamicBodySpec)
		DynamicBodySpec->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
	if (DynamicSeals)
		DynamicSeals->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
	if (DynamicRightEye)
		DynamicRightEye->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
	if (DynamicSpearMetallic)
		DynamicSpearMetallic->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
	if (DynamicSpearSpec)
		DynamicSpearSpec->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
	if (DynamicSpearEdge)
		DynamicSpearEdge->SetVectorParameterValue(FName("FillLightVector"), FillLightValue);
}

void ABTCharacterACH::SpawnPBS()
{
	ABTCharacterBase::SpawnPBS();

	//spawn unique projectiles to character and save their states to character state
}
