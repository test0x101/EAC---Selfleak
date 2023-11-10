#pragma once
#include <iostream>
#include <windows.h>
using namespace std;

// vars
static void(*spoofcall_internal)( ... );
static void* report_packet{}, * kick_packet{};
static void* eac_client{};
static boolean be_enabled, eac_enabled, use_sme_redirect;
static std::uintptr_t base_address{};

//settings
static boolean memory = true;
static boolean check_if_inside_of_hitbox = true;
static boolean building_AI = false;
static boolean player_port_binding = true;
static boolean queue_to_match_instantly = true;
static boolean unload_cheat = false;