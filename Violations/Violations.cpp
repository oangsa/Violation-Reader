#include <iostream>

#include "Process/Process.hpp"
#include "Offsets.h"

#define __PAIR64__(high, low) (((uint64_t)high << 32) | (uint32_t)low)

uintptr_t DecryptState(uintptr_t encryptedState) {
	DWORD low = Read<DWORD>(encryptedState) - (DWORD)encryptedState;
	DWORD high = Read<DWORD>(encryptedState + 0x4) - (DWORD)encryptedState;
	return __PAIR64__(high, low);
}

static inline uint8_t __ROL1__(uint8_t x, int n) { return (uint8_t)((x << (n & 7)) | (x >> ((8 - (n & 7)) & 7))); }

int main() {
	pid = GetPIDByName("RobloxPlayerBeta.exe");
	if (!pid) {
		std::cout << "Roblox not found\n";
		return 0;
	}

	pHandle = OpenProcess(PROCESS_ALL_ACCESS & ~PROCESS_TERMINATE, 0, pid);
	if (!pHandle) {
		std::cout << "Failed to open handle to roblox\n";
		return 0;
	}
	
	uintptr_t hyperionBase = GetModuleBase("RobloxPlayerBeta.dll");
	uintptr_t robloxBase = GetModuleBase("RobloxPlayerBeta.exe");

	uintptr_t violationTable = hyperionBase + Offsets::ViolationTable;

	while (true) {
		uint64_t index = (uint64_t)Read<uint8_t>(violationTable + Offsets::ViolationsIndex);
		uintptr_t encodedTable = (uintptr_t)(violationTable + (index * 4) + Offsets::EncodedViolations);
		uintptr_t xorTable = (uintptr_t)(violationTable + Offsets::XorTableIndex);

		uint32_t violations = 0;
		for (int i = 0; i < 4; i++) {
			uint8_t value = Read<uint8_t>(encodedTable + i);
			value = __ROL1__(value, 4);

			violations |= (uint32_t)value << (i * 8);
		}

		if (violations == 0)
			std::cout << "Hyperion Violations: " << std::hex << "0x" << violations << " (clean)\n";
		else
			std::cout << "Hyperion Violations: " << std::hex << "0x" << violations << " (detected)\n";

		uintptr_t dataModel = Read<uintptr_t>(robloxBase + Offsets::DatamodelArbiter);
		if (dataModel) {
			dataModel += Offsets::ArbiterToDatamodel;

			uintptr_t childrenContainer = Read<uintptr_t>(dataModel + Offsets::Children);
			if (childrenContainer) {
				uintptr_t children = Read<uintptr_t>(childrenContainer);
				uintptr_t scriptContext = Read<uintptr_t>(children + Offsets::ScriptContext);

				auto bit = [](uintptr_t ptr, int b) -> int {
					return (Read<uint32_t>(Read<uintptr_t>(ptr + Offsets::LGlobal) + Offsets::RegistryFree) >> b) & 1;
				};

				uintptr_t encryptedL1 = scriptContext + Offsets::GlobalStates + Offsets::GlobalState;
				uintptr_t encryptedL2 = encryptedL1 + Offsets::ActorState;

				uintptr_t L1 = DecryptState(encryptedL1 + Offsets::EncryptedState);
				uintptr_t L2 = DecryptState(encryptedL2 + Offsets::EncryptedState);

				auto result =
					(bit(L1, 28) & bit(L2, 28)) |
					((bit(L1, 29) & bit(L2, 29)) << 1) |
					((bit(L1, 30) & bit(L2, 30)) << 2);

				if (result == 0x7)
					std::cout << "Luau Violations: 0x" << std::hex << result << " (clean)\n";
				else
					std::cout << "Luau Violations: 0x" << std::hex << result << " (detected)\n";
			}
		}

		Sleep(1000);
	}

    return 0;
}