#include "PhysX_Manager.h"
#include "GameInstance.h"

#include "Transform.h"
#include "RigidBody.h"

#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"

CPhysX_Manager::CPhysX_Manager()
{
}

HRESULT CPhysX_Manager::Initialize()
{
    // PhysX Foundation ����
    // PhysX�� ���õ� ��� ��ü���� ����� �Ǵ� �̱��� ��ü
    m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

    // PVD(PhysX Visual Debugger) ����
    m_Pvd = PxCreatePvd(*m_PxFoundation);
    // PVD ���� ���� ����
    m_pTransport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_iPvdPortNumber, m_iPvdTimeOutSeconds);
    m_Pvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

    // PhysX SDK ����
    // ������ ��ü���� ������ �� �ִ� �̱��� ���丮 Ŭ����
    // ���̳� ���͸����� �����ϰų� ���� ��ü�� ��ü ����(Shape), ��ü �ν��Ͻ� �� ���� ��κ��� PhysX ��ü���� ������ �� ���
    m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);
    PxInitExtensions(*m_PhysX, m_Pvd);

    // �� ���� ����
    PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);

    // CPU ����ó(��Ŀ ������) ����
    m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_PxDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    // �� ���� PhysX ���͵��� ��ġ�� �� �ִ� ��
    m_PxScene = m_PhysX->createScene(sceneDesc);

    // PVD Ŭ���̾�Ʈ ����
    // Physx Visual Debugger ���α׷��� �� ���� ������ ������ �� � ������ ������ ���� �÷��׸� �������ִ� ��ü
    m_PvdClient = m_PxScene->getScenePvdClient();
    if (m_PvdClient)
    {
        // PVD Ŭ���̾�Ʈ �÷��� ����
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //��ü ����
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // �浹 ����
        m_PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene ����
    }

    // Material�� �������� ǥ�� ������ ���¸� ��Ÿ���� ��ü
    // ǥ���� �����������, ��������, �ݹ߰�� �� ǥ�� ������ �پ��� ������ ������ �� �ִ�
    //gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    //PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
    //gScene->addActor(*groundPlane);

    return S_OK;
}

void CPhysX_Manager::PhysX_Update(_float fTimeDelta)
{
    if (nullptr == m_pPlayer)
        return;

    Compute_MonsterCollision();

    // ���� �� ������Ʈ
    m_PxScene->simulate(fTimeDelta);
    m_PxScene->fetchResults(true);
}

HRESULT CPhysX_Manager::AddScene_ColMesh(CGameObject* pObject, _wstring strModelName)
{ 
    // ���� ��ü�� �� ������Ʈ�� ����
    CModel* pModel = dynamic_cast<CModel*>(pObject->Find_Component(MODEL));

    // ���� �� �޽��� ���� ó��
    for (auto* Mesh : pModel->Get_Meshes())
    {
        VTXMESH*        pVB = Mesh->Get_Vertices();  // ���� ������
        VTXANIMMESH*    pAnimVB = Mesh->Get_AnimVertices();  // ���� ������
        _uint*          pIB = Mesh->Get_Indices();  // �ε��� ������

        _uint iNumVertices = Mesh->Get_NumVertices();  // ���� ����
        _uint iNumIndices = Mesh->Get_NumIndices();  // �ε��� ����
        _uint iNumTriangles = iNumIndices / 3;  // �ﰢ�� ����

        // ���� ��ǥ�� ������ �������� ����
        vector<PxVec3> Vertices;
        Vertices.reserve(iNumVertices);

        // ���� ��ü�� ���� ��ȯ ���
        _matrix WorldMatrix = pObject->Get_Transform()->Get_WorldMatrix();

        // ���� ��ǥ ��ȯ �� ����
        for (_uint i = 0; i < iNumVertices; ++i)
        {
            _vector vVertexPosition = {};
            if(nullptr == pVB)
                vVertexPosition = XMLoadFloat3(&pAnimVB[i].vPosition);
            else
                vVertexPosition = XMLoadFloat3(&pVB[i].vPosition);

            vVertexPosition = XMVector3TransformCoord(vVertexPosition, WorldMatrix);
            Vertices.push_back(PxVec3(XMVectorGetX(vVertexPosition), XMVectorGetY(vVertexPosition), XMVectorGetZ(vVertexPosition)));
            
        }

        // �ε����� ������ ����
        vector<PxU32> Indices;
        Indices.reserve(iNumIndices);

        // �ε��� ������ ����
        for (_uint i = 0; i < iNumIndices; ++i)
        {
            _uint Index = pIB[i];
            Indices.push_back(Index);
        }

        // �ﰢ�� �޽� ���� ����
        PxTriangleMeshDesc tDesc;
        tDesc.points.count = iNumVertices;
        tDesc.points.stride = sizeof(PxVec3);
        tDesc.points.data = Vertices.data();
        tDesc.triangles.count = iNumTriangles;
        tDesc.triangles.stride = sizeof(PxU32) * 3;
        tDesc.triangles.data = Indices.data();

        // �ﰢ�� �޽� ����
        PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(PxCookingParams(PxTolerancesScale(0.0f, 0.0f)), tDesc);

        // �ﰢ�� �޽� ���� ����
        PxTriangleMeshGeometry* pGeometry = new PxTriangleMeshGeometry(pTriangleMesh);

        // �浹 �޽� ��Ͽ� �߰�
        m_ColMesheGeometries.push_back(pGeometry);

        // RigidStatic ��ü ����
        // �������� �ʴ� ���� �𵨵��� �̰ŷ� ��������
        PxRigidStatic* pActor = m_PhysX->createRigidStatic(Get_PhysXTransform(pObject));
   
        // ���� ���� ����
        PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);
        // �浹 ���� ���� �� �߰�
        PxShape* pShape = m_PhysX->createShape(*pGeometry, *Material);
        pActor->attachShape(*pShape);

        // ���� ���� Actor �߰�
        m_PxScene->addActor(*pActor);

        // ���� Actor ��Ͽ� �߰�
        m_StaticActors.push_back(pActor);
    }

    return S_OK;
}

