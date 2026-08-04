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

# include <Siv3D/Mesh2D.hpp>
# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/Shape2D.hpp>
# include <Siv3D/FastMath.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		inline constexpr Float4 DefaultVertexColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		template <class UVGenerator>
		static void AssignVertices(Array<Vertex2D>& dst, const Array<Float2>& src, UVGenerator&& uvGenerator)
		{
			dst.resize(src.size());

			for (size_t i = 0; i < src.size(); ++i)
			{
				const Float2 pos = src[i];
				dst[i].set(pos, uvGenerator(pos), DefaultVertexColor);
			}
		}

		static void AssignVertices(Array<Vertex2D>& dst, const Array<Float2>& src)
		{
			dst.resize(src.size());

			for (size_t i = 0; i < src.size(); ++i)
			{
				dst[i].set(src[i], Float2{ 0.0f, 0.0f }, DefaultVertexColor);
			}
		}

		[[nodiscard]]
		static std::span<const TriangleIndex> GetTriangleSubset(const Array<TriangleIndex>& indices, const size_t startTriangle, const size_t triangleCount) noexcept
		{
			if ((triangleCount == 0)
				|| (indices.size() <= startTriangle))
			{
				return{};
			}

			const size_t availableTriangleCount = (indices.size() - startTriangle);
			const size_t actualTriangleCount = Min(triangleCount, availableTriangleCount);

			return{ (indices.data() + startTriangle), actualTriangleCount };
		}

		static void DrawTriangles(const Array<Vertex2D>& vertices, const std::span<const TriangleIndex> indices, const Optional<Vec2>& offset, const Texture* texture)
		{
			if (vertices.empty() || indices.empty())
			{
				return;
			}

			if (texture)
			{
				SIV3D_ENGINE(Renderer2D)->addMesh2D(*texture, vertices, indices, offset);
			}
			else
			{
				SIV3D_ENGINE(Renderer2D)->addMesh2D(vertices, indices, offset);
			}
		}

		template <class PositionGenerator>
		[[nodiscard]]
		static Mesh2D MakeGrid(const Size& divisions, PositionGenerator&& positionGenerator)
		{
			if ((divisions.x <= 0) || (divisions.y <= 0))
			{
				return{};
			}

			const size_t xDivisions = static_cast<size_t>(divisions.x);
			const size_t yDivisions = static_cast<size_t>(divisions.y);
			const size_t columns = (xDivisions + 1);
			const size_t rows = (yDivisions + 1);

			// columns * rows のオーバーフローを避けつつ、頂点数の上限を検証する
			if ((Mesh2D::MaxVertexCount / columns) < rows)
			{
				return{};
			}

			const size_t vertexCount = (columns * rows);
			const size_t triangleCount = (xDivisions * yDivisions * 2);
			Mesh2D mesh{ vertexCount, triangleCount };

			for (size_t y = 0; y <= yDivisions; ++y)
			{
				const double v = (static_cast<double>(y) / yDivisions);

				for (size_t x = 0; x <= xDivisions; ++x)
				{
					const double u = (static_cast<double>(x) / xDivisions);
					const size_t vertexIndex = (y * columns + x);

					mesh.vertices[vertexIndex].set(
						positionGenerator(u, v),
						Float2{ static_cast<float>(u), static_cast<float>(v) },
						DefaultVertexColor);
				}
			}

			size_t triangleIndex = 0;

			for (size_t y = 0; y < yDivisions; ++y)
			{
				for (size_t x = 0; x < xDivisions; ++x)
				{
					const size_t topLeft = (y * columns + x);
					const size_t topRight = (topLeft + 1);
					const size_t bottomLeft = (topLeft + columns);
					const size_t bottomRight = (bottomLeft + 1);

					mesh.indices[triangleIndex++] = TriangleIndex{
						static_cast<TriangleIndex::value_type>(topLeft),
						static_cast<TriangleIndex::value_type>(topRight),
						static_cast<TriangleIndex::value_type>(bottomRight)
					};

					mesh.indices[triangleIndex++] = TriangleIndex{
						static_cast<TriangleIndex::value_type>(topLeft),
						static_cast<TriangleIndex::value_type>(bottomRight),
						static_cast<TriangleIndex::value_type>(bottomLeft)
					};
				}
			}

			return mesh;
		}

		template <class PositionGenerator>
		[[nodiscard]]
		static Mesh2D MakeTriangleGrid(const Size& divisions, PositionGenerator&& positionGenerator)
		{
			if ((divisions.x <= 0) || (divisions.y <= 0))
			{
				return{};
			}

			const size_t xDivisions = static_cast<size_t>(divisions.x);
			const size_t yDivisions = static_cast<size_t>(divisions.y);
			const size_t rowCount = (yDivisions + 1);
			const size_t evenRowCount = ((rowCount + 1) / 2);
			const size_t oddRowCount = (rowCount / 2);
			const size_t evenRowVertexCount = (xDivisions + 1);
			const size_t oddRowVertexCount = (xDivisions + 2);

			if ((Mesh2D::MaxVertexCount / evenRowVertexCount) < evenRowCount)
			{
				return{};
			}

			const size_t evenVertexCount = (evenRowVertexCount * evenRowCount);

			if ((oddRowCount != 0)
				&& ((Mesh2D::MaxVertexCount - evenVertexCount) / oddRowVertexCount) < oddRowCount)
			{
				return{};
			}

			const size_t vertexCount = (evenVertexCount + oddRowVertexCount * oddRowCount);
			const size_t triangleCount = (yDivisions * (xDivisions * 2 + 1));
			Mesh2D mesh{ vertexCount, triangleCount };

			auto GetRowVertexCount = [xDivisions](const size_t y) noexcept
			{
				return (xDivisions + 1 + (y & 1));
			};

			auto GetRowOffset = [xDivisions](const size_t y) noexcept
			{
				const size_t evenRowsBefore = ((y + 1) / 2);
				const size_t oddRowsBefore = (y / 2);

				return (evenRowsBefore * (xDivisions + 1)
					+ oddRowsBefore * (xDivisions + 2));
			};

			auto GetUNumerator = [xDivisions](const size_t y, const size_t x) noexcept
			{
				if ((y & 1) == 0)
				{
					return (x * 2);
				}

				if (x == 0)
				{
					return size_t{ 0 };
				}

				if (x == (xDivisions + 1))
				{
					return (xDivisions * 2);
				}

				return (x * 2 - 1);
			};

			for (size_t y = 0; y <= yDivisions; ++y)
			{
				const double v = (static_cast<double>(y) / yDivisions);
				const size_t currentRowVertexCount = GetRowVertexCount(y);
				const size_t rowOffset = GetRowOffset(y);

				for (size_t x = 0; x < currentRowVertexCount; ++x)
				{
					const double u = (static_cast<double>(GetUNumerator(y, x)) / (xDivisions * 2));

					mesh.vertices[rowOffset + x].set(
						positionGenerator(u, v),
						Float2{ static_cast<float>(u), static_cast<float>(v) },
						DefaultVertexColor);
				}
			}

			size_t triangleIndex = 0;

			for (size_t y = 0; y < yDivisions; ++y)
			{
				const size_t topOffset = GetRowOffset(y);
				const size_t bottomOffset = GetRowOffset(y + 1);
				const size_t topCount = GetRowVertexCount(y);
				const size_t bottomCount = GetRowVertexCount(y + 1);
				size_t top = 0;
				size_t bottom = 0;

				while (((top + 1) < topCount) || ((bottom + 1) < bottomCount))
				{
					const bool canAdvanceTop = ((top + 1) < topCount);
					const bool canAdvanceBottom = ((bottom + 1) < bottomCount);

					bool advanceTop;

					if (not canAdvanceBottom)
					{
						advanceTop = true;
					}
					else if (not canAdvanceTop)
					{
						advanceTop = false;
					}
					else
					{
						const size_t topNextU = GetUNumerator(y, top + 1);
						const size_t bottomNextU = GetUNumerator(y + 1, bottom + 1);

						if (topNextU != bottomNextU)
						{
							advanceTop = (topNextU < bottomNextU);
						}
						else
						{
							// 右端で両方の次の頂点が一致する場合は、
							// 頂点数の少ない側を先に進めて左右対称に閉じる
							advanceTop = (topCount < bottomCount);
						}
					}

					if (advanceTop)
					{
						mesh.indices[triangleIndex++] = TriangleIndex{
							static_cast<TriangleIndex::value_type>(topOffset + top),
							static_cast<TriangleIndex::value_type>(topOffset + top + 1),
							static_cast<TriangleIndex::value_type>(bottomOffset + bottom)
						};

						++top;
					}
					else
					{
						mesh.indices[triangleIndex++] = TriangleIndex{
							static_cast<TriangleIndex::value_type>(topOffset + top),
							static_cast<TriangleIndex::value_type>(bottomOffset + bottom + 1),
							static_cast<TriangleIndex::value_type>(bottomOffset + bottom)
						};

						++bottom;
					}
				}
			}

			return mesh;
		}

		template <class PositionGenerator>
		[[nodiscard]]
		Mesh2D MakeRadialGrid(const Size& divisions, PositionGenerator&& positionGenerator)
		{
			if ((divisions.x < 3) || (divisions.y <= 0))
			{
				return{};
			}

			const size_t angularDivisions = static_cast<size_t>(divisions.x);
			const size_t radialDivisions = static_cast<size_t>(divisions.y);

			// 1 + angularDivisions * radialDivisions のオーバーフローを避けつつ、
			// 頂点数の上限を検証する
			if (((Mesh2D::MaxVertexCount - 1) / angularDivisions) < radialDivisions)
			{
				return{};
			}

			const size_t vertexCount = (1 + angularDivisions * radialDivisions);
			const size_t triangleCount = (angularDivisions * (radialDivisions * 2 - 1));
			Mesh2D mesh{ vertexCount, triangleCount };

			// 中心頂点
			mesh.vertices[0].set(
				positionGenerator(Float2{ 0.0f, 0.0f }),
				Float2{ 0.5f, 0.5f },
				DefaultVertexColor);

			constexpr float StartAngle = (-Math::HalfPiF);

			// 同心円状に頂点を生成
			// 周方向は 12 時方向から時計回り
			for (size_t radialIndex = 1; radialIndex <= radialDivisions; ++radialIndex)
			{
				const float radius = (static_cast<float>(radialIndex) / radialDivisions);
				const size_t ringOffset = (1 + (radialIndex - 1) * angularDivisions);

				for (size_t angularIndex = 0; angularIndex < angularDivisions; ++angularIndex)
				{
					const float angle = (StartAngle + Math::TwoPiF * angularIndex / angularDivisions);
					const auto sinCos = FastMath::SinCos(angle);
					const float x = (radius * sinCos.second);
					const float y = (radius * sinCos.first);
					const Float2 normalizedPos{ x, y };

					mesh.vertices[ringOffset + angularIndex].set(
						positionGenerator(normalizedPos),
						Float2{ (0.5f + x * 0.5f), (0.5f + y * 0.5f) },
						DefaultVertexColor);
				}
			}

			size_t triangleIndex = 0;

			// 中心から最初のリングへの三角形
			{
				const size_t ringOffset = 1;

				for (size_t angularIndex = 0; angularIndex < angularDivisions; ++angularIndex)
				{
					const size_t nextAngularIndex = ((angularIndex + 1) % angularDivisions);

					mesh.indices[triangleIndex++] = TriangleIndex{
						0,
						static_cast<TriangleIndex::value_type>(ringOffset + angularIndex),
						static_cast<TriangleIndex::value_type>(ringOffset + nextAngularIndex)
					};
				}
			}

			// リング間を接続
			for (size_t radialIndex = 1; radialIndex < radialDivisions; ++radialIndex)
			{
				const size_t innerRingOffset = (1 + (radialIndex - 1) * angularDivisions);
				const size_t outerRingOffset = (1 + radialIndex * angularDivisions);

				for (size_t angularIndex = 0; angularIndex < angularDivisions; ++angularIndex)
				{
					const size_t nextAngularIndex = ((angularIndex + 1) % angularDivisions);

					const size_t innerCurrent = (innerRingOffset + angularIndex);
					const size_t innerNext = (innerRingOffset + nextAngularIndex);
					const size_t outerCurrent = (outerRingOffset + angularIndex);
					const size_t outerNext = (outerRingOffset + nextAngularIndex);

					mesh.indices[triangleIndex++] = TriangleIndex{
						static_cast<TriangleIndex::value_type>(innerCurrent),
						static_cast<TriangleIndex::value_type>(outerCurrent),
						static_cast<TriangleIndex::value_type>(outerNext)
					};

					mesh.indices[triangleIndex++] = TriangleIndex{
						static_cast<TriangleIndex::value_type>(innerCurrent),
						static_cast<TriangleIndex::value_type>(outerNext),
						static_cast<TriangleIndex::value_type>(innerNext)
					};
				}
			}

			return mesh;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Mesh2D::Mesh2D(const Polygon& polygon)
	{
		const auto& srcVertices = polygon.vertices();

		if (MaxVertexCount < srcVertices.size())
		{
			return;
		}

		AssignVertices(vertices, srcVertices);
		indices = polygon.indices();
	}

	Mesh2D::Mesh2D(const Polygon& polygon, const RectF& mappingRect)
	{
		const auto& srcVertices = polygon.vertices();

		if ((MaxVertexCount < srcVertices.size())
			|| (mappingRect.w == 0.0)
			|| (mappingRect.h == 0.0))
		{
			return;
		}

		const Float2 uvOrigin{
			static_cast<float>(mappingRect.x),
			static_cast<float>(mappingRect.y)
		};
		const Float2 uvScale{
			static_cast<float>(1.0 / mappingRect.w),
			static_cast<float>(1.0 / mappingRect.h)
		};

		AssignVertices(vertices, srcVertices,
			[uvOrigin, uvScale](const Float2 pos) noexcept
			{
				return ((pos - uvOrigin) * uvScale);
			});

		indices = polygon.indices();
	}

	Mesh2D::Mesh2D(const Polygon& polygon, const Mat3x2& uvTransform)
	{
		const auto& srcVertices = polygon.vertices();

		if (MaxVertexCount < srcVertices.size())
		{
			return;
		}

		AssignVertices(vertices, srcVertices,
			[&uvTransform](const Float2 pos) noexcept
			{
				return uvTransform.transformPoint(pos);
			});

		indices = polygon.indices();
	}

	Mesh2D::Mesh2D(const Shape2D& shape2D)
	{
		const auto& srcVertices = shape2D.vertices();

		if (MaxVertexCount < srcVertices.size())
		{
			return;
		}

		AssignVertices(vertices, srcVertices);
		indices = shape2D.indices();
	}

	Mesh2D::Mesh2D(const Shape2D& shape2D, const RectF& mappingRect)
	{
		const auto& srcVertices = shape2D.vertices();

		if ((MaxVertexCount < srcVertices.size())
			|| (mappingRect.w == 0.0)
			|| (mappingRect.h == 0.0))
		{
			return;
		}

		const Float2 uvOrigin{
			static_cast<float>(mappingRect.x),
			static_cast<float>(mappingRect.y)
		};
		const Float2 uvScale{
			static_cast<float>(1.0 / mappingRect.w),
			static_cast<float>(1.0 / mappingRect.h)
		};

		AssignVertices(vertices, srcVertices,
			[uvOrigin, uvScale](const Float2 pos) noexcept
			{
				return ((pos - uvOrigin) * uvScale);
			});

		indices = shape2D.indices();
	}

	Mesh2D::Mesh2D(const Shape2D& shape2D, const Mat3x2& uvTransform)
	{
		const auto& srcVertices = shape2D.vertices();

		if (MaxVertexCount < srcVertices.size())
		{
			return;
		}

		AssignVertices(vertices, srcVertices,
			[&uvTransform](const Float2 pos) noexcept
			{
				return uvTransform.transformPoint(pos);
			});

		indices = shape2D.indices();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isValid
	//
	////////////////////////////////////////////////////////////////

	bool Mesh2D::isValid() const noexcept
	{
		if (MaxVertexCount < vertices.size())
		{
			return false;
		}

		const size_t vertexCount = vertices.size();

		for (const auto& triangle : indices)
		{
			if ((vertexCount <= triangle.i0)
				|| (vertexCount <= triangle.i1)
				|| (vertexCount <= triangle.i2))
			{
				return false;
			}
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	vertexCount
	//
	////////////////////////////////////////////////////////////////

	size_t Mesh2D::vertexCount() const noexcept
	{
		return vertices.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	triangleCount
	//
	////////////////////////////////////////////////////////////////

	size_t Mesh2D::triangleCount() const noexcept
	{
		return indices.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	bool Mesh2D::append(const Mesh2D& mesh)
	{
		if (not mesh.isValid())
		{
			return false;
		}

		const size_t vertexOffset = vertices.size();
		const size_t sourceVertexCount = mesh.vertices.size();

		if ((MaxVertexCount < vertexOffset)
			|| ((MaxVertexCount - vertexOffset) < sourceVertexCount))
		{
			return false;
		}

		const size_t triangleOffset = indices.size();
		const size_t sourceTriangleCount = mesh.indices.size();

		if (this == &mesh)
		{
			vertices.resize(vertexOffset + sourceVertexCount);

			for (size_t i = 0; i < sourceVertexCount; ++i)
			{
				vertices[vertexOffset + i] = vertices[i];
			}

			indices.resize(triangleOffset + sourceTriangleCount);

			for (size_t i = 0; i < sourceTriangleCount; ++i)
			{
				indices[triangleOffset + i] = indices[i];
			}
		}
		else
		{
			vertices.insert(
				vertices.end(),
				mesh.vertices.begin(),
				mesh.vertices.end());

			indices.insert(
				indices.end(),
				mesh.indices.begin(),
				mesh.indices.end());
		}

		for (size_t i = triangleOffset; i < indices.size(); ++i)
		{
			auto& triangle = indices[i];

			triangle.i0 = static_cast<TriangleIndex::value_type>(vertexOffset + triangle.i0);
			triangle.i1 = static_cast<TriangleIndex::value_type>(vertexOffset + triangle.i1);
			triangle.i2 = static_cast<TriangleIndex::value_type>(vertexOffset + triangle.i2);
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setColor
	//
	////////////////////////////////////////////////////////////////

	Mesh2D& Mesh2D::setColor(const ColorF& color) noexcept
	{
		const Float4 vertexColor = color.toFloat4();

		for (auto& vertex : vertices)
		{
			vertex.color = vertexColor;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformUV
	//
	////////////////////////////////////////////////////////////////

	Mesh2D& Mesh2D::transformUV(const Mat3x2& transform) noexcept
	{
		for (auto& vertex : vertices)
		{
			vertex.tex = transform.transformPoint(vertex.tex);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	void Mesh2D::draw() const
	{
		DrawTriangles(vertices, indices, none, nullptr);
	}

	void Mesh2D::draw(const Vec2& offset) const
	{
		DrawTriangles(vertices, indices, offset, nullptr);
	}

	void Mesh2D::draw(const Texture& texture) const
	{
		DrawTriangles(vertices, indices, none, &texture);
	}

	void Mesh2D::draw(const Vec2& offset, const Texture& texture) const
	{
		DrawTriangles(vertices, indices, offset, &texture);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawSubset
	//
	////////////////////////////////////////////////////////////////

	void Mesh2D::drawSubset(const size_t startTriangle, const size_t triangleCount) const
	{
		DrawTriangles(vertices, GetTriangleSubset(indices, startTriangle, triangleCount), none, nullptr);
	}

	void Mesh2D::drawSubset(const size_t startTriangle, const size_t triangleCount, const Vec2& offset) const
	{
		DrawTriangles(vertices, GetTriangleSubset(indices, startTriangle, triangleCount), offset, nullptr);
	}

	void Mesh2D::drawSubset(const size_t startTriangle, const size_t triangleCount, const Texture& texture) const
	{
		DrawTriangles(vertices, GetTriangleSubset(indices, startTriangle, triangleCount), none, &texture);
	}

	void Mesh2D::drawSubset(const size_t startTriangle, const size_t triangleCount, const Vec2& offset, const Texture& texture) const
	{
		DrawTriangles(vertices, GetTriangleSubset(indices, startTriangle, triangleCount), offset, &texture);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Grid
	//
	////////////////////////////////////////////////////////////////

	Mesh2D Mesh2D::Grid(const SizeF& size, const Size& divisions)
	{
		return Grid(RectF{ size }, divisions);
	}

	Mesh2D Mesh2D::Grid(const RectF& rect, const Size& divisions)
	{
		return MakeGrid(divisions,
			[&rect](const double u, const double v) noexcept
			{
				return Float2{
					static_cast<float>(rect.x + rect.w * u),
					static_cast<float>(rect.y + rect.h * v)
				};
			});
	}

	Mesh2D Mesh2D::Grid(const Quad& quad, const Size& divisions)
	{
		return MakeGrid(divisions,
			[&quad](const double u, const double v) noexcept
			{
				const Vec2 top = (quad.p0 + (quad.p1 - quad.p0) * u);
				const Vec2 bottom = (quad.p3 + (quad.p2 - quad.p3) * u);
				const Vec2 pos = (top + (bottom - top) * v);

				return Float2{
					static_cast<float>(pos.x),
					static_cast<float>(pos.y)
				};
			});
	}

	////////////////////////////////////////////////////////////////
	//
	//	TriangleGrid
	//
	////////////////////////////////////////////////////////////////

	Mesh2D Mesh2D::TriangleGrid(const SizeF& size, const Size& divisions)
	{
		return TriangleGrid(RectF{ size }, divisions);
	}

	Mesh2D Mesh2D::TriangleGrid(const RectF& rect, const Size& divisions)
	{
		return MakeTriangleGrid(divisions,
			[&rect](const double u, const double v) noexcept
			{
				return Float2{
					static_cast<float>(rect.x + rect.w * u),
					static_cast<float>(rect.y + rect.h * v)
				};
			});
	}

	Mesh2D Mesh2D::TriangleGrid(const Quad& quad, const Size& divisions)
	{
		return MakeTriangleGrid(divisions,
			[&quad](const double u, const double v) noexcept
			{
				const Vec2 top = (quad.p0 + (quad.p1 - quad.p0) * u);
				const Vec2 bottom = (quad.p3 + (quad.p2 - quad.p3) * u);
				const Vec2 pos = (top + (bottom - top) * v);

				return Float2{
					static_cast<float>(pos.x),
					static_cast<float>(pos.y)
				};
			});
	}

	////////////////////////////////////////////////////////////////
	//
	//	RadialGrid
	//
	////////////////////////////////////////////////////////////////

	Mesh2D Mesh2D::RadialGrid(const Circle& circle, const Size& divisions)
	{
		return MakeRadialGrid(divisions,
			[&circle](const Float2 normalizedPos) noexcept
			{
				return Float2{
					static_cast<float>(circle.center.x + circle.r * normalizedPos.x),
					static_cast<float>(circle.center.y + circle.r * normalizedPos.y)
				};
			});
	}
}
