#pragma once

#include "Config.h"
#include "Timestep.h"
#include "UUID.h"
#include "Renderer/Camera.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Texture.h"
#include "UE_Assert.h"
#include <entt.hpp>
#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Auxiliaries/Physics.h"
#include "Components.h"

namespace UE {

    class Entity;

    class  Scene{
    public:
        Scene();
        Scene(uint32_t width, uint32_t height);
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);      
        template<typename Entt, typename Comp, typename Task>
        void ViewEntity(Task&& task){
            // UE_CORE_ASSERT(std::is_base_of<Entity, Entt>::value, "error viewing entt");
            m_Registry.view<Comp>().each([this, &task] 
            (const auto entity, auto& comp) 
            { 
                // task(std::move(Entt(&m_Registry, entity)), comp);
                task(std::move(Entt(entity, this)), comp);
            });
        }

        template<typename Comp, typename Task>
        void GroupEntity(Task&& task){
            auto group = m_Registry.group<Comp>(entt::get<TransformComponent>);
            for(auto entity : group){
                const auto& [transform, comp] = group.template get<TransformComponent, Comp>(entity);
                task(std::move(Entity(entity, this)), comp, transform, entity);
            }
        }

        void OnRuntimeStart();
		void OnRuntimeStop();
        void PhysicsUpdate(float dt);

        void OnUpdateRuntime(Timestep ts, int& mouseX, int& mouseY, glm::vec2& viewportSize);  
        void OnUpdateEditor(Timestep ts, EditorCamera& camera, int& mouseX, int& mouseY, glm::vec2& viewportSize);
        void DrawScreen(Ref<Framebuffer>& buffer, EditorCamera& camera);          
        void OnViewportResize(uint32_t width, uint32_t height);   
        void OnMouseInput(float mouseX, float mouseY, bool mousePressed, Timestep ts);     

        void DuplicateEntity(Entity entity);

        Entity GetHoveredEntity();

        entt::registry& GetRegistry() { return m_Registry;}
        // Ref<Framebuffre>& GetBuffer() { return m_Framebuffer;}
        //temp
        Ref<Framebuffer> m_Framebuffer;
        PhysicsEngine m_Physics3D;
        bool ShowBoxes = false;
        bool ShowCams = true;
        bool ShowBoxesPlay = false;
    private:
        template<typename T>
        void  OnComponentAdded(Entity entity, T& component);
    private:
        void ReadPixelEntity(int& mouseX, int& mouseY, glm::vec2& viewportSize);
    private:
        entt::registry m_Registry;
        Ref<Texture2D> m_Screen;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        float m_MouseX, m_MouseY;
        friend class Entity;                
        friend class SceneSerializer;    
    };
}
