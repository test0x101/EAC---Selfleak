#pragma once
#include "header.hpp"

namespace sme_bypass
{
	/*
	*	bypass for sme allows us to do things like instant queue and sever related stuff
	*	well this is just an exploit the name may be weird or a throw off to what it really does.
	* 
	*	this will allow you to skip queue for matchmaking which is an exploit this means you could load into forntite on a banned account.	
	* 
	*	currently supports latest FN - Epic Games Launcher version
	*/

	/*
	*	sorry about the crazy amounts of shellcode loading, there is not other work around that I have personally discovered yet.
	*   this is way more efficient than hooking a bunch of functions etc..
	*/

	static void* sme_bypass_standalone = {};

	void sme_redirect_function(std::uintptr_t VDS_storage, std::uintptr_t partition)
	{
		/*
		* handles indirect & direct symbols
		*/

		std::uintptr_t* pending_reqx = *reinterpret_cast<std::uintptr_t**>(VDS_storage + 0x16D);
		size_t          reqx_default_size = *reinterpret_cast<std::size_t*>(VDS_storage + 0x18D);

		for (std::int16_t x = 0; x < reqx_default_size; x++)
		{
			std::uintptr_t current_reqx = pending_reqx[x];
			if (!current_reqx) continue; /* invalid request */

			if (current_reqx == 0x1C00F00 || 
				current_reqx == 0x1C00E9E)
			{
				/*
				* VDS doesn't like inequal responses so it will abort in cases which this occurs
				* SME starts a sequence function which reports any failed queue's and helps with logging attempts
				* SME also loads type aquire to help with supporting the VDS System.
				*/

				std::uintptr_t copyed_response = *reinterpret_cast<std::uintptr_t*>( 
					partition + 0x405 );
				std::int32_t attempts = *reinterpret_cast<std::int32_t*>( VDS_storage + 0x2C0 );
				attempts ^= 6; /* no reason for epic games developers to implement some type of encryption like this as its useless. */

				std::int16_t our_attempts = x + attempts;
				*reinterpret_cast<std::uintptr_t*>(current_reqx + 0x13) =
					our_attempts;

				std::uintptr_t copyed_response_skip_function[36] =
				{
					0x80, 0x30, 0x59, 0x19, 0x60,
					0x10, 0x86, 0x75, 0x40, 0x10,
					0x99, 0x20, 0x53, 0x21, 0x18,
					0x29, 0x30, 0x16, 0x49, 0x30,
					0x89, 0xE8, 0x20, 0x19, 0x85, /* runs requested function & skips response validation */
					0x39, 0x60, 0x13, 0x60, 0x20,
					0x73, 0x90, 0x20, 0x52, 0x64,
					0xC3
				};
				std::uintptr_t copyed_response_pointer = *reinterpret_cast<std::uintptr_t*>(copyed_response - 0x84);
				for (std::int16_t i = 0; i < 36; i++)
				{
					*reinterpret_cast<std::uint32_t*>(copyed_response_pointer + i) =
						copyed_response_skip_function[i];
				}

				/*
				* applys shellcode function ^
				*/

				*reinterpret_cast<std::uintptr_t*>(current_reqx + 0x107) =
					copyed_response;

				/*
				* now watch as you instantly load into a game (does not bypass any type of KICK by in-game anti-cheats and external anti-cheats)
				*/
			}
		}
	}

	void load_exploit( )
	{
		if (!sme_bypass_standalone)
		{
			std::uintptr_t faulty_page = *reinterpret_cast<std::uintptr_t*>(base_address + 0xB464118);
			if (faulty_page == 0x100F00 ||
				faulty_page == 0x100F40 || faulty_page == 0x100742)
			{
				/*
				* this shellcode function copys a readable/writeable sme access address,
				* which is then writen to our pointer from our module.
				*/
				std::uint32_t shellcode[21] =
				{
					0x74, 0x39, 0x00, 0x00,
					0x00, 0x00, 0x50, 0x46,
					0x83, 0x90, 0x20, 0x10,
					0x24, 0x89, 0x70, 0x40,
					0x93, 0x13, 0x83, 0x10,
					0xC3
				};
				shellcode[2] = reinterpret_cast<std::uint32_t>(sme_bypass_standalone);

				std::uintptr_t translation_from_page = *reinterpret_cast<std::uintptr_t*>(faulty_page + 0x6C);
				for (std::int16_t i = 0; i < 21; i++)
				{
					*reinterpret_cast<std::uint32_t*>(translation_from_page + i) =
						shellcode[i];
				}
			}
		}

		/*
		* credits to the person who showed me the direct functionality of SME and directed me towards the
		* correct implement that would help with removing this function.
		*/

		std::uintptr_t SME = *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(sme_bypass_standalone)
			+ 0x330);
		if (SME > 0x7FE)
		{
			/*
			* the function has three different parts (VDS)
			* - validation
			* - decryption
			* - storage
			*
			* we are interested in the storage function as its what allows for us to symbolically aquire queue for matchmaking
			*
			*/

			std::uintptr_t VDS_storage = *reinterpret_cast<std::uintptr_t*>(SME + 0x16F4);
			if (use_sme_redirect)
			{
				std::uintptr_t partition = *reinterpret_cast<std::uintptr_t*>(VDS_storage + 0x4A);
				std::uintptr_t partition2 = *reinterpret_cast<std::uintptr_t*>(partition + 0x223);
				std::uintptr_t partition3 = *reinterpret_cast<std::uintptr_t*>(partition2 + 0x3D9);
				if (partition3 == 0x300F ||
					partition3 == 0x304F)
				{
					sme_redirect_function(VDS_storage, partition3);
				}
			}
		}
	}
}
