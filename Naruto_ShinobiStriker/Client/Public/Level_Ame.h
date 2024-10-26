#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_Ame final : public CLevel
{
private:
	CLevel_Ame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Ame() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();	
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Effect();
	HRESULT Ready_Layer_Monster();

	HRESULT Ready_LandObjects();
	HRESULT Ready_CustomPlayer();

	HRESULT Load_Map();
	

public:
	static CLevel_Ame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
