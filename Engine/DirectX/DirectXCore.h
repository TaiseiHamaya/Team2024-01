#pragma once

#include <d3d12.h>
#include <memory>
#include "Engine/DirectX/DirectXResourceObject/ConstantBuffer/ConstantBuffer.h"

class PipelineState;
class GameObject;
struct DirectionalLightData;

class DirectXCore {
private:
	DirectXCore();
	
public:
	~DirectXCore();

private:
	DirectXCore(const DirectXCore&) = delete;
	DirectXCore& operator=(const DirectXCore&) = delete;

public:
	static void Initialize();
	static void BeginFrame();
	static void EndFrame();
	static void Finalize();

#ifdef _DEBUG
	static void ShowDebugTools();
#endif // _DEBUG
	static void ShowGrid();

private:
	static DirectXCore& GetInstance();

private:
	void initialize();

	void begin_frame();
	void end_frame();

#ifdef _DEBUG
	void show_debug_tools();
#endif // _DEBUG

private:
	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;

	std::unique_ptr<PipelineState> pipelineState;

	std::unique_ptr<GameObject> gridMesh;
	std::unique_ptr<ConstantBuffer<DirectionalLightData>> light;

private:
	class Debug {
	private:
		Debug() = default;

	public:
		~Debug();

	private:
		Debug(const Debug&) = delete;
		Debug& operator=(const Debug&) = delete;

	public:
		static void Initialize();
		static void InfoQueue();

	private:
		static DirectXCore::Debug& GetInstance();

	private:
	};
};