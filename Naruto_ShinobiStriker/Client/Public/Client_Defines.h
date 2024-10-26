#pragma once

#include <process.h>
#include <queue>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_CUSTOM, LEVEL_CHRSELECT, LEVEL_AME, LEVEL_KONOHA, LEVEL_CHUNIN, LEVEL_END };

	enum SOUND_ID { SOUND_BGM, SOUND_PLAYER, SOUND_SKILL, SOUND_PLAYERVOICE, SOUND_END };	//BGM은 항상 0번

	enum MONSTER_ID { WHITEZETSU, TENTAIL, KURAMA, SUSANOO, MON_END };
}

extern float g_fVolume;
extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;
