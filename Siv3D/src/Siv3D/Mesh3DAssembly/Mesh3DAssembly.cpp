//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Mesh3DAssembly.hpp>
# include "../Mesh3D/Mesh3DCommon.hpp"
# include <algorithm>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		Err<Mesh3DError> AssemblyFailed(const Mesh3DErrorCode code, const StringView message)
		{
			LOG_FAIL(message);
			return Err{ Mesh3DError{ code, String{ message } } };
		}

		template <class ID>
		[[nodiscard]]
		constexpr size_t Index(const ID id) noexcept
		{
			return static_cast<size_t>(id);
		}
	}

	Optional<Mesh3DAssembly::PartID> Mesh3DAssembly::ClonedSubtree::find(const PartID source) const noexcept
	{
		const auto it = std::lower_bound(parts.begin(), parts.end(), source,
			[](const auto& pair, const PartID id) { return (pair.first < id); });
		return ((it != parts.end() && it->first == source) ? Optional<PartID>{ it->second } : none);
	}

	void Mesh3DAssembly::reserve(const size_t meshCapacity, const size_t materialCapacity, const size_t partCapacity)
	{
		m_meshes.reserve(meshCapacity);
		m_materials.reserve(materialCapacity);
		m_parts.reserve(partCapacity);
	}

	void Mesh3DAssembly::clear() noexcept
	{
		m_meshes.clear();
		m_materials.clear();
		m_parts.clear();
	}

	Result<Mesh3DAssembly::MeshID, Mesh3DError> Mesh3DAssembly::addMesh(Mesh3D mesh)
	{
		if (mesh.isEmpty() || (not mesh.validate()))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3DAssembly::addMesh(): mesh is empty or has invalid indices");
		}
		const MeshID id{ m_meshes.size() };
		m_meshes.push_back(std::move(mesh));
		return id;
	}

	Result<void, Mesh3DError> Mesh3DAssembly::setMesh(const MeshID id, Mesh3D mesh)
	{
		if (m_meshes.size() <= Index(id))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly::setMesh(): unknown mesh ID");
		}
		if (mesh.isEmpty() || (not mesh.validate()))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3DAssembly::setMesh(): mesh is empty or has invalid indices");
		}
		m_meshes[Index(id)] = std::move(mesh);
		return{};
	}

	Mesh3DAssembly::MaterialID Mesh3DAssembly::addMaterial(Material material)
	{
		const MaterialID id{ m_materials.size() };
		m_materials.push_back(std::move(material));
		return id;
	}

	Result<void, Mesh3DError> Mesh3DAssembly::setMaterial(const MaterialID id, Material material)
	{
		if (m_materials.size() <= Index(id))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly::setMaterial(): unknown material ID");
		}
		m_materials[Index(id)] = std::move(material);
		return{};
	}

	Result<void, Mesh3DError> Mesh3DAssembly::checkPart(const Part& part, const size_t parentLimit) const
	{
		if (part.mesh && (m_meshes.size() <= Index(*part.mesh)))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly: unknown mesh ID in part");
		}
		if (part.material && (m_materials.size() <= Index(*part.material)))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly: unknown material ID in part");
		}
		if (part.parent && (parentLimit <= Index(*part.parent)))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly: parent must precede child");
		}
		return{};
	}

	Result<Mesh3DAssembly::PartID, Mesh3DError> Mesh3DAssembly::addPart(Part part)
	{
		if (const auto result = checkPart(part, m_parts.size()); not result)
		{
			return Err{ result.error() };
		}
		const PartID id{ m_parts.size() };
		m_parts.push_back(std::move(part));
		return id;
	}

	Result<void, Mesh3DError> Mesh3DAssembly::setPart(const PartID id, Part part)
	{
		if (m_parts.size() <= Index(id))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly::setPart(): unknown part ID");
		}
		if (const auto result = checkPart(part, Index(id)); not result)
		{
			return result;
		}
		m_parts[Index(id)] = std::move(part);
		return{};
	}

	Result<void, Mesh3DError> Mesh3DAssembly::setPlacement(const PartID id, const Mesh3DPlacement& placement)
	{
		if (m_parts.size() <= Index(id))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly::setPlacement(): unknown part ID");
		}
		m_parts[Index(id)].placement = placement;
		return{};
	}

	Result<Mesh3DAssembly::ClonedSubtree, Mesh3DError> Mesh3DAssembly::cloneSubtree(
		const PartID root, const Mesh3DPlacement& placement, const Optional<PartID> parent)
	{
		const size_t originalSize = m_parts.size();
		if ((originalSize <= Index(root)) || (parent && (originalSize <= Index(*parent))))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly::cloneSubtree(): unknown root or parent ID");
		}

		ClonedSubtree result{ .root = PartID{ originalSize } };
		const size_t available = (m_parts.max_size() - originalSize);
		for (size_t i = Index(root); i < originalSize; ++i)
		{
			const auto sourceParent = m_parts[i].parent;
			if ((i != Index(root)) && ((not sourceParent) || (not result.find(*sourceParent))))
			{
				continue;
			}
			if (result.parts.size() == available)
			{
				return AssemblyFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3DAssembly::cloneSubtree(): part count exceeds storage limit");
			}
			result.parts.emplace_back(PartID{ i }, PartID{ originalSize + result.parts.size() });
		}

		// Stage strings and transforms before mutating m_parts. placement may
		// alias an existing Part, and the final append may invalidate that reference.
		Array<Part> copies;
		copies.reserve(result.parts.size());
		for (const auto& [source, destination] : result.parts)
		{
			Part copy = m_parts[Index(source)];
			if (source == root)
			{
				copy.parent = parent;
				copy.placement = placement;
			}
			else
			{
				copy.parent = result.find(*copy.parent);
			}
			copies.push_back(std::move(copy));
		}
		static_assert(std::is_nothrow_move_constructible_v<Part>);
		static_assert(std::is_nothrow_move_assignable_v<Part>);
		// Let the destination retain its normal geometric capacity growth when
		// repeatedly cloning, rather than reserving an exact size on each call.
		m_parts.append(std::move(copies));
		return result;
	}

	const Mesh3D* Mesh3DAssembly::getMesh(const MeshID id) const noexcept
	{
		return ((Index(id) < m_meshes.size()) ? &m_meshes[Index(id)] : nullptr);
	}

	const Material* Mesh3DAssembly::getMaterial(const MaterialID id) const noexcept
	{
		return ((Index(id) < m_materials.size()) ? &m_materials[Index(id)] : nullptr);
	}

	const Mesh3DAssembly::Part* Mesh3DAssembly::getPart(const PartID id) const noexcept
	{
		return ((Index(id) < m_parts.size()) ? &m_parts[Index(id)] : nullptr);
	}

	size_t Mesh3DAssembly::meshCount() const noexcept
	{
		return m_meshes.size();
	}

	size_t Mesh3DAssembly::materialCount() const noexcept
	{
		return m_materials.size();
	}

	size_t Mesh3DAssembly::partCount() const noexcept
	{
		return m_parts.size();
	}

	Result<Mat4x4, Mesh3DError> Mesh3DAssembly::computeWorldTransform(const PartID id) const
	{
		if (m_parts.size() <= Index(id))
		{
			return AssemblyFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3DAssembly::computeWorldTransform(): unknown part ID");
		}
		Mat4x4 result = m_parts[Index(id)].placement.getTransform();
		Optional<PartID> parent = m_parts[Index(id)].parent;
		while (parent)
		{
			const Part& part = m_parts[Index(*parent)];
			result *= part.placement.getTransform();
			parent = part.parent;
		}
		return result;
	}

	Result<void, Mesh3DError> Mesh3DAssembly::bake(BakedMesh& destination, const Mesh3DBakeOptions& options) const
	{
		size_t vertexCount = 0;
		size_t triangleCount = 0;
		const size_t vertexLimit = std::min(options.maxVertexCount, Mesh3D::MaxVertexCount);
		for (const Part& part : m_parts)
		{
			if (not part.mesh)
			{
				continue;
			}
			const Mesh3D& mesh = m_meshes[Index(*part.mesh)];
			if ((not Mesh3DDetail::CheckedAdd(vertexCount, mesh.vertexCount(), vertexCount))
				|| (not Mesh3DDetail::CheckedAdd(triangleCount, mesh.triangleCount(), triangleCount))
				|| (vertexLimit < vertexCount) || (options.maxTriangleCount < triangleCount))
			{
				return AssemblyFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3DAssembly::bake(): output exceeds mesh size budget");
			}
		}

		// All fallible size checks precede writes. Output metadata doubles as the
		// transform workspace, so repeated baking needs no temporary arrays.
		destination.mesh.vertices.resize(vertexCount);
		destination.mesh.indices.resize(triangleCount);
		destination.parts.resize(m_parts.size());
		destination.materials = m_materials;
		size_t vertexOffset = 0;
		size_t triangleOffset = 0;
		for (size_t i = 0; i < m_parts.size(); ++i)
		{
			const Part& part = m_parts[i];
			BakedPart& baked = destination.parts[i];
			baked.id = PartID{ i };
			baked.name = part.name;
			baked.parent = part.parent;
			baked.material = part.material;
			baked.worldTransform = part.placement.getTransform();
			if (part.parent)
			{
				baked.worldTransform *= destination.parts[Index(*part.parent)].worldTransform;
			}
			baked.range = Mesh3DRange{ vertexOffset, 0, triangleOffset, 0 };
			if (not part.mesh)
			{
				continue;
			}

			const Mesh3D& mesh = m_meshes[Index(*part.mesh)];
			baked.range.vertexCount = mesh.vertexCount();
			baked.range.triangleCount = mesh.triangleCount();
			Vertex3D* const vertices = (destination.mesh.vertices.data() + vertexOffset);
			std::copy(mesh.vertices.begin(), mesh.vertices.end(), vertices);
			Mesh3DDetail::TransformVertexRange(std::span<Vertex3D>{ vertices, mesh.vertexCount() }, baked.worldTransform);

			const bool mirrored = (baked.worldTransform.determinant() < 0.0f);
			const uint32 base = static_cast<uint32>(vertexOffset);
			for (const TriangleIndex32& triangle : mesh.indices)
			{
				TriangleIndex32& output = destination.mesh.indices[triangleOffset++];
				output = TriangleIndex32{ (base + triangle.i0), (base + triangle.i1), (base + triangle.i2) };
				if (mirrored)
				{
					std::swap(output.i1, output.i2);
				}
			}
			vertexOffset += mesh.vertexCount();
		}
		return{};
	}

	Result<Mesh3DAssembly::BakedMesh, Mesh3DError> Mesh3DAssembly::bake(const Mesh3DBakeOptions& options) const
	{
		BakedMesh result;
		if (auto status = bake(result, options); not status)
		{
			return Err{ std::move(status.error()) };
		}
		return result;
	}
}
