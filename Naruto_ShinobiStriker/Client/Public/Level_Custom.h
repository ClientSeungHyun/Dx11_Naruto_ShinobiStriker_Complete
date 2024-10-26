#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Custom final : public CLevel
{
private:
	CLevel_Custom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Custom() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_CustomPlayer();
	HRESULT Ready_Layer_UI();

private:
	class CSound* m_pSoundCom = { nullptr };

	class CPlayer_Custom*			m_pCustomPlayer = { nullptr };
	class CCustomCamera*		m_pCustomCamera = { nullptr };

	_uint							m_iCurrentPannelIndex = { 0 };
	_uint							m_iNextPannelIndex = { 0 };

	_bool							m_isSelectConfirm = { false };
	_bool							m_isChanging = { false };

	vector<class CCustomPannel*>	m_Pannels;
	class CUI_ConfirmButton*		m_pConfirmButton = { nullptr };

	_float4						m_vStaticLightDepthPos = {};
	_float4						m_vStaticLightDepthAt = {};


public:
	static CLevel_Custom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
