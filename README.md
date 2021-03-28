# Flexer

Header-only, simple, flexible layout engine for C++.

# Example
```cpp
auto flex = std::make_unique<Flexer>();

EID par1 = flex->createElement({
	.bounds = Rect(10, 10, ScreenWidth() - 20, ScreenHeight() - 20),
	.axis = Axis::Vertical
});
EID par2 = flex->createElement({
	.parent = par1,
	.bounds = Rect(0, 0, 0, 80),
	.proportion = 0,
	.axis = Axis::Horizontal
});

flex->createElement({
	.parent = par2,
	.bounds = Rect(0, 0, 120, 0),
	.proportion = 1
});

flex->createElement({
	.parent = par2,
	.bounds = Rect(0, 0, 120, 0),
	.proportion = 0
});

flex->createElement({
	.parent = par2,
	.bounds = Rect(0, 0, 120, 0),
	.proportion = 1
});
```

## Result
![Centering an element with a fixed size](https://i.imgur.com/sfOVew0.png)