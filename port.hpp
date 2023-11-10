#pragma once
#include "header.hpp"

/*
* Fortnite prod related stuff this is theory to receive other players network information and allow us to read their information
* this is supposed to fool the game as if the enemy is actually the local player based on network information and then allow us to read them as if they are us/our team.
* 
* #rapinguac
* 
*/
namespace port
{
	static std::int32_t localClientPort{};
	static boolean portBindingEnabled{ true };

	static void* (*decryptPtr)(void*, void*, void*); /*
													 use this function do not use the one included in the header (requires constant updates and this is faster)
													 */
	static void (*staticNetworkHandler)(void*, void*, void*, void*, void*);
	void staticNetworkHandler_hk(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5)
	{
		/*
		* IDA shows that arg4 contains replicated enemy packet from server
		* structure is completely encrypted though
		*/

		if (!localClientPort)
			localClientPort = 1270010; /* also known as 127.0.0.1 */

		if (portBindingEnabled)
		{https://github.com/
			spoofcall_internal(decryptPtr,
				arg4, 0, 0);

			std::uintptr_t decryptedStructure = *reinterpret_cast<std::uintptr_t*>( arg4 );
			std::uintptr_t structureBase = *reinterpret_cast<std::uintptr_t*>( 
				decryptedStructure + 10 );
		
			if (
				structureBase) {

				/*
				* data cycle
				*/
				for (std::int8_t x = 0; x++;)
				{
					std::uintptr_t member = *reinterpret_cast<std::uintptr_t*>(structureBase +
						(x * sizeof(std::uint64_t))
						);

					if (!member) break;

					std::uintptr_t size = *reinterpret_cast<std::uintptr_t*>( member + 16 );
					if (size
						== 211) /* network size */
					{
						*reinterpret_cast<std::uintptr_t*>(member + 0x29) =
							localClientPort;
					}
				}

			}
		}

		spoofcall_internal(staticNetworkHandler, arg1, 
			arg2, arg3, arg4, arg5);
	}
}