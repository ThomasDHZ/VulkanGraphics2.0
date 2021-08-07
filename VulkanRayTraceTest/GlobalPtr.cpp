#include "GlobalPtr.h"

std::shared_ptr<VulkanWindow> GlobalPtr::windowPtr = nullptr;
std::shared_ptr<VulkanEngine> GlobalPtr::enginePtr = nullptr;
std::shared_ptr<AssetManager> GlobalPtr::assetManagerPtr = nullptr;
std::shared_ptr<InputManager> GlobalPtr::inputManagerPtr = nullptr;
std::shared_ptr<CameraManager> GlobalPtr::cameraManagerPtr = nullptr;
std::shared_ptr<MaterialManager> GlobalPtr::materialManagerPtr = nullptr;
std::shared_ptr<TextureManager> GlobalPtr::textureManagerPtr = nullptr;
std::shared_ptr<MeshManager> GlobalPtr::meshManagerPtr = nullptr;
std::shared_ptr<LightManager> GlobalPtr::lightManagerPtr = nullptr;
std::shared_ptr<GUIManager> GlobalPtr::guiManagerPtr = nullptr;
std::shared_ptr<ObjectManager> GlobalPtr::ObjManagerPtr = nullptr;