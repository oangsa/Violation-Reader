#pragma once

typedef unsigned long long uintptr_t;

namespace Offsets {
	// Hyperion
	constexpr uintptr_t ViolationTable = 0x1190EB8;
	constexpr int ViolationsIndex = 0x10;
	constexpr int EncodedViolations = 0x19;

	constexpr int XorTableIndex = 0x0; // not always present
	
	// Instance
	constexpr uintptr_t DatamodelArbiter = 0x8f8e968;
	constexpr int ArbiterToDatamodel = 0x1F0;
	constexpr int Children = 0x70;
	constexpr int ScriptContext = 0x440;

	// Structs
	constexpr int LGlobal = 0x60; // L->global
	constexpr int RegistryFree = 0x630; // global->registryfree

	// Luau
	constexpr int GlobalStates = 0x188;
	constexpr int ActorState = 0x310;
	constexpr int GlobalState = 0x18;
	constexpr int EncryptedState = 0x220;
}