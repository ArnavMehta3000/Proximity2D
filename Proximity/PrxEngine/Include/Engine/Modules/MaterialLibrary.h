#pragma once
#include "Graphics/Material.h"

namespace Proximity::Modules
{
	class MaterialLibrary : public Utils::IService
	{
		using MaterialMap = std::unordered_map<std::string, std::shared_ptr<Graphics::Material>>;

	public:
		MaterialLibrary() = default;

		inline const Math::U64 Count()     const noexcept          { return m_materials.size(); }
		inline const MaterialMap& GetMap() const noexcept          { return m_materials; }
		inline void UpdateSelected(std::string_view shader)        { m_selectedMaterial = shader; OnMaterialSelected(shader); }
		inline const std::string& GetSelectedName() const noexcept { return m_selectedMaterial; }

		void InitProjectLib();
		void SerializeMaterials();

		// Add a material by reference
		void AddMaterial(const std::shared_ptr<Graphics::Material>& material);

		// Remove a material from the library by name
		void RemoveMaterial(const std::string& materialName);

		// Checks if a material exists om the library
		bool Exists(const std::string& materialName);

		// Retrives a material from the library
		std::shared_ptr<Graphics::Material> Get(const std::string& materialName);

		void Release();

	public:
		Utils::Action<std::string_view> OnMaterialSelected;

	private:
		std::string m_selectedMaterial;
		MaterialMap m_materials;
	};
}