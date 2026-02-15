// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.

// This file was modified by Oracle on 2017-2022.
// Modifications copyright (c) 2019-2022, Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <map>
#include <deque>
#include <vector>
#include <algorithm>
#include <boost/range/size.hpp>
#include <boost/range/value_type.hpp>
#include <boost/geometry/algorithms/detail/overlay/get_turns.hpp>
#include <boost/geometry/algorithms/detail/overlay/self_turn_points.hpp>
#include <boost/geometry/algorithms/detail/overlay/overlay.hpp>
#include <boost/geometry/algorithms/detail/overlay/turn_info.hpp>
#include <boost/geometry/algorithms/detail/overlay/traversal_info.hpp>
#include <boost/geometry/algorithms/detail/overlay/enrich_intersection_points.hpp>
#include <boost/geometry/algorithms/detail/overlay/add_rings.hpp>
#include <boost/geometry/algorithms/detail/overlay/assign_parents.hpp>
#include <boost/geometry/algorithms/detail/overlay/ring_properties.hpp>
#include <boost/geometry/algorithms/detail/overlay/select_rings.hpp>
#include <boost/geometry/algorithms/detail/overlay/traversal_ring_creator.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/sym_difference.hpp>
#include <boost/geometry/algorithms/union.hpp>
#include <boost/geometry/algorithms/reverse.hpp>
#include <boost/geometry/policies/robustness/segment_ratio_type.hpp>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/exterior_ring.hpp>

namespace boost
{
	namespace geometry
	{
		namespace detail
		{
			namespace inserter
			{
				template<typename Tag1, typename Tag2>
				struct insert_geometry {};

				template<>
				struct insert_geometry<ring_tag, polygon_tag>
				{
					template<typename Ring, typename Collection>
					static void apply(Ring const& ring, Collection& collection)
					{
						collection.resize(collection.size() + 1);
						geometry::exterior_ring(collection.back()) = ring;
					}
				};

				template<>
				struct insert_geometry<polygon_tag, polygon_tag>
				{
					template<typename Geometry, typename Collection>
					static void apply(Geometry const& geometry, Collection& collection)
					{
						collection.push_back(geometry);
					}
				};

				template<typename Geometry, typename Collection>
				void insert(Geometry const& geometry, Collection& collection)
				{
					insert_geometry<typename geometry::tag<Geometry>::type, typename geometry::tag<typename boost::range_value<Collection>::type>::type>::apply(geometry, collection);
				}
			}

			namespace dissolver
			{
				class plusmin_policy
				{
					template <typename Geometry1, typename Geometry2, typename RescalePolicy, typename OutputCollection, typename Strategy>
					static bool check_negative(Geometry1 a, Geometry2 b, RescalePolicy const& rescale_policy, OutputCollection& output_collection, Strategy const& strategy)
					{
						// Precondition: a = positive, b = negative

						// 1: make b positive to get proper intersection
						geometry::reverse(b);
						{
							// 2: Check if there is overlap
							OutputCollection difference;
							geometry::intersection(a, b, difference, strategy);
							if (difference.size() <= 0)
							{
								return false;
							}
						}

						// There is overlap and we want to remove it, by subtracting it from b

						//negative = true;

						typedef typename geometry::point_type<Geometry2>::type point_type;

						typedef overlay::turn_info<point_type, typename segment_ratio_type<point_type, RescalePolicy>::type> turn_info;
						std::deque<turn_info> turns;

						// Get (and stop on) any intersection
						detail::disjoint::disjoint_interrupt_policy policy;
						geometry::get_turns<false, false, overlay::assign_null_policy>(a, b, strategy, rescale_policy, turns, policy);

						if (!policy.has_intersections)
						{
							// There is overlap but no intersections -> b is inside a.
							// So keep A and keep B, do not change anything
							return false;
						}

						// There are intersections.
						// 3: make a negative
						geometry::reverse(a); // now negative

						// This will calculate B minus A, result is then positive
						OutputCollection difference;
						geometry::intersection(a, b, difference, strategy);

						// Add original a to output (NOT necessary! TODO avoid this)
						{
							geometry::reverse(a); // positive again
							detail::inserter::insert(a, output_collection);
						}

						// Make negative output negative again
						typedef typename boost::range_iterator<OutputCollection>::type iterator_type;
						for (iterator_type it = boost::begin(difference); it != boost::end(difference); ++it)
						{
							geometry::reverse(*it);
							detail::inserter::insert(*it, output_collection);
						}
						return true;
					}