HRESULT CPhysX_Manager::SetUp_Player(CGameObject* pPlayer)
{
    PxRigidDynamic* pRigid = m_PhysX->createRigidDynamic(Get_PhysXTransform(pPlayer));

    // ���׸��� ȯ�� ����
    // ���� ����, ���� ����, �ݹ� ���
    PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);

    // �������� 0.5�̰� ���̰� 0.3�� ĸ�� ����

    m_PlayerCapusle = PxCapsuleGeometry(0.55f, 0.29f);
    PxShape* pShape = m_PhysX->createShape(m_PlayerCapusle, *Material);

    pRigid->attachShape(*pShape);

    m_PxScene->addActor(*pRigid);
    m_pPlayerRigid = pRigid;

    m_pPlayer = pPlayer;
    Safe_AddRef(m_pPlayer);

    return S_OK;
}

HRESULT CPhysX_Manager::Add_Monster(CGameObject* pMonster)
{
    if (nullptr == pMonster)
        return E_FAIL;

    PxRigidDynamic* pRigid = m_PhysX->createRigidDynamic(Get_PhysXTransform(pMonster));

    // ���׸��� ȯ�� ����
    // ���� ����, ���� ����, �ݹ� ���
    PxMaterial* Material = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);

    // �������� 0.5�̰� ���̰� 0.3�� ĸ�� ����

    PxCapsuleGeometry MonsterCapsule = PxCapsuleGeometry(0.55f, 0.29f);
    PxShape* pShape = m_PhysX->createShape(MonsterCapsule, *Material);

    pRigid->attachShape(*pShape);
    m_PxScene->addActor(*pRigid);

    PxTransform             PxMonsterransform;

    m_PxMonsterTransforms.emplace_back(PxMonsterransform);
    m_MonsterCapsules.emplace_back(MonsterCapsule);
    m_MonsterRigids.emplace_back(pRigid);
    m_Monsters.emplace_back(pMonster);

    return S_OK;
}

