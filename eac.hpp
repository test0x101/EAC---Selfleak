#pragma once
#include "header.hpp"

namespace eac
{
	static std::uint16_t handlerCallbacks{}; 
	static std::uint16_t reportAttemps{};
	static std::uint16_t kickAttemps{};

	static boolean findReportBytes(void* packet, std::size_t size)
	{
		boolean validReport = false;
		for (std::int16_t i = 0; i < size; i++)
		{
			std::uint32_t byte = *reinterpret_cast<std::uint32_t*>(&packet + i);
			if (byte == 0xD7)
			{
				byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 1));
				if (byte == 0x8D)
				{
					byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 2));
					if (byte == 0x94)
					{
						byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 3));
						if (byte == 0x80)
						{
							byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 4));
							if (byte == 0x92)
							{
								byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 5));
								if (byte == 0x99)
								{
									byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 6));
									if (byte == 0x2A)
									{
										byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 7));
										if (byte == 0x5E)
										{
											validReport = true;
											reportAttemps++;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (validReport) report_packet = packet;
		return validReport;
	}

	static boolean findKickBytes(void* packet, std::size_t size)
	{
		boolean validKick = false;
		for (std::int16_t i = 0; i < size; i++)
		{
			std::uint32_t byte = *reinterpret_cast<std::uint32_t*>(&packet + i);
			if (byte == 0x28)
			{
				byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 1));
				if (byte == 0x23)
				{
					byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 2));
					if (byte == 0x2A)
					{
						byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 3));
						if (byte == 0x24)
						{
							byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 4));
							if (byte == 0x23)
							{
								byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 5));
								if (byte == 0x24)
								{
									byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 6));
									if (byte == 0x26)
									{
										byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 7));
										if (byte == 0x2A)
										{
											byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 8));
											if (byte == 0x44)
											{
												byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 9));
												if (byte == 0x40)
												{
													byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 10));
													if (byte == 0x6B)
													{
														byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 11));
														if (byte == 0x56)
														{
															byte = *reinterpret_cast<std::uint32_t*>(&packet + (i + 16));
															if (byte == 0x6E)
															{
																validKick = true;
																kickAttemps++;
																break;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (validKick) kick_packet = packet;
		return validKick;
	}

	static void hookCallBackEvent(void* client, void* packet, void* size,
		void* arg4, void* arg5)
	{
		if (client != nullptr)
		{
			eac_client = client;
			handlerCallbacks++;

			/*
			* Ghetto handler
			*/
			if (findReportBytes(packet, size_t(size)))
			{
				spoofcall_internal(std::printf, "report packet Found inside of EAC module");
				return;
			}
			else if (findKickBytes(packet, size_t(size)))
			{
				spoofcall_internal(std::printf, "game kick packet Found inside of EAC module");
				return;
			}
			else
			{
				return;
			}
		}
	}
}