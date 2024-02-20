#pragma once

#include "Scene.h"
#include <EnTT/entt.hpp>

struct Tag
{
	std::string name;

	Tag() = default;
	Tag(const Tag&) = default;
	Tag(const std::string& tag)
		: name(tag) {}
};

class Entity
{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : m_entity(handle), m_scene(scene) {};
		Entity(const Entity& other) = default; //copy constructor
		~Entity() = default;

		template<typename T>
		bool HasComponent()
		{
			std::cout << "Checking if entity has component " << typeid(T).name() << std::endl;
			return m_scene->m_registry.all_of<T>(m_entity);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			std::cout << "Adding component " << typeid(T).name()  << " to entity" << std::endl;
			// should have a check to see if the component already exists to prevent adding a duplicate
			if (HasComponent<T>())
			{
				std::cout << "Entity already has component " << typeid(T).name() << std::endl;
				//return GetComponent<T>();
			}
			return m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			std::cout << "Getting component " << typeid(T).name() << " from entity" << std::endl;
			// should have a check to see if the component exists to be retrieved
			if (!HasComponent<T>())
			{
				std::cout << "Entity does not have component " << typeid(T).name() << std::endl;
				//return AddComponent<T>();
			}
			return m_scene->m_registry.get<T>(m_entity);
		}

		template<typename T>
		void RemoveComponent()
		{
			std::cout << "Removing component " << typeid(T).name() << " from entity" << std::endl;
			// should have a check to see if the component exists to be removed
			if (!HasComponent<T>())
			{
				std::cout << "Entity does not have component " << typeid(T).name() << std::endl;
				//return AddComponent<T>();
			}
			m_scene->m_registry.remove<T>(m_entity);
		}

		void Destroy()
		{
			std::cout << "Destroying entity" << std::endl;
			m_scene->m_registry.destroy(m_entity);
		}

		operator bool() const { return m_entity != entt::null; }

		operator entt::entity() const { return m_entity; } // this is great, allows us to use the entity like an entt::entity, used above for destroy

		bool operator==(const Entity& other) const 
		{ return m_entity == other.m_entity && m_scene == other.m_scene; }

		bool operator!=(const Entity& other) const
		{ return !(*this == other); }


	private:
		bool active = true;
		entt::entity m_entity{ entt::null }; // set like this since its valid to have an empty entity
		Scene* m_scene = nullptr;
};