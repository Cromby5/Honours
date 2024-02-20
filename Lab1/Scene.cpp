#include "Scene.h"
#include "Entity.h"
#include <imgui_internal.h>




//ECS Architecture Attempt using EnTT https://github.com/skypjack/entt, bound to be a bit messy to fit the project requirements
// ObjectHandler is not really supposed be used, old code that I didn't want to remove in case I needed it later
Scene::Scene()
{
	//entt::entity entity = m_registry.create();
}

Scene::~Scene()
{
}

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity = Entity(m_registry.create(), this);
	entity.AddComponent<Transform>(); // add transform component to entity, this is a default component for all entities in the scene 
	// A tag component is a component that has no data, it is just a way to tag an entity as something specific, default component
	auto& tag = entity.AddComponent<Tag>();
	tag.name = name.empty() ? "Entity" : name; // if name is empty, set tag to "Entity", otherwise set it to the name passed in
	return entity;
}

void Scene::Update(float deltaTime, const WorldCamera& cam, float counter, float newCounter,bool imgui)
{
	auto& view = m_registry.view<Tag,Transform>();
	// Scene Hierarcy View
	if (imgui)
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "ENTITY LIST");
		ImGui::BeginChild("Scrolling");
		for (auto entity : view)
		{
			auto& tag = view.get<Tag>(entity);
			ImGui::Text("Entity name: %s", tag.name.c_str());
			auto& transformEdit = view.get<Transform>(entity);
			DrawVec3Control("Position", transformEdit.GetPos(), 0.0f, 100.0f);
			DrawVec3Control("Rotation", transformEdit.GetRot(), 0.0f, 100.0f);
			DrawVec3Control("Scale", transformEdit.GetScale(), 0.0f, 100.0f);

		}
		ImGui::EndChild();
	}

	// Rendering 
	auto& group = m_registry.group<Transform>(entt::get<Model, ShaderHandler, TextureMap, Sphere>);
	for (auto entity : group)
	{
		auto& [transform, sceneModel, shader, texMap, sphereCollider] = group.get<Transform, Model, ShaderHandler, TextureMap, Sphere>(entity);
		//std :: cout << "Updating Scene" << std::endl;

		shader.Use();
		shader.Update(transform, cam , 0.5f, 0.5f);
		texMap.Bind(0);

		sceneModel.Draw(shader);

		if (imgui)
		{
			sphereCollider.SetPos(transform.GetPos());
			sphereCollider.SetRadius(1.0f);
		}

	}

	// In future there should be a onUpdate function to listen for here
	// Missile update
	auto& viewMissile = m_registry.view<Missile,Transform>();
	for (auto entity : viewMissile)
	{
		auto& missile = viewMissile.get<Missile>(entity);
		auto& missileTransform = viewMissile.get<Transform>(entity);
		if (missile.CheckLifeTime())
		{
			m_registry.destroy(entity);
			continue;
		}
		else
		{
			missileTransform.MoveForward(9.0f * deltaTime);
			missile.updateMissile(deltaTime);
		}
	}
	// Player update
	if (imgui)
	{
		auto& player = m_registry.view<Transform, Player>();
		for (auto entity : player)
		{
			auto& player1 = player.get<Player>(entity);
			auto& playTransform = player.get<Transform>(entity);
			playTransform.SetPos(cam.GetPos());
			playTransform.SetForward(cam.GetForward());

			//rotate with the camer
		
			playTransform.SetRot(glm::vec3(cam.GetPitch(), cam.GetYaw(), 0.0f));

		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				CreateEntity("Entity");
			}
			ImGui::EndPopup();
		}
	}
}



void Scene::CheckCollisionsEntity(Entity entity, float deltaTime)
{
	
	auto& view = m_registry.view<Sphere>();

	auto& sphereCollider = view.get<Sphere>(entity);

	for (auto otherEnt : view)
	{
		if (otherEnt == entity)
			continue;

		auto& otherSphere = view.get<Sphere>(otherEnt);

			glm::vec3 m1Pos = sphereCollider.GetPos();
			float m1Rad = sphereCollider.GetRadius();
			glm::vec3 m2Pos = otherSphere.GetPos();
			float m2Rad = otherSphere.GetRadius();

			float distance = ((m2Pos.x - m1Pos.x) * (m2Pos.x - m1Pos.x) + (m2Pos.y - m1Pos.y) * (m2Pos.y - m1Pos.y) + (m2Pos.z - m1Pos.z) * (m2Pos.z - m1Pos.z));

			if (distance * distance < (m1Rad + m2Rad))
			{
				m_registry.destroy(otherEnt);
			}
			else
			{
				//return false;
			}
	}
}

// testing a transform view / editor using imgui

void Scene::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}
