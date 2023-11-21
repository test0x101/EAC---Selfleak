#pragma once
#include "header.hpp"

namespace screenshot_saver
{
	/*
	* main function
	*/
	const std::uintptr_t screenshot_function = 0x94E0D12;
	const void(*screenShot_cel)(void*, void*, void*, void*, void*, void*, void*, void*);

	///*
	//* replicated function with data exporting so we can look at the screenshots
	//*/
	//void fakescreenShot(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
	//{
	//	//redacted
	//}

	void hk_screenShot_cel(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
	{
		//if (a1 != nullptr)
		//{
		//	static char* ctx = {};

		//	void** table = (void**)a1;
		//	auto member = table[39];
		//	ctx = *reinterpret_cast<char**>((std::uintptr_t)member + 6);

		//	if (*(char**)(a5) == ctx)
		//	{
		//		/*
		//		* report ss
		//		*/

		//		fakescreenShot(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		//	}
		//}

		return screenShot_cel(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
	}
}