_bool CPhysX_Manager::RayCast(PxVec3 vRayPos, PxVec3 vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _bool isHit = false;

    PxRaycastHit hit;
    PxRaycastBuffer hitBuffer(&hit);
    PxReal fLength = 100.f;

    PxVec3 vRayEnd = vRayPos + vRayDir * fLength;

    PxQueryFilterData filterData(PxQueryFlag::eSTATIC);

    // ����ĳ��Ʈ ����
    // ���� ó�� �浹�� ���� ��ȯ eMESH_ANY
    // ����ƽ ��ü���� ���� eSTATIC
    if (isHit = m_PxScene->raycast(vRayPos, vRayDir, fLength, hitBuffer, PxHitFlag::eMESH_ANY, filterData))
    {
        hit = hitBuffer.block;
        PxShape* shape = hitBuffer.block.shape;

        if (shape)
        {
            const PxGeometryHolder geometry = shape->getGeometry();
            PxGeometryType::Enum eType = geometry.getType();

            if (eType == PxGeometryType::eTRIANGLEMESH)
            {
                const PxTriangleMeshGeometry& triangleMeshGeom = geometry.triangleMesh();
                PxTriangleMesh* triangleMesh = triangleMeshGeom.triangleMesh;

                // �浹 ������ ��ġ
                PxVec3 collisionPoint = hitBuffer.block.position;

                // �ﰢ���� ���� ������ ��������
                PxU32 triangleIndex = hitBuffer.block.faceIndex;
                const void* indexBuffer = triangleMesh->getTriangles();
                const PxVec3* vertices = triangleMesh->getVertices();

                // �ﰢ�� �ε��� Ÿ�Կ� ���� ĳ����
                PxU32 idx0, idx1, idx2;

                if (triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES) {
                    const PxU16* indices = reinterpret_cast<const PxU16*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }
                else {
                    const PxU32* indices = reinterpret_cast<const PxU32*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }

                // �ﰢ���� ���� ��ġ ��������
                PxVec3 v0 = vertices[idx0];
                PxVec3 v1 = vertices[idx1];
                PxVec3 v2 = vertices[idx2];

                // �ﰢ���� ���� ���

                // �� ���͸� ����
                PxVec3 edge1 = v1 - v0;
                PxVec3 edge2 = v2 - v1;

                PxVec3 normal = edge1.cross(edge2);
                normal.normalize(); // ������ ����ȭ

                *vNormal = XMVectorSet(normal.x, normal.y, normal.z, 0.f);
                *vHitPos = XMVectorSet(hitBuffer.block.position.x, hitBuffer.block.position.y, hitBuffer.block.position.z, 1.f);
                *fHitDistance = hitBuffer.block.distance;
            }
        }
    }

#ifdef _DEBUG

    if (m_PvdClient && isHit)
    {
        PxDebugLine lines = { PxDebugLine(vRayPos, hitBuffer.block.position, PxDebugColor::eARGB_YELLOW) };
        m_PvdClient->drawLines(&lines, 1);
    }
#endif

    return isHit;
}



_bool CPhysX_Manager::RayCast(_vector vRayPos, _vector vRayDir, _vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _bool isHit = false;

    PxRaycastHit hit;
    PxRaycastBuffer hitBuffer(&hit);
    PxReal fLength = 100.f;

    //PxVec3 vRayEnd = vRayPos + vRayDir * fLength;

    PxVec3 PXvRayPos = PxVec3(XMVectorGetX(vRayPos), XMVectorGetY(vRayPos), XMVectorGetZ(vRayPos));
    PxVec3 PXvRayDir = PxVec3(XMVectorGetX(vRayDir), XMVectorGetY(vRayDir), XMVectorGetZ(vRayDir));

    PxQueryFilterData filterData(PxQueryFlag::eSTATIC);

    // ����ĳ��Ʈ ����
    // ���� ó�� �浹�� ���� ��ȯ eMESH_ANY
    // ����ƽ ��ü���� ���� eSTATIC
    if (isHit = m_PxScene->raycast(PXvRayPos, PXvRayDir, fLength, hitBuffer, PxHitFlag::eMESH_ANY, filterData))
    {
        hit = hitBuffer.block;
        PxShape* shape = hitBuffer.block.shape;

        if (shape)
        {
            const PxGeometryHolder geometry = shape->getGeometry();
            PxGeometryType::Enum eType = geometry.getType();

            if (eType == PxGeometryType::eTRIANGLEMESH)
            {
                const PxTriangleMeshGeometry& triangleMeshGeom = geometry.triangleMesh();
                PxTriangleMesh* triangleMesh = triangleMeshGeom.triangleMesh;

                // �浹 ������ ��ġ
                PxVec3 collisionPoint = hitBuffer.block.position;

                // �ﰢ���� ���� ������ ��������
                PxU32 triangleIndex = hitBuffer.block.faceIndex;
                const void* indexBuffer = triangleMesh->getTriangles();
                const PxVec3* vertices = triangleMesh->getVertices();

                // �ﰢ�� �ε��� Ÿ�Կ� ���� ĳ����
                PxU32 idx0, idx1, idx2;

                if (triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES) {
                    const PxU16* indices = reinterpret_cast<const PxU16*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }
                else {
                    const PxU32* indices = reinterpret_cast<const PxU32*>(indexBuffer);
                    idx0 = indices[3 * triangleIndex];
                    idx1 = indices[3 * triangleIndex + 1];
                    idx2 = indices[3 * triangleIndex + 2];
                }

                // �ﰢ���� ���� ��ġ ��������
                PxVec3 v0 = vertices[idx0];
                PxVec3 v1 = vertices[idx1];
                PxVec3 v2 = vertices[idx2];

                // �ﰢ���� ���� ���

                // �� ���͸� ����
                PxVec3 edge1 = v1 - v0;
                PxVec3 edge2 = v2 - v1;

                PxVec3 normal = edge1.cross(edge2);
                normal.normalize(); // ������ ����ȭ

                *vNormal = XMVectorSet(normal.x, normal.y, normal.z, 0.f);
                *vHitPos = XMVectorSet(hitBuffer.block.position.x, hitBuffer.block.position.y, hitBuffer.block.position.z, 1.f);
                *fHitDistance = hitBuffer.block.distance;
            }
        }
    }

#ifdef _DEBUG

    if (m_PvdClient && isHit)
    {
        PxDebugLine lines = { PxDebugLine(PXvRayPos, hitBuffer.block.position, PxDebugColor::eARGB_YELLOW) };
        m_PvdClient->drawLines(&lines, 1);
    }
#endif

    return isHit;
}

