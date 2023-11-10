#pragma once
#include "header.hpp"
#include "threading.hpp"

namespace Battleye_Internal
{
	void Load( std::uintptr_t handle )
	{
		// 5E AE B7 96 9F E7 F7 73 67 A3 F1 8F 9D 64 3C 64 4D 5D C8 7F 37 81
		std::uintptr_t ExchangePointer = *reinterpret_cast<std::uintptr_t*>( 
			handle + 0x38E19B );
		std::uint8_t CheckBE = *reinterpret_cast<std::uint8_t*>( ExchangePointer + 5 );
		if (CheckBE)
		{
			be_enabled = true;
			
			*reinterpret_cast<std::uint32_t*>( ExchangePointer + 0x30 ) = 0x51445232;

			std::printf("reading ExchangePointer and overriding the previous encryption TAG");

			*reinterpret_cast<std::uint32_t*>( ExchangePointer + 0x4C ) = 0x90;
			*reinterpret_cast<std::uint32_t*>( ExchangePointer + 0x4D ) = 0x90;
			*reinterpret_cast<std::uint32_t*>( ExchangePointer + 0x4E ) = 0x90;
			*reinterpret_cast<std::uint32_t*>( ExchangePointer + 0x4F ) = 0x90;

			std::printf("patched re-allocation stub to prevent our imports from being wiped");
		}
	}
}

namespace DCFN_Internal
{
	namespace Imports
	{
		static std::uintptr_t SpoofCall{};
	}

	void Load_Imports( std::uintptr_t handle, 
		std::uintptr_t* address )
	{
		std::uintptr_t codecave = *reinterpret_cast<std::uintptr_t*>( handle + 0x95A1FAD ); // Unused UFunction
		std::size_t sizeof_imports = sizeof(address);
		for (std::int8_t i = 0; i < sizeof_imports; i++)
		{
			std::uintptr_t codecave_sub = *reinterpret_cast<std::uintptr_t*>(codecave 
				+ i);
			*reinterpret_cast<std::uintptr_t*>(codecave_sub + 1) = *reinterpret_cast<std::uintptr_t*>(address + 1);
		}
		Imports::SpoofCall = reinterpret_cast<std::uintptr_t>(codecave);
	}

	void Main( std::uintptr_t handle )
	{
		if (handle)
		{
			std::printf("main module was allocated correctly");
			base_address = handle;
		}
		Battleye_Internal::Load( handle );
		spoofcall_internal = reinterpret_cast<decltype(spoofcall_internal)>(Imports::SpoofCall);

		spoofcall_internal( std::printf, 
			"hooking an undocumented function to run our main thread" );

		{
			std::uintptr_t firstAddress = *reinterpret_cast<std::uintptr_t*>( handle + 0x3C0A1E6 );
			firstAddress -= 0x2D;

			std::uintptr_t exchangeAddress = *reinterpret_cast<std::uintptr_t*>( firstAddress + 0x10D4 );
			if (be_enabled)
			{
				exchangeAddress = *reinterpret_cast<std::uintptr_t*>( firstAddress + 0x10FB );
			}
			for (std::int8_t i = 0; i < 23; i++)
			{
				std::uint32_t pointer = *reinterpret_cast<std::uint32_t*>( exchangeAddress + i );
				if (!pointer) continue;

				/* first jmp we overwrite our function there */
				if (pointer == 0xE8)
				{
					threading::undocumented_FN = reinterpret_cast<decltype(threading::undocumented_FN)>( exchangeAddress );
					*reinterpret_cast<decltype(threading::undocumented_FN)*>( 
						pointer + 4 ) = &threading::undocumented_FN_Hook;
					break;
				}
			}

			*reinterpret_cast<void**>( 
				handle + 0x5B6C96 ) = &eac::hookCallBackEvent;
		}
	}
}


//void __stdcall injection_bind(std::uintptr_t handle, std::uintptr_t* imports)
//{
//	if (imports)
//	{
//		/*
//		* lets not do some stupid shit that needs updates every game
//		* Using SDK::FMemoryBudget and its variables help us to find loose memory that we can modify
//		* 
//		* Use a Unreal Engine class like PlayerController and something the game uses but find a function that isn't called
//		* We can use FMemory to copy the private function
//		* 
//		*/
//		std::uintptr_t TestCodecave =
//			Cheat_Internals::FindUECodeCave(handle);
//
//		Imports_Test::Load_Imports(TestCodecave,
//			imports);
//	}
//
//	/*
//	* Injection should've been properly done now
//	*/
//}

void __stdcall export_function( std::uintptr_t handle, 
	std::uintptr_t* imports 
)
{
	std::printf("export function executed inside of game module");

	if (imports)
	{
		std::printf("imports have been allocated inside of the export function");
		DCFN_Internal::Load_Imports( handle, 
			imports );

		if (DCFN_Internal::Imports::SpoofCall)
		{
			std::printf("spoofcall is now allocated inside of the games memory");
		}
	}

	DCFN_Internal::Main( handle );
}