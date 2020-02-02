#pragma once

#include "html/IHtmlSystem.h"

extern IHtmlSystem* g_pHtmlSystem;

bool HtmlSystem_Init();
void HtmlSystem_Tick();
void HtmlSystem_Shutdown();