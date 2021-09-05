#include "MegaMan.h"
#include "Water2D.h"

MegaMan::MegaMan() : Sprite()
{
}

MegaMan::MegaMan(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, glm::vec2 StartPos, VkDescriptorSetLayout& layout) : Sprite()
{
	const std::vector<Vertex> MegaManVertices =
	{
		{{0.0f, 0.0f, -0.1f},				 {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{SpriteSize.x, 0.0f, -0.1f},		 {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{SpriteSize.x, SpriteSize.y, -0.1f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
		{{0.0f, SpriteSize.y, -0.1f},		 {0.0f, 0.0f, 1.0f}, {1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
	};

	std::vector<Frame2D> StandFrames;
	StandFrames.emplace_back(Frame2D(glm::vec2(0.0f, 0.0f)));
	StandFrames.emplace_back(Frame2D(glm::vec2(0.0521f, 0.0f)));
	StandFrames.emplace_back(Frame2D(glm::vec2(0.1021f, 0.0f)));

	std::vector<Frame2D> RunFrames;
	RunFrames.emplace_back(Frame2D(glm::vec2(0.16459f, 0.0f)));
	RunFrames.emplace_back(Frame2D(glm::vec2(0.2167f, 0.0f)));
	RunFrames.emplace_back(Frame2D(glm::vec2(0.2708f, 0.0f)));
	RunFrames.emplace_back(Frame2D(glm::vec2(0.2167f, 0.0f)));

	std::vector<Frame2D> SlideFrames;
	SlideFrames.emplace_back(Frame2D(glm::vec2(0.3333f, 0.0f), glm::vec2(1.6875f, 1.0f), 3.0f));

	std::vector<Frame2D> JumpFrames;
	JumpFrames.emplace_back(Frame2D(glm::vec2(0.40412f, 0.0f)));

	std::vector<Frame2D> ShootFrames;
	ShootFrames.emplace_back(Frame2D(glm::vec2(0.60625f, 0.0f)));

	StandAni = Animation2D(StandFrames, 2.0f, 0);
	RunAni = Animation2D(RunFrames, 0.1f, 1);
	SlideAni = Animation2D(SlideFrames, 6.1f, 2);
	JumpAni = Animation2D(JumpFrames, 6.1, 3);
	ShootAni = Animation2D(ShootFrames, 6.1, 4);

	CurrentAni = StandAni;

	MeshTextures MegaManTextures = {};
	MegaManTextures.DiffuseMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/MegaMan_diffuse.png";
	MegaManTextures.SpecularMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/MegaMan_Specular.png";
	MegaManTextures.NormalMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/MegaMan_normal.png";
	MegaManTextures.AlphaMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/MegaMan_Alpha.png";

	ObjectFlagBits = ObjectFlags::Player | ObjectFlags::ApplyGravity;
	SetUpSprite(engine, textureManager, MegaManVertices, MegaManTextures, StartPos, layout);
}

MegaMan::~MegaMan()
{
}

void MegaMan::Update(GLFWwindow* window, VulkanEngine& engine, std::shared_ptr<Camera>& camera, float dt, LightBufferObject light, std::vector<std::shared_ptr<Object2D>>& SpriteList, std::shared_ptr<TextureManager>textureManager)
{
	if (OnGroundCheck(SpriteList))
	{
		JumpLimit = false;
		OnGroundHeight = ObjectColliderList[0]->GetCollider().Bottom;
		MegaManStateBitFlag |= MegaManStatesFlag::StateOnGround;
	}
	else
	{
		MegaManStateBitFlag &= ~MegaManStatesFlag::StateOnGround;
	}

	glm::vec3 MoveDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			//ObjectMesh->properites.ReflectSprite = true;
			MoveDirection = glm::vec3(-5.00f, 0.0f, 0.0f) * dt;
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			//ObjectMesh->properites.ReflectSprite = false;
			MoveDirection = glm::vec3(5.0f, 0.0f, 0.0f) * dt;
		}

		MegaManStateBitFlag |= MegaManStatesFlag::StateRunning;
	}
	else
	{
		MegaManStateBitFlag &= ~MegaManStatesFlag::StateRunning;
	}


	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		MegaManStateBitFlag |= MegaManStatesFlag::StateJump;
		if (OnGroundHeight + JumpHeight >= ObjectMesh->MeshPosition.y &&
			!JumpLimit)
		{
			ObjectMesh->MeshPosition.y += 0.1f;
		}
		else
		{
			JumpLimit = true;
		}
	}

	//CurrentAni.Update();
	//ObjectMesh->properites.UVOffset = CurrentAni.GetCurrentFrame().GetUVOffset();
	//ObjectMesh->properites.UVScale = CurrentAni.GetCurrentFrame().GetUVScale();

	Move(SpriteList, MoveDirection);
	Sprite::Update(engine, dt, camera, light);
}

void MegaMan::AnimationHandler()
{
	if (MegaManStateBitFlag & MegaManStatesFlag::StateOnGround)
	{
		if (CurrentAni.GetAnimationID() != StandAni.GetAnimationID())
		{
			CurrentAni = StandAni;
		}
	}
	else
	{
		CurrentAni = JumpAni;
	}

	if (MegaManStateBitFlag & MegaManStatesFlag::StateRunning | MegaManStatesFlag::StateOnGround)
	{
		if (CurrentAni.GetAnimationID() != StandAni.GetAnimationID())
		{
			CurrentAni = RunAni;
		}
	}
}

void MegaMan::Collision(VulkanEngine& engine, std::vector<std::shared_ptr<Object2D>>& ObjectList)
{
	auto a = ObjectColliderList;
	int ba = 34;
	//for (auto& sprite : SpriteList)
	//{
	//	if (dynamic_cast<Water2D*>(sprite.get()))
	//	{
	//		for (auto spriteCollider : sprite->ObjectColliderList)
	//		{
	//			if (spriteCollider->GetCollider().CollidesWith(spriteCollider->GetCollider()))
	//			{
	//				MegaManStateBitFlag |= MegaManStatesFlag::StateInWater;
	//				break;
	//			}
	//			else
	//			{
	//				MegaManStateBitFlag &= ~MegaManStatesFlag::StateInWater;
	//			}
	//		}
	//	}
	//}
}
