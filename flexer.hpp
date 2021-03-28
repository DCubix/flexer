#ifndef FLEXER_HPP
#define FLEXER_HPP

/**
 * ========================================================================================
 * Project: Flexer - Simple flexible layout engine
 * Author: Diego Lopes <diego95lopes@gmail.com>
 * Date: 03/21/2021
 * ========================================================================================
 * BASIC USAGE
 * ========================================================================================
 * Simply include the header into your program, and define FLEXER_IMPLEMENTATION like so:
 * 
 * #define FLEXER_IMPLEMENTATION
 * #include "flexer.hpp"
 * 
 * Create a new instance of flexer::Flexer and Call performLayout() on each tick.
 * 
 * std::unique_ptr<flexer::Flexer> flex = std::make_unique<flexer::Flexer>();
 * ...
 * flex->performLayout();
 * 
 * ========================================================================================
 * EMBEDDING INTO AN EXISTING GUI FRAMEWORK
 * ========================================================================================
 * After calling performLayout(), a new rectangle will get generated. You can access that
 * by simply calling getElementRect(id) and passing the desired element ID.
 * 
 * You can use the returned rectangle both for rendering and handling events.
 * 
 * ========================================================================================
 * VERSION HISTORY
 * ========================================================================================
 * 		- v1.0: Initial version release
 * 
 */

#include <cstdint>
#include <vector>
#include <map>
#include <memory>
#include <numeric>

namespace flexer {

	enum class Axis {
		Horizontal = 0,
		Vertical
	};

	struct Rect {
		int x{ 0 }, y{ 0 }, width{ 100 }, height{ 100 };

		Rect() = default;
		Rect(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}
	};

	using EID = uint64_t;

	struct Element {
		EID parent{ 0 };
		Rect bounds{};

		/**
		 * Specifies how much the element will grow relative to the rest of the elements in a container.
		 * A proportion value of 0 will not be considered, instead the specified element size will get used.
		 */
		unsigned int proportion{ 1 };

		/**
		 * Outer spacing of the elements
		 */
		unsigned int border{ 3 };

		/**
		 * Spacing between elements
		 */
		unsigned int spacing{ 3 };

		std::vector<EID> children{};

		/**
		 * Axis on which the elements will flow
		 */
		Axis axis{ Axis::Horizontal };
	};

	class Flexer {
	public:
		Flexer() = default;
		~Flexer() = default;

		EID createElement(Element el);
		Element* getElement(EID id);
		Rect getElementRect(EID id);

		/**
		 * Gets a map of element rectangles. Use this after calling performLayout() to render your widgets and perform event handling.
		 * @retval A map containing the generated rectangles, which are mapped by element ID.
		 */
		const std::map<EID, Rect>& rects() const { return m_elementLayoutRects; }

		/**
		 * Recursively (re)generates all the layout rectangles of your elements.
		 */
		void performLayout() {
			performContainerLayout(1);
		}

	private:
		EID m_globalID{ 1 };

		std::map<EID, std::unique_ptr<Element>> m_elements;
		std::map<EID, Rect> m_elementLayoutRects;

		void addParent(EID id, EID parent);
		void performContainerLayout(EID id);

	};
}

#ifdef FLEXER_IMPLEMENTATION
namespace flexer {
	EID Flexer::createElement(Element el) {
		EID id = m_globalID++;
		m_elements[id] = std::make_unique<Element>();
		m_elements[id]->bounds = el.bounds;
		m_elements[id]->axis = el.axis;
		m_elements[id]->proportion = el.proportion;
		m_elements[id]->border = el.border;
		m_elements[id]->spacing = el.spacing;
		addParent(id, el.parent);
		return id;
	}

	Element* Flexer::getElement(EID id) { 
		if (m_elements.find(id) == m_elements.end()) return nullptr;
		return m_elements[id].get();
	}

	Rect Flexer::getElementRect(EID id) {
		return m_elementLayoutRects[id];
	}

	void Flexer::addParent(EID id, EID parent) {
		if (parent != 0) {
			m_elements[id]->parent = parent;
			m_elements[parent]->children.push_back(id);
		}
	}

	void Flexer::performContainerLayout(EID id) {
		if (m_elements.find(id) == m_elements.end()) return;

		Element* el = getElement(id);
		if (el->parent == 0) {
			m_elementLayoutRects[id] = el->bounds;
		}
		
		int totalProportions = 0;
		for (auto& e : el->children) {
			totalProportions += getElement(e)->proportion;
		}
		totalProportions = totalProportions <= 0 ? 1 : totalProportions;

		if (el->children.empty()) return;

		Rect parentBounds = m_elementLayoutRects[id];
		int& compSize = el->axis == Axis::Horizontal ? parentBounds.width : parentBounds.height;
		int& comp = el->axis == Axis::Horizontal ? parentBounds.x : parentBounds.y;

		for (auto& e : el->children) {
			Element* c = getElement(e);
			int size = el->axis == Axis::Horizontal ? c->bounds.width : c->bounds.height;
			if (c->proportion == 0) {
				compSize -= size;
			}
		}
		const int size = (el->axis == Axis::Horizontal ? parentBounds.width : parentBounds.height) - el->border * 2;

		for (auto& e : el->children) {
			Element* c = getElement(e);

			Rect ret = c->bounds;
			ret.x += parentBounds.x + el->border;
			ret.y += parentBounds.y + el->border;

			switch (el->axis) {
				case Axis::Horizontal: {
					const int sizeSlice = (size - (c->proportion <= 0 ? ret.width : 0)) / totalProportions;
					if (c->proportion >= 1) {
						ret.width = sizeSlice * c->proportion;
					}
					ret.height = parentBounds.height - el->border * 2;
					parentBounds.x += ret.width;
					parentBounds.width -= ret.width;
					if (e != el->children.back()) ret.width -= el->spacing;
				} break;
				case Axis::Vertical: {
					const int sizeSlice = (size - (c->proportion <= 0 ? ret.height : 0)) / totalProportions;
					if (c->proportion >= 1) {
						ret.height = sizeSlice * c->proportion;
					}
					ret.width = parentBounds.width - el->border * 2;
					parentBounds.y += ret.height;
					parentBounds.height -= ret.height;
					if (e != el->children.back()) ret.height -= el->spacing;
				} break;
			}

			m_elementLayoutRects[e] = ret;
			performContainerLayout(e);
		}
	}
}
#endif

#endif // FLEXER_HPP
