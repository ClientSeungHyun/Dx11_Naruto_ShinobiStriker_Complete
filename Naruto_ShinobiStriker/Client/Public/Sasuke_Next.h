#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_TextWindow.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFsm;
END

BEGIN(Client)

class CSasuke_Next final : public CGameObject
{
public:
	enum SASUKE_ANIM { ANIM_APPEAR, ANIM_EXIT, ANIM_REAC1, ANIM_REAC2, ANIM_SPEAK, ANIM_END };
	enum SAUSKE_STATE { ST_IDLE, ST_SPEAK, ST_EXIT ,ST_END };
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3		vPosition = _float3(0.f, 0.f, 0.f);
		_float3		vScale = _float3(1.f, 1.f, 1.f);
		_float3		vRotation = _float3(0.f, 0.f, 0.f);
	}NPC_DESC;

private:
	CSasuke_Next(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSasuke_Next(const CSasuke_Next& Prototype);
	virtual ~CSasuke_Next() = default;

public:
	SAUSKE_STATE	Get_State() { return m_eState; }
	void			Set_State(SAUSKE_STATE eState) { m_eState = eState; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	void Appear();
	void Exit();

	void Change_SpeakMode(class CPlayerCamera* pPlayerCamera, _bool isCameraiControl = true);
	void End_SpeakMode();
	void Change_TextWindow(_wstring strText);

private:
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };

	class CUI_TextWindow* m_pUITextWindow = { nullptr };

	_uint m_iAnimationIndex[ANIM_END];

	_bool m_isRender = { false };
	_bool m_isSpeak = { false };

	SAUSKE_STATE	m_eState = { ST_END };
private:
	HRESULT Ready_Components();
	HRESULT Ready_UI();

	void	Update_Chunin(_float fTimeDelta);

public:
	static CSasuke_Next* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END