_bool CPhysX_Manager::RayCast_PlayerDown(_vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _vector vPlayerDown = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP)) * -1.f;
    _vector vPlayerLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));
    _vector vPlayerPos = m_pPlayer->Get_Position();

    PxVec3 vRayDir = PxVec3(XMVectorGetX(vPlayerDown), XMVectorGetY(vPlayerDown), XMVectorGetZ(vPlayerDown));
    PxVec3 vRayPos = m_pPlayerRigid->getGlobalPose().p;// +(vRayDir * (0.86f));
    //PxVec3 vRayPos = PxVec3(XMVectorGetX(vPlayerPos), XMVectorGetY(vPlayerPos), XMVectorGetZ(vPlayerPos));

    return RayCast(vRayPos, vRayDir, vHitPos, vNormal, fHitDistance);
}

_bool CPhysX_Manager::RayCast_PlayerLook(_vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _vector vPlayerLook = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));
    PxVec3 vRayDir = PxVec3(XMVectorGetX(vPlayerLook), XMVectorGetY(vPlayerLook), XMVectorGetZ(vPlayerLook));

    PxVec3 vCenterPos = m_pPlayerRigid->getGlobalPose().p;
    PxVec3 vRayPos = m_pPlayerRigid->getGlobalPose().p + (vRayDir * (m_PlayerCapusle.radius + 0.01f));

    return RayCast(vRayPos, vRayDir, vHitPos, vNormal, fHitDistance);
}

_bool CPhysX_Manager::RayCast_PlayerHitDir(_vector* vHitPos, _vector* vNormal, _float* fHitDistance)
{
    _vector vPlayerDown = XMVector3Normalize(m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP)) * -1.f;

    static _float f = 0.01f;
    if (GetKeyState(VK_UP) & 0x8000)
        f += 0.01f;

    if (GetKeyState(VK_DOWN) & 0x8000)
        f -= 0.01f;

    PxVec3 vCenterPos = m_pPlayerRigid->getGlobalPose().p;
    PxVec3 vRayDir = PxVec3(XMVectorGetX(vPlayerDown), XMVectorGetY(vPlayerDown), XMVectorGetZ(vPlayerDown));
    PxVec3 vRayPos = m_pPlayerRigid->getGlobalPose().p + (vRayDir * (0.97f));

    return RayCast(vRayPos, vRayDir, vHitPos, vNormal, fHitDistance);
}

void CPhysX_Manager::Compute_Collision(_vector* vHitDir, _float* fHitLength)
{
    if (nullptr == m_pPlayer)
        return;

    CTransform* PlayerTransformCom = m_pPlayer->Get_Transform();
    Safe_AddRef(PlayerTransformCom);

    _matrix PlayerWorldMatirx = PlayerTransformCom->Get_WorldMatrix();

    _vector vPlayerPos = PlayerWorldMatirx.r[3];
    _vector vPlayerUp = XMVector3Normalize(PlayerWorldMatirx.r[1]);

    PxVec3  vDir = { XMVectorGetX(*vHitDir), XMVectorGetY(*vHitDir), XMVectorGetZ(*vHitDir) };

    _vector vStartPos = vPlayerPos + vPlayerUp * 0.1f; // �Ʒ�
    _vector vEndPos = vPlayerPos + vPlayerUp * 1.5f;

    PxVec3 PxStartPos = { XMVectorGetX(vStartPos), XMVectorGetY(vStartPos), XMVectorGetZ(vStartPos) };
    PxVec3 PxEndPos = { XMVectorGetX(vEndPos), XMVectorGetY(vEndPos), XMVectorGetZ(vEndPos) };

    m_PxPlayerTransform = PxTransformFromSegment(PxStartPos, PxEndPos);

    _bool isCollision = false;
    _bool isGround = false;

    for (auto& Mesh : m_ColMesheGeometries)
    {
        // �浹�� �ݴ� ����, �浹 ����, �˻��� ����(EX: ĸ��), 
        if (PxComputeTriangleMeshPenetration(vDir, *fHitLength, m_PlayerCapusle, m_PxPlayerTransform, *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
        {
            // �浹�� ���̰� 0.1f �̻��̸�
            if (*fHitLength > 0.1f)
            {
                isCollision = true;

                // �浹�� �ݴ� ����
                *vHitDir = XMVector3Normalize(XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f));

                _float fRadain;
                XMStoreFloat(&fRadain, XMVector3Dot(vPlayerUp, *vHitDir));

                // �浹 ������ 45�� �Ʒ��� ���� �پ��ִ°ɷ� �Ǵ�
                _float fAngle = XMConvertToDegrees(acosf(fRadain));
                if (fAngle <= 45.f)
                {
                     isGround = true;
                }
                else
                {
                    if (m_pPlayer->Get_IsCanAttachWall() && 45.f < fAngle && fAngle <= 95.f)
                    {
                        m_pPlayer->Set_IsAttachWall(true);
                        m_pPlayer->Set_Normal(*vHitDir);
                    }
                }

                // �浹�� ������ �ݴ� �������� �浹�� ���̸�ŭ ��ġ�� �̵�
                vPlayerPos = vPlayerPos + (*vHitDir * ((*fHitLength - 0.05f)));
                PlayerTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
            }
        }
    }

    m_pPlayer->Set_IsCollision(isCollision);
    m_pPlayer->Set_IsGround(isGround);

    m_pPlayerRigid->setGlobalPose(m_PxPlayerTransform);

    Safe_Release(PlayerTransformCom);
}

void CPhysX_Manager::Compute_MonsterCollision()
{
    for (_uint i = 0; i < m_Monsters.size(); ++i)
    {
        CTransform* MonsterTransformCom = m_Monsters[i]->Get_Transform();
        Safe_AddRef(MonsterTransformCom);

        CRigidBody* MonsterRigidbodyCom = dynamic_cast<CRigidBody*>(m_Monsters[i]->Find_Component(RIGIDBODY));
        Safe_AddRef(MonsterRigidbodyCom);

        _matrix MonsterWorldMatirx = MonsterTransformCom->Get_WorldMatrix();

        _vector vMonsterPos = MonsterWorldMatirx.r[3];
        _vector vMonsterUp = XMVector3Normalize(MonsterWorldMatirx.r[1]);

        PxVec3  vPxDir;
        PxReal fHitLenght;

        _vector vStartPos = vMonsterPos + vMonsterUp * 0.1f; // �Ʒ�
        _vector vEndPos = vMonsterPos + vMonsterUp * 1.5f;

        PxVec3 PxStartPos = { XMVectorGetX(vStartPos), XMVectorGetY(vStartPos), XMVectorGetZ(vStartPos) };
        PxVec3 PxEndPos = { XMVectorGetX(vEndPos), XMVectorGetY(vEndPos), XMVectorGetZ(vEndPos) };

        m_PxMonsterTransforms[i] = PxTransformFromSegment(PxStartPos, PxEndPos);

        _bool isCollision = false;
        _bool isGround = false;

        for (auto& Mesh : m_ColMesheGeometries)
        {
            // �浹�� �ݴ� ����, �浹 ����, �˻��� ����(EX: ĸ��), 
            if (PxComputeTriangleMeshPenetration(vPxDir, fHitLenght, m_MonsterCapsules[i], m_PxMonsterTransforms[i], *Mesh, PxTransform(PxIDENTITY::PxIdentity), 1))
            {
                // �浹�� ���̰� 0.1f �̻��̸�
                if (fHitLenght > 0.1f)
                {
                    isCollision = true;

                    // �浹�� �ݴ� ����
                    _vector vDir = XMVector3Normalize(XMVectorSet(vPxDir.x, vPxDir.y, vPxDir.z, 0.f));

                    _float fRadain;
                    XMStoreFloat(&fRadain, XMVector3Dot(vMonsterUp, vDir));

                    // �浹 ������ 45�� �Ʒ��� ���� �پ��ִ°ɷ� �Ǵ�
                    _float fAngle = XMConvertToDegrees(acosf(fRadain));
                    if (fAngle <= 45.f)
                    {
                        isGround = true;
                    }

                    // �浹�� ������ �ݴ� �������� �浹�� ���̸�ŭ ��ġ�� �̵�
                    vMonsterPos = vMonsterPos + (vDir * ((fHitLenght - 0.1f)));
                    MonsterTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
                }
            }
        }

        m_Monsters[i]->Set_IsCollision(isCollision);
        m_Monsters[i]->Set_IsGround(isGround);

        m_MonsterRigids[i]->setGlobalPose(m_PxPlayerTransform);

        Safe_Release(MonsterRigidbodyCom);
        Safe_Release(MonsterTransformCom);
    }

    m_MonsterCapsules.clear();
    for (auto& Rigid : m_MonsterRigids)
    {
        PhysX_RELEASE(Rigid);
    }
    m_MonsterRigids.clear();
    m_Monsters.clear();
}

void CPhysX_Manager::Reset_PhsyX()
{
    for (auto& Geometry : m_ColMesheGeometries)
    {
        PhysX_RELEASE(Geometry->triangleMesh);
        Safe_Delete(Geometry);
    }
    m_ColMesheGeometries.clear();

    for (auto& Actor : m_StaticActors)
    {
        PhysX_RELEASE(Actor);
    }
    m_StaticActors.clear();

    PhysX_RELEASE(m_pPlayerRigid);

    for (auto& Rigid : m_MonsterRigids)
    {
        PhysX_RELEASE(Rigid);
    }
    m_MonsterRigids.clear();

    Safe_Release(m_pPlayer);
    m_pPlayer = nullptr;
}

PxTransform CPhysX_Manager::Get_PhysXTransform(CGameObject* pObject)
{
    //�̰� �³�?
    //��ư �������� Transfrom���� ��ȯ
    const _float4x4* WorldMat = pObject->Get_Transform()->Get_WorldMatrix_Ptr();

    PxVec3 position(WorldMat->_41, WorldMat->_42, WorldMat->_43);

    // ȸ�� ���� (3x3 ��� ����� ���ʹϾ����� ��ȯ)
    PxMat33 rotation(
        PxVec3(WorldMat->_11, WorldMat->_12, WorldMat->_13),
        PxVec3(WorldMat->_21, WorldMat->_22, WorldMat->_23),
        PxVec3(WorldMat->_31, WorldMat->_32, WorldMat->_33)
    );
    PxQuat quaternion = PxQuat(rotation);

    return PxTransform(position, quaternion);
}

CPhysX_Manager* CPhysX_Manager::Create()
{
    CPhysX_Manager* pInstance = new CPhysX_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CPhysX_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysX_Manager::Free()
{
    __super::Free();

    Reset_PhsyX();

    PhysX_RELEASE(m_PxScene);

    // PhysX Extensions �ݱ�
    PxCloseExtensions();

    PhysX_RELEASE(m_PxDispatcher);
    PhysX_RELEASE(m_PhysX);
    PhysX_RELEASE(m_pTransport);  
    PhysX_RELEASE(m_Pvd);

    // ���� �������� Release
    PhysX_RELEASE(m_PxFoundation);


}