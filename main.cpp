#include "Engine/WinApp.h"

#include <cstdint>
#include <format>
#include <unordered_set>

#include "externals/imgui/imgui.h"

#include "Engine/DirectX/DirectXCore.h"
#include "Engine/DirectX/DirectXResourceObject/Texture/TextureManager/TextureManager.h"
#include "Engine/GameObject/GameObject.h"
#include "Engine/GameObject/PolygonMesh/PolygonMeshManager/PolygonMeshManager.h"
#include "Engine/GameObject/SpriteObject.h"
#include "Engine/Math/Camera2D.h"
#include "Engine/Math/Camera3D.h"
#include "Engine/Render/RenderPathManager/RenderPathManager.h"
#include "Engine/Utility/BackgroundLoader/BackgroundLoader.h"
#include "Engine/Utility/ImGuiLoadManager/ImGuiLoadManager.h"

#include "Engine/Render/RenderPath/RenderPath.h"
#include "Engine/Render/RenderNode/Object3DNode/Object3DNode.h"
#include "Engine/Render/RenderNode/Sprite/SpriteNode.h"
#include "Engine/Render/RenderNode/Grayscale/GrayscaleNode.h"
#include "Engine/DirectX/DirectXSwapChain/DirectXSwapChain.h"
#include "Engine/Render/RenderNode/ChromaticAberration/ChromaticAberrationNode.h"
#include "Engine/Render/RenderNode/RadialBlur/RadialBlurNode.h"