					template <typename Geometry1, typename Geometry2, typename AreaType, typename RescalePolicy, typename OutputCollection, typename Strategy>
					static bool check(Geometry1 const& a, Geometry2 const& b,
							AreaType const& area_a, AreaType const& area_b,
							RescalePolicy const& rescale_policy,
							OutputCollection& output_collection,
							Strategy const& strategy)
					{
						AreaType const zero = AreaType();
						if (area_a > zero && area_b > zero)
						{
							geometry::union_(a, b, output_collection, strategy);
							return true;
						}
						else if (area_a > zero && area_b < zero)
						{
							return check_negative(a, b, rescale_policy, output_collection, strategy);
						}
						else if (area_a < zero && area_b > zero)
						{
							return check_negative(b, a, rescale_policy, output_collection, strategy);
						}
						return false;
					}

				public:

					template <typename Geometry1, typename Geometry2, typename RescalePolicy, typename OutputCollection, typename Strategy>
					static bool apply(Geometry1 const& a, Geometry2 const& b, RescalePolicy const& rescale_policy, OutputCollection& output_collection, Strategy const& strategy)
					{
						return check(a, b, geometry::area(a), geometry::area(b), rescale_policy, output_collection, strategy);
					}
				};

				template <typename CombinePolicy>
				struct dissolver_generic
				{
					// Small structure to access elements by index;
					// this avoids copying or accessing elements by address (pointer)
					template <typename Box>
					struct dissolve_helper
					{
						int source; // 0,1
						int index; // index in the original array
						bool dissolved;
						Box box;

						dissolve_helper() {}

						dissolve_helper(int i, Box b, int s)
							: source(s)
							, index(i)
							, dissolved(false)
							, box(b) {}
					};

					struct get_geometry
					{
						template <typename Range>
						static typename boost::range_value<Range>::type const& apply(Range const& range, int index)
						{
							return range[index];
						}
					};

					template <typename Vector, typename HelperVector, typename Strategy>
					static void init_helper(Vector const& v, HelperVector& helper, Strategy const& strategy, int index = 0, int source = 0)
					{
						typedef typename boost::range_value<Vector>::type value_type;
						typedef typename geometry::point_type<value_type>::type point_type;
						typedef model::box<point_type> box_type;

						for (auto it = boost::begin(v); it != boost::end(v); ++it, ++index)
						{
							helper.push_back(dissolve_helper<box_type>(index, geometry::return_envelope<box_type>(*it, strategy), source));
						}
					}

					template <typename Element, typename Geometry1, typename Geometry2, typename RescalePolicy, typename OutputCollection, typename Strategy>
					static bool call_policy(
							Element const&, Element const&,
							// Both geometry1 and geometry2 are copied,
							// because they are elements of the output collection which is changed,
							// which might change the collection itself and the address/contents of geometry1/geometry2
							Geometry1 geometry1,
							Geometry2 geometry2,
							RescalePolicy const& rescale_policy,
							OutputCollection& output_collection,
							Strategy const& strategy)
					{
						if (!geometry::disjoint(geometry1, geometry2, strategy))
						{
							return CombinePolicy::apply(geometry1, geometry2, rescale_policy, output_collection, strategy);
						}

						return false;
					}

