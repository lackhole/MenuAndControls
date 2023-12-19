#pragma once

#include <memory>
#include <utility>

#include "shape.h"

class ShapeBuilderBase {
public:
	ShapeBuilderBase() = default;
	virtual ~ShapeBuilderBase() = default;

	virtual std::unique_ptr<ShapeBase> Create() {
		return std::make_unique<ShapeNull>();
	}
};

template<typename T>
class ShapeBuilder : public ShapeBuilderBase {
public:
	ShapeBuilder() = default;

	std::unique_ptr<ShapeBase> Create() override {
		return std::make_unique<T>();
	}
};