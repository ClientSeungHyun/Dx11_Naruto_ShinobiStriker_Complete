#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CTerrainController : public CBase
{
	DECLARE_SINGLETON(CTerrainController)

public:
	typedef struct 
	{
		_bool	m_bWireFrame = true;
		CVIBuffer_Terrain::VIBUFFER_TERRAIN_DESC VIBuffer_Terrain_Desc;

	}TERRAIN_DESC;

private:
	CTerrainController();
	virtual ~CTerrainController() = default;

public:
	class CTerrain*		Get_Terrin() { return m_pTerrain; }
	void				Set_Terrain(class CTerrain* pTerrain) { m_pTerrain = pTerrain; }

	TERRAIN_DESC		Get_TerrainDesc() const { return m_tTerrainDesc; }
	void				Set_TerrainDesc(TERRAIN_DESC* pTerrainDesc) { memcpy(&m_tTerrainDesc, pTerrainDesc, sizeof(TERRAIN_DESC)); }

	_float3				Get_PickingPos() { return m_vPickPos; }
	void				Set_PickPos(_float3 vPickPos) { m_vPickPos = vPickPos; }

public:
	HRESULT Initialize();

private:
	class CGameInstance*	m_pGameInstance = { nullptr };

	class CTerrain*			m_pTerrain = { nullptr };

	TERRAIN_DESC			m_tTerrainDesc = {};

	_float3					m_vPickPos = {};

public:
	virtual void Free() override;
};

	END

