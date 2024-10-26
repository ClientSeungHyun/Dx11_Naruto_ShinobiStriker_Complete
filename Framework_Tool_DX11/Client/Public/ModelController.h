#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CModelController : public CBase
{
	DECLARE_SINGLETON(CModelController)

public:
	vector<_wstring>&		Get_PrototypeTags() { return m_PrototypeTags; }
	_wstring&				Get_PrtotypeTagIndex(_uint iIndex) { return m_PrototypeTags[iIndex]; }

	void					Add_ProtytypeTag(_wstring strTag) { m_PrototypeTags.emplace_back(strTag); }

private:
	CModelController();
	virtual ~CModelController() = default;

public:
	HRESULT Initialize();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

	vector<_wstring>	m_PrototypeTags;

public:
	virtual void Free() override;
};

END