					template <typename HelperVector, typename IndexVector, typename InputRange, typename RescalePolicy, typename OutputCollection, typename Strategy>
					static bool divide_and_conquer(HelperVector& helper_vector
							, IndexVector& index_vector
							, InputRange const& input_range
							, RescalePolicy const& rescale_policy
							, OutputCollection& output_collection
							, Strategy const& strategy
							, bool& changed)
					{
						typedef typename boost::range_value<HelperVector>::type helper_type;
						typedef typename boost::range_iterator<IndexVector const>::type iterator_type;

						// There are less then 16 elements, handle them quadraticly

						std::size_t n = boost::size(output_collection);

						for (iterator_type it1 = boost::begin(index_vector); it1 != boost::end(index_vector); ++it1)
						{
							helper_type& element1 = helper_vector[*it1];

							bool unioned = false;
							for (iterator_type it2 = boost::begin(index_vector); !unioned && it2 != it1; ++it2)
							{
								helper_type& element2 = helper_vector[*it2];

								// If they are NOT disjoint, union them
								if (!element1.dissolved
									&& !element2.dissolved
									&& !geometry::disjoint(element1.box, element2.box))
								{
									// Runtime type check here...
									if ((element1.source == 0 && element2.source == 0
										&& call_policy
										(
											element1, element2,
											get_geometry::apply(input_range, element1.index),
											get_geometry::apply(input_range, element2.index),
											rescale_policy,
											output_collection,
											strategy
										)
										)
										|| (element1.source == 0 && element2.source == 1
											&& call_policy
											(
												element1, element2,
												get_geometry::apply(input_range, element1.index),
												get_geometry::apply(output_collection, element2.index),
												rescale_policy,
												output_collection,
												strategy
											)
											)
										|| (element1.source == 1 && element2.source == 0
											&& call_policy
											(
												element1, element2,
												get_geometry::apply(output_collection, element1.index),
												get_geometry::apply(input_range, element2.index),
												rescale_policy,
												output_collection,
												strategy
											)
											)
										|| (element1.source == 1 && element2.source == 1
											&& call_policy
											(
												element1, element2,
												get_geometry::apply(output_collection, element1.index),
												get_geometry::apply(output_collection, element2.index),
												rescale_policy,
												output_collection,
												strategy
											)
											)
										)
									{
										changed = true;
										element1.dissolved = true;
										element2.dissolved = true;

										unioned = true;
									}
								}
							}
						}

						// Append new records in output collection to helper class
						init_helper(std::make_pair(boost::begin(output_collection) + n, boost::end(output_collection)), helper_vector, strategy, static_cast<int>(n), 1);

						return changed;
					}

					template <typename T>
					static bool helper_dissolved(T const& t)
					{
						return t.dissolved;
					}

					template <typename InputRange, typename RescalePolicy, typename OutputCollection, typename Strategy>
					static void apply(InputRange const& input_range, RescalePolicy const& rescale_policy, OutputCollection& output_collection, Strategy const& strategy)
					{
						typedef typename boost::range_value<OutputCollection>::type output_type;

						typedef typename geometry::point_type<output_type>::type point_type;
						typedef model::box<point_type> box_type;
						typedef dissolve_helper<box_type> dissolve_helper_type;
						typedef std::vector<dissolve_helper_type> helper_vector_type;

						// Vector with indices to both input_range (source 0) and output_collection (source 1)
						helper_vector_type helper_vector;

						// Vector with indices to helper-vector, for divide and conquer
						std::vector<int> index_vector;

						init_helper(input_range, helper_vector, strategy);

						// Fill intrusive list with copies, and determine bounding box
						int index = 0;
						for (typename boost::range_iterator<helper_vector_type const>::type
							it = boost::begin(helper_vector);
							it != boost::end(helper_vector);
							++it, ++index)
						{
							index_vector.push_back(index);
						}

						std::vector<output_type> unioned_collection;

						std::size_t size = 0, previous_size = 0;
						std::size_t n = 0;

						bool changed = false;
						while (divide_and_conquer(helper_vector, index_vector, input_range, rescale_policy, unioned_collection, strategy, changed) && n < 5)
						{
							// Remove everything which is already dissolved.
							helper_vector.erase(
								std::remove_if(helper_vector.begin(), helper_vector.end(), helper_dissolved<dissolve_helper_type>),
								helper_vector.end()
							);

							previous_size = size;
							size = helper_vector.size();
							n = previous_size == size ? n + 1 : 0;

							// Re-initialize the list
							index_vector.clear();
							index = 0;
							for (typename boost::range_iterator<helper_vector_type const>::type
								it = boost::begin(helper_vector);
								it != boost::end(helper_vector);
								++it, ++index)
							{
								index_vector.push_back(index);
							}

							changed = false;
						}

						// Add input+output to real output
						typedef typename boost::range_iterator<helper_vector_type>::type iterator_type;
						for (iterator_type it = boost::begin(helper_vector);
							it != boost::end(helper_vector);
							++it)
						{
							if (!it->dissolved)
							{
								switch (it->source)
								{
								case 0:
									detail::inserter::insert(
										get_geometry::apply(input_range, it->index),
										output_collection);
									break;
								case 1:
									detail::inserter::insert(
										get_geometry::apply(unioned_collection, it->index),
										output_collection);
									break;
								}
							}
						}
					}
				};
			}

