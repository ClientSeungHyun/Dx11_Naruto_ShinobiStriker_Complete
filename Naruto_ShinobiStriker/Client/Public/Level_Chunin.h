#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_Chunin final : public CLevel
{
private:
	CLevel_Chunin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Chunin() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CSasuke_Next*		m_pNpc_Sasuke = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };
	class CBoss_Susanoo*	m_pSusanoo = { nullptr };
	class CPlayerCamera*	m_pPlayerCamera = { nullptr };

	_bool m_isControlCamera = { true };
	_bool m_isCameraView1 = { false };
	_bool m_isCameraView2 = { false };
	_bool m_isCaemraViewPlayer = { false };
	_bool m_isCameraInit = { false };
	_bool m_isAppearUI = { false };
	_bool m_isEnd_FirstText = { false };
	_bool m_isEnd_SecondText = { false };

	_bool m_isSummonMonster = { false };

	_bool m_isSummonBoss = { false };
	_bool m_isBossAppearCamera = { false };
	_bool m_isBossCameraView1 = { false };
	_bool m_isBossCameraView2 = { false };

	_float m_fSecondTextTime = { 0.f };
	_float m_fBossCameraTime = { 0.f };


	vector<_wstring>			m_TextList01;
	vector<_wstring>			m_TextList02;
	_uint						m_iTextNum = { 0 };

	_float4						m_vStaticLightDepthPos = {};
	_float4						m_vStaticLightDepthAt = {};

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Effect();

	HRESULT Ready_LandObjects();
	HRESULT Ready_Player();
	HRESULT Summon_Boss();
	HRESULT Summon_WhiteZetsu();
	HRESULT Ready_NPC();

	HRESULT Load_Map();

	HRESULT Ready_NpcText();

	void Start_Camera();
	void FirstText();
	void SecondText();


private:
	void Clear_ObjectPool();

public:
	static CLevel_Chunin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