// クライアント領域サイズ
const std::int32_t kClientWidth = 1280;
const std::int32_t kClientHight = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp::Initialize("DirectXGame", kClientWidth, kClientHight);
	auto camera3D = std::make_unique<Camera3D>();
	camera3D->initialize();
	camera3D->set_transform({
		CVector3::BASIS,
		Quaternion::EulerDegree(45, 0, 0),
		{ 0, 10, -10 }
		});
	Camera2D::Initialize();

	TextureManager::RegisterLoadQue("./Engine/Resources", "uvChecker.png");
	PolygonMeshManager::RegisterLoadQue("./Engine/Resources", "Sphere.obj");
	BackgroundLoader::WaitEndExecute();

	std::shared_ptr<Object3DNode> object3DNode{ new Object3DNode };
	object3DNode->initialize();
	object3DNode->set_render_target();
	std::shared_ptr<GrayscaleNode> grayscaleNode{ new GrayscaleNode };
	grayscaleNode->initialize();
	grayscaleNode->set_render_target();
	grayscaleNode->set_texture_resource(object3DNode->result_stv_handle());
	std::shared_ptr<RadialBlurNode> radialBlurNode{ new RadialBlurNode };
	radialBlurNode->initialize();
	radialBlurNode->set_render_target();
	radialBlurNode->set_texture_resource(grayscaleNode->result_stv_handle());
	std::shared_ptr<ChromaticAberrationNode> chromaticAberrationNode{ new ChromaticAberrationNode };
	chromaticAberrationNode->initialize();
	//chromaticAberrationNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
	chromaticAberrationNode->set_render_target();
	chromaticAberrationNode->set_texture_resource(radialBlurNode->result_stv_handle());
	std::shared_ptr<SpriteNode> spriteNode{ new SpriteNode };
	spriteNode->initialize();
	spriteNode->set_background_texture(chromaticAberrationNode->result_stv_handle());
	spriteNode->set_render_target_SC(DirectXSwapChain::GetRenderTarget());
	RenderPath path;
	path.initialize({ object3DNode, grayscaleNode, radialBlurNode, chromaticAberrationNode, spriteNode });

	RenderPathManager::RegisterPath("GrayScale1", std::move(path));
	RenderPathManager::SetPath("GrayScale1");

	std::vector<GameObject> objects;
	std::vector<std::string> objectNames;
	objects.emplace_back("Sphere.obj");
	objectNames.emplace_back("Sphere");
	std::unordered_multiset<std::string> objectList;
	objectList.emplace("Sphere");
	SpriteObject sprite{ "uvChecker.png", {0.5f,0.5f} };

	bool isShowGrid = true;

	std::string selectMesh;
	char name[1024]{};

	while (!WinApp::IsEndApp()) {
		WinApp::BeginFrame();
#ifdef _DEBUG
		// カメラ、ライトのImGui
		DirectXCore::ShowDebugTools();

		ImGui::Begin("Camera3D");
		camera3D->debug_gui();
		ImGui::End();

		// メインImGuiウィンドウ
		//ImGui::SetNextWindowSize(ImVec2{ 330,130 }, ImGuiCond_Once);
		//ImGui::SetNextWindowPos(ImVec2{ 20, 205 }, ImGuiCond_Once);
		ImGui::Begin("Main", nullptr);
		PolygonMeshManager::MeshListGui(selectMesh);
		ImGui::InputText("Name", const_cast<char*>(name), 1024);
		if (ImGui::Button("CreateObject") && !selectMesh.empty()) {
			std::string objName;
			if (name[0] == '\0') {
				objName = selectMesh;
			}
			else {
				objName = name;
			}
			if (!objectList.contains(objName)) {
				objects.emplace_back(selectMesh);
				objectNames.emplace_back(objName);
				objectList.emplace(objName);
			}
			else {
				objects.emplace_back(selectMesh);
				std::string addName = std::format("{}-{}", objName, objectList.count(objName));
				objectNames.push_back(addName);
				objectList.insert(addName);
				objectList.insert(objName);
			}
		}
		ImGui::Separator();
		ImGui::Checkbox("IsShowGrid", &isShowGrid);
		ImGui::End();

		// ロード関連ImGui
		ImGuiLoadManager::ShowGUI();

		// オブジェクト用ImGui
		ImGuiID objectDock = ImGui::GetID("ObjectDock");
		ImGui::SetNextWindowDockID(objectDock, ImGuiCond_FirstUseEver);
		//ImGui::SetNextWindowSize(ImVec2{ 345,445 }, ImGuiCond_FirstUseEver);
		//ImGui::SetNextWindowPos(ImVec2{ 900, 20 }, ImGuiCond_FirstUseEver);
		ImGui::Begin("Sprite", nullptr);
		sprite.debug_gui();
		ImGui::End();

		for (int i = 0; i < objects.size(); ) {
			ImGui::SetNextWindowDockID(objectDock, ImGuiCond_FirstUseEver);
			//ImGui::SetNextWindowSize(ImVec2{ 345,445 }, ImGuiCond_FirstUseEver);
			//ImGui::SetNextWindowPos(ImVec2{ 900, 20 }, ImGuiCond_FirstUseEver);
			ImGui::Begin(objectNames[i].c_str(), nullptr);
			objects[i].debug_gui();
			if (ImGui::Button("Destroy")) {
				objects.erase(objects.begin() + i);
				objectNames.erase(objectNames.begin() + i);
			}
			else {
				++i;
			}
			ImGui::End();
		}

		ImGui::Begin("PostEffects");
		grayscaleNode->debug_gui();
		ImGui::Separator();
		chromaticAberrationNode->debug_gui();
		ImGui::Separator();
		radialBlurNode->debug_gui();
		ImGui::End();

#endif // _DEBUG

		Camera2D::CameraUpdate();
		camera3D->update_matrix();
		camera3D->update();

		RenderPathManager::BeginFrame();

		for (int i = 0; i < objects.size(); ++i) {
			objects[i].begin_rendering(*camera3D);
		}

		for (int i = 0; i < objects.size(); ++i) {
			objects[i].draw();
		}
		sprite.begin_rendering();

		if (isShowGrid) {
			DirectXCore::ShowGrid(*camera3D);
		}

		RenderPathManager::Next();

		grayscaleNode->draw();

		RenderPathManager::Next();

		radialBlurNode->draw();

		RenderPathManager::Next();

		chromaticAberrationNode->draw();

		RenderPathManager::Next();

		sprite.draw();

		RenderPathManager::Next();

		WinApp::EndFrame();
	}

	WinApp::Finalize();
}