			namespace dissolve
			{
				/*!
					\brief Traverses through intersection points / geometries. This version
						for dissolves calls traversal_ring_creator in two phases
					\ingroup dissolve
					*/
				template <bool Reverse, typename Backtrack>
				class traverse
				{
				public:
					template <typename Geometry, typename IntersectionStrategy, typename RobustPolicy, typename Turns, typename Rings, typename TurnInfoMap, typename Clusters, typename Visitor>
					static void apply(Geometry const& geometry,
							IntersectionStrategy const& intersection_strategy,
							RobustPolicy const& robust_policy,
							Turns& turns, Rings& rings,
							TurnInfoMap& turn_info_map,
							Clusters& clusters,
							Visitor& visitor)
					{
						detail::overlay::traversal_ring_creator
							<
							Reverse, Reverse, overlay_dissolve,
							Geometry, Geometry,
							Turns, TurnInfoMap, Clusters,
							IntersectionStrategy,
							RobustPolicy, Visitor,
							Backtrack
							> trav(geometry, geometry, turns, turn_info_map, clusters, intersection_strategy, robust_policy, visitor);

						std::size_t finalized_ring_size = boost::size(rings);

						typename Backtrack::state_type state;

						for (std::size_t phase = 0; phase < 2; phase++)
						{
							trav.iterate_with_preference(phase, rings, finalized_ring_size, state);
						}
					}
				};

				struct no_interrupt_policy
				{
					static bool const enabled = false;
					static bool const has_intersections = false;

					template <typename Range>
					static bool apply(Range const&)
					{
						return false;
					}
				};

				template<typename Geometry>
				class backtrack_for_dissolve
				{
				public :
					typedef detail::overlay::backtrack_state state_type;

					template <typename Operation, typename Rings, typename Turns, typename IntersectionStrategy, typename RobustPolicy, typename Visitor>
					static void apply(std::size_t size_at_start,
								Rings& rings,
								typename boost::range_value<Rings>::type& ring,
								Turns& turns,
								typename boost::range_value<Turns>::type const& /*turn*/,
								Operation& operation,
								detail::overlay::traverse_error_type,
								Geometry const& ,
								Geometry const& ,
								IntersectionStrategy const& ,
								RobustPolicy const& ,
								state_type& state,
								Visitor const& /*visitor*/
								)
					{
						state.m_good = false;

						// Make bad output clean
						rings.resize(size_at_start);
						ring.clear();

						// Reject this as a starting point
						operation.visited.set_rejected();

						// And clear all visit info
						clear_visit_info(turns);
					}
				};

				template <typename Ring, typename GeometryOut, bool Reverse>
				struct dissolve_ring
				{
					template <typename Turns>
					static void adapt_turns(Turns& turns)
					{
						typedef typename boost::range_value<Turns>::type turn_type;
						typedef typename turn_type::turn_operation_type turn_operation_type;

						for (typename Turns::iterator it = turns.begin(); it != turns.end(); ++it)
						{
							turn_type& turn = *it;
							for (int i = 0; i < 2; i++)
							{
								turn_operation_type& op = turn.operations[i];

								if (op.operation != detail::overlay::operation_union
									&& op.operation != detail::overlay::operation_continue)
								{
									// Only prefer union and continue turns
									op.enriched.prefer_start = false;
								}

								if (op.operation == detail::overlay::operation_intersection)
								{
									// Make all ii->uu, iu->uu, etc, basically handle most
									// as if it is union
									op.operation = detail::overlay::operation_union;
								}
							}
						}
					}

					template <typename RescalePolicy, typename OutputIterator, typename Strategy, typename Visitor>
					static void apply_one(Ring const& input_ring, RescalePolicy const& rescale_policy, OutputIterator out, Strategy const& strategy, Visitor& visitor)
					{
						using point_type = typename point_type<Ring>::type;

						// Get the self-intersection points, including turns
						using turn_info = detail::overlay::traversal_turn_info<point_type, typename segment_ratio_type<point_type, RescalePolicy>::type>;

						std::deque<turn_info> turns;
						detail::dissolve::no_interrupt_policy policy;
						detail::self_get_turn_points::self_turns<Reverse, detail::overlay::assign_null_policy>(input_ring, strategy, rescale_policy, turns, policy, 0, false);

						adapt_turns(turns);

						visitor.visit_turns(1, turns);

						if (boost::size(turns) == 0)
						{
							// No self-turns, then add original ring,
							// possibly reversing the order
							GeometryOut g;
							geometry::convert(input_ring, g);
							geometry::correct(g);
							*out++ = g;
						}

						std::map<signed_size_type, detail::overlay::cluster_info> clusters;

						// Enrich/traverse the polygons
						enrich_intersection_points<Reverse, Reverse, overlay_dissolve>(turns, clusters, input_ring, input_ring, rescale_policy, strategy);

						visitor.visit_turns(2, turns);

						visitor.visit_clusters(clusters, turns);

						std::deque<Ring> rings;
						std::map<ring_identifier, overlay::ring_turn_info> turn_info_per_ring;

						detail::dissolve::traverse<Reverse, backtrack_for_dissolve<Ring>>::apply(input_ring, strategy, rescale_policy, turns, rings, turn_info_per_ring, clusters, visitor);

						visitor.visit_turns(3, turns);
						visitor.visit_generated_rings(rings);

						detail::overlay::get_ring_turn_info<overlay_dissolve>(turn_info_per_ring, turns, clusters);

						using area_strategy_type = decltype(strategy.area(detail::dummy_ring()));
						using area_result_type = typename area_strategy_type::template result_type<point_type>::type;
						using properties = detail::overlay::ring_properties<point_type, area_result_type>;

						std::map<ring_identifier, properties> selected;

						detail::overlay::select_rings<overlay_dissolve>(input_ring, turn_info_per_ring, selected, strategy);

						{
							ring_identifier id(2, 0, -1);
							for (auto it = boost::begin(rings); it != boost::end(rings); ++it)
							{
								selected[id] = properties(*it, strategy);
								id.multi_index++;
							}
						}

						detail::overlay::assign_parents<overlay_dissolve>(input_ring, rings, selected, strategy);
						detail::overlay::add_rings<GeometryOut>(selected, input_ring, rings, out, strategy);
					}

					template <typename RescalePolicy, typename OutputIterator, typename Strategy, typename Visitor>
					static OutputIterator apply(Ring const& geometry, RescalePolicy const& rescale_policy, OutputIterator out, Strategy const& strategy, Visitor& visitor)
					{
						using multi_polygon = model::multi_polygon<GeometryOut>;
						multi_polygon step1;
						apply_one(geometry, rescale_policy, std::back_inserter(step1), strategy, visitor);

						// Step 2: remove mutual overlap
						{
							multi_polygon step2; // TODO avoid this, output to "out", if possible
							detail::dissolver::dissolver_generic<detail::dissolver::plusmin_policy>::apply(step1, rescale_policy, step2, strategy);

							for (auto it = step2.begin(); it != step2.end(); ++it)
							{
								*out++ = *it;
							}
						}

						return out;
					}
				};

				template <typename Polygon, typename GeometryOut, bool Reverse>
				struct dissolve_polygon
				{
					typedef typename ring_type<Polygon>::type ring_type;

					template <typename RescalePolicy, typename OutputCollection, typename Strategy, typename Visitor>
					static void apply_ring(ring_type const& ring, RescalePolicy const& rescale_policy, OutputCollection& out, Strategy const& strategy, Visitor& visitor)
					{
						bool const orientation_ok = geometry::area(ring) >= 0;
						if (orientation_ok)
						{
							dissolve_ring<ring_type, GeometryOut, Reverse>::apply(ring, rescale_policy, std::back_inserter(out), strategy, visitor);
						}
						else
						{
							// Apply the whole dissolve implementation reversed
							dissolve_ring<ring_type, GeometryOut, ! Reverse>::apply(ring, rescale_policy, std::back_inserter(out), strategy, visitor);
						}
					}

					template <typename Rings, typename RescalePolicy, typename OutputCollection, typename Strategy, typename Visitor>
					static void apply_rings(Rings const& rings, RescalePolicy const& rescale_policy, OutputCollection& out, Strategy const& strategy, Visitor& visitor)
					{
						for (auto it = boost::begin(rings); it != boost::end(rings); ++it)
						{
							apply_ring(*it, rescale_policy, out, strategy, visitor);
						}
					}

					template <typename RescalePolicy, typename OutputIterator, typename Strategy, typename Visitor>
					static OutputIterator apply(Polygon const& polygon, RescalePolicy const& rescale_policy, OutputIterator out, Strategy const& strategy, Visitor& visitor)
					{
						using multi_polygon = model::multi_polygon<GeometryOut>;

						// Handle exterior ring
						multi_polygon exterior_out;
						apply_ring(exterior_ring(polygon), rescale_policy, exterior_out, strategy, visitor);

						// Dissolve all the (negative) interior rings into a (positive) mulpolygon. Do this per interior ring and combine them.
						multi_polygon interior_out_per_ring;
						apply_rings(interior_rings(polygon), rescale_policy, interior_out_per_ring, strategy, visitor);

						// Remove mutual overlap in the interior ring output
						multi_polygon interior_out;
						detail::dissolver::dissolver_generic<detail::dissolver::plusmin_policy>::apply(interior_out_per_ring, rescale_policy, interior_out, strategy);

						// Subtract the interior rings from the output. Where interior rings
						// are partly or completely outside the polygon, sym_difference will
						// turn them into exterior rings. This is probably what most users will
						// expect - alternatively, difference could be used to have them pure
						// as interior rings only
						return detail::sym_difference::sym_difference_insert<GeometryOut>(exterior_out, interior_out, out);
					}
				};
			}
		}

		template <typename Geometry, typename Collection>
		void dissolve(Geometry const& geometry, Collection& output_collection)
		{
			using geometry_out = typename boost::range_value<Collection>::type;
			using strategy_type = typename strategies::relate::services::default_strategy<Geometry, Geometry>::type;
			using rescale_policy_type = typename geometry::rescale_policy_type<typename geometry::point_type<Geometry>::type, typename strategy_type::cs_tag>::type;

			strategy_type strategy{};
			rescale_policy_type robust_policy = geometry::get_rescale_policy<rescale_policy_type>(geometry, strategy);

			detail::overlay::overlay_null_visitor visitor;
			constexpr bool Reverse = detail::overlay::do_reverse<geometry::point_order<Geometry>::value>::value;

			detail::dissolve::dissolve_polygon<Geometry, geometry_out, Reverse>::apply(
				geometry,
				robust_policy,
				std::back_inserter(output_collection),
				strategy,
				visitor
			);
		}
	}
}
