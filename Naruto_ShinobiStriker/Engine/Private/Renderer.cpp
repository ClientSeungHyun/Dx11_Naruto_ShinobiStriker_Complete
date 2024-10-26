#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "BlendObject.h"
#include "UIObject.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Load_StaticLightDepthTexture(const _wstring& strTextureFilePath)
{
	if (FAILED(m_pGameInstance->Load_RenderTargetTexture(TEXT("Target_StaticLightDepth"), strTextureFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Initialize()
{
	_uint	iNumViewPorts = { 1 };

	D3D11_VIEWPORT ViewportDesc{};

	// Context는 생성한 스레드에서만 사용해야함(로더에서 하면 터질 수 있음)
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_StaticLightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionTrail00"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionTrail01"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionTrail02"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionTrail03"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_MotionTrail04"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectDiffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectDown0"), (_uint)(ViewportDesc.Width / 32.f), (_uint)(ViewportDesc.Height / 32.f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_EffectResult"), (_uint)(ViewportDesc.Width), (_uint)(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), (_uint)(ViewportDesc.Width / 32.f), (_uint)(ViewportDesc.Height / 32.f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), (_uint)(ViewportDesc.Width / 32.f), (_uint)(ViewportDesc.Height / 32.f), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* MRT_Lights */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* MRT_ShadowObj*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObj"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/* MRT_StaticShadowObj*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_StaticShadowObj"), TEXT("Target_StaticLightDepth"))))
		return E_FAIL;

	/* MRT_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
		return E_FAIL;

	/* MRT_MotionTrail00 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_MotionTrail00"), TEXT("Target_MotionTrail00"))))
		return E_FAIL;

	///* MRT_Distortion */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
	//	return E_FAIL;

	///* MRT_Distortion */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
	//	return E_FAIL;

	///* MRT_Distortion */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
	//	return E_FAIL;

	///* MRT_Distortion */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
	//	return E_FAIL;

	/* MRT_Distortion */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;

	/* MRT_Effect */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_EffectDiffuse"))))
		return E_FAIL;

	/* MRT_EffectDown */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_EffectDown"), TEXT("Target_EffectDown0"))))
		return E_FAIL;

	/* MRT_EffectResult */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_EffectResult"), TEXT("Target_EffectResult"))))
		return E_FAIL;

	/* MRT_Blur_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;

	/* MRT_Blur_Y */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectDeferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pDistortionShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Distortion.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

	if (FAILED(Ready_LightDepthStencilView()))
		return E_FAIL;

	m_pDistortionTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Effect/T_EFF_Noise_09_M.dds"));

#ifdef _DEBUG

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_StaticLightDepth"), 300.f, 500.f, 200.f, 200.f)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_X"), 1280.f - 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_Y"), 1280.f - 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_EffectDown0"), 1280.f - 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_EffectResult"), 1280.f - 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;*/
	
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_Y"), 1280.f - 100.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_MotionTrail00"), 1280.f - 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject ||
		eRenderGroupID >= RG_END)
		return E_FAIL;

	/* 실제 오브젝트 매니져에 추가된 객체의 주소를 여기에다가 공유했다. */
	m_RenderObjects[eRenderGroupID].emplace_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_ShadowObj()))
		return E_FAIL;
	//if (FAILED(Render_StaticShadow()))
	//	return E_FAIL;
	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_NonLights()))
		return E_FAIL;

	if (FAILED(Render_Final()))
		return E_FAIL;

	//if (FAILED(Render_MotionTrail()))
	//	return E_FAIL;
	//if(FAILED(Render_FinalMotionTrail()))
	//	return E_FAIL;

	if (FAILED(Render_Final()))
		return E_FAIL;

	if (FAILED(Render_Effect()))
		return E_FAIL;
	if (FAILED(Render_EffectDown()))
		return E_FAIL;
	if (FAILED(Render_EffectBlur()))
		return E_FAIL;
	if (FAILED(Render_EffectDeferred()))
		return E_FAIL;

	if (FAILED(Render_Final()))
		return E_FAIL;

	if (FAILED(Render_Distortion()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif	

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowObj()
{	
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObj"), m_pLightDepthStencilView)))
		return E_FAIL;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pGameObject : m_RenderObjects[RG_SHADOWOBJ])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_SHADOWOBJ].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal + Specular */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONBLEND].clear();


	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	/* Shade타겟을 바인딩하고 빛연산한 결과를 그려주겠다. */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_LigthDepthPosition(), m_pGameInstance->Get_LigthDepthAt(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	
	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_StaticLightViewMatrix", &m_pGameInstance->Get_StaticLightViewMatrix())))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_StaticLightProjMatrix", &m_pGameInstance->Get_StaticTransform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_LightDepth"), "g_LightDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_StaticLightDepth"), "g_StaticLightDepthTexture")))
		return E_FAIL;
	
	//if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_StaticLightDepth"), "g_LightDepthTexture")))
	//	return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_BackTexture", m_pGameInstance->Get_BackBuffer_SRV())))
		return E_FAIL;

	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;

	m_pShader->Begin(5);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Blur_X"), "g_BlurXTexture")))
		return E_FAIL;

	m_pShader->Begin(6);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Blur_Y"), "g_BlurYTexture")))
		return E_FAIL;

	m_pShader->Begin(7);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_MotionTrail()
{
	return E_NOTIMPL;
}

HRESULT CRenderer::Render_NonLights()
{
	for (auto& pGameObject : m_RenderObjects[RG_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pDistortionShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_fDistortionTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_60"));
	if (m_fDistortionTime >= 1.f)
		m_fDistortionTime = 0.1f;

	if (FAILED(m_pDistortionShader->Bind_RawValue("g_fTime", &m_fDistortionTime, sizeof(_float))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_DISTORTION])
	{

		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_DISTORTION].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pDistortionShader, TEXT("Target_Final"), "g_BackTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pDistortionShader, TEXT("Target_Distortion"), "g_DistortionTexture")))
		return E_FAIL;

	if (FAILED(m_pDistortionTexture->Bind_ShadeResource(m_pDistortionShader, "g_NoiseTexture", 0)))
		return E_FAIL;

	m_pDistortionShader->Begin(1);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"))))
		return E_FAIL;

	_vector vCamPosition = m_pGameInstance->Get_CamPosition_Vector();

	m_RenderObjects[RG_EFFECT].sort([&](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			_float fDistSour, fDistDest;
			fDistSour = XMVectorGetX(XMVector3Length(pSour->Get_Position() - vCamPosition));
			fDistDest = XMVectorGetX(XMVector3Length(pDest->Get_Position() - vCamPosition));

			return fDistSour > fDistDest;
		});

	for (auto& pGameObject : m_RenderObjects[RG_EFFECT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_EFFECT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_EffectDown()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_EffectDown"))))
		return E_FAIL;

	if (FAILED(m_pEffectShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pShader->Bind_SRV("g_DiffuseTexture", m_pGameInstance->Get_BackBuffer_SRV())))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pEffectShader, TEXT("Target_EffectDiffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	m_pEffectShader->Begin(2);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_EffectBlur()
{
	if (FAILED(m_pEffectShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pEffectShader, TEXT("Target_EffectDown0"), "g_FinalTexture")))
		return E_FAIL;

	m_pEffectShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pEffectShader, TEXT("Target_Blur_X"), "g_BlurXTexture")))
		return E_FAIL;

	m_pEffectShader->Begin(4);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_EffectResult"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pEffectShader, TEXT("Target_EffectDiffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pEffectShader, TEXT("Target_Blur_Y"), "g_BlurYTexture")))
		return E_FAIL;

	m_pEffectShader->Begin(5);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_EffectDeferred()
{
	if (FAILED(m_pEffectShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pEffectShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pEffectShader, TEXT("Target_EffectResult"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pEffectShader, TEXT("Target_Final"), "g_BackTexture")))
		return E_FAIL;

	
	m_pEffectShader->Begin(1);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	_vector vCamPosition = m_pGameInstance->Get_CamPosition_Vector();

	m_RenderObjects[RG_BLEND].sort([&](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			_float fDistSour, fDistDest;
			fDistSour = XMVectorGetX(XMVector3Length(pSour->Get_Position() - vCamPosition));
			fDistDest = XMVectorGetX(XMVector3Length(pDest->Get_Position() - vCamPosition));

			return fDistSour > fDistDest;
		});

	for (auto& pGameObject : m_RenderObjects[RG_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RG_UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return dynamic_cast<CUIObject*>(pSour)->Get_Depth() < dynamic_cast<CUIObject*>(pDest)->Get_Depth();
		});

	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_StaticShadow()
{
	if (KEY_TAP(KEY::ENTER))
		m_isRenderStaticLight = true;

	if (!m_isRenderStaticLight)
	{
		m_RenderObjects[RG_STATICSHADOWOBJ].clear();
		return S_OK;
	}

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_StaticShadowObj"), m_pStaticLightDepthStencilView)))
		return E_FAIL;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& iter : m_RenderObjects[RG_STATICSHADOWOBJ])
	{
		if (FAILED(iter->Render_LightDepth()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RG_STATICSHADOWOBJ].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTargetTexture(TEXT("../Bin/DataFiles/StaticLight/StaticLighDepth.dds"), TEXT("Target_StaticLightDepth"))))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	m_isRenderStaticLight = false;

	return S_OK;
}

HRESULT CRenderer::Ready_LightDepthStencilView()
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


	ID3D11Texture2D* pStaticDepthStencilTexture = nullptr;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pStaticDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pStaticDepthStencilTexture, nullptr, &m_pStaticLightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pStaticDepthStencilTexture);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	if (KEY_TAP(KEY::F1))
		m_isDebugRender = !m_isDebugRender;

	if (!m_isDebugRender)
		return S_OK;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_ShadowObj"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_StaticShadowObj"), m_pShader, m_pVIBuffer);
	
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Final"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Blur_X"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Blur_Y"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_MotionTrail00"), m_pShader, m_pVIBuffer); 
	
	return S_OK;
}
#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);

		RenderObjects.clear();
	}

	Safe_Release(m_pLightDepthStencilView);
	Safe_Release(m_pStaticLightDepthStencilView);
	Safe_Release(m_pShader);
	Safe_Release(m_pEffectShader);
	Safe_Release(m_pDistortionShader);
	Safe_Release(m_pDistortionTexture);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

}
