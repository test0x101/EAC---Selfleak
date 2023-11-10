#pragma once
#include "eac.hpp"
#include "cheat.hpp"

namespace threading
{
	void(*undocumented_FN)( void*, void*,
		void*, void*, void* );

	void* read_memory_safe( void* address )
	{
		/*
		* rubbish to convert from memory to the actual page
		*/
		void* virtualPage = *reinterpret_cast<void**>((reinterpret_cast<std::uintptr_t>(address) % 0x1000) /
			16);
		/*
		* dependent on process bit (x64 or x32)
		*/
		void* merge = *reinterpret_cast<void**>( reinterpret_cast<std::uintptr_t>(virtualPage) - 64 );
	
		return *reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(merge) -
			reinterpret_cast<std::uintptr_t>(address));
	}

	void undocumented_FN_Hook(void* arg1, void* arg2,
		void* arg3, void* arg4, void* arg5)
	{
		/*
		* at this point some type of game play clip was sent to EAC/BE so lets just not run the cheat.
		*/
		if (eac::reportAttemps > 5)
		{
			return spoofcall_internal(undocumented_FN, arg1, arg2, arg3, arg4, arg5);
		}

		if (be_enabled)
		{
			/*
			* 
			* testing if BE is actually loaded or was silent loaded
			* 
			*/

			auto arg = read_memory_safe(arg5);
			if (arg)
			{
				spoofcall_internal(std::printf, "cheat has detected a Battleye anomaly which can alter the expereince while using our service.");
			}
		}

		/*
		* cheat here
		*/
		if (!cheat::MyCanvas::Draw)
		{
			cheat::controller = cheat::stats->GetPlayerController(cheat::world, 0);
			cheat::MyCanvas::Init(cheat::controller->MyHUD);
		}

		if (unload_cheat)
		{
			cheat::renderer->EndDrawCanvasToRenderTarget(cheat::world, cheat::MyCanvas::Context);
		}

		return spoofcall_internal(undocumented_FN, arg1, arg2, arg3, arg4, arg5);
	}
}