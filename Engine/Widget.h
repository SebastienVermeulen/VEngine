#pragma once

class Widget abstract
{
public:
	Widget();
	virtual ~Widget();

	Widget(Widget& other) = delete;
	Widget(Widget&& other) = delete;
	Widget operator=(Widget& other) = delete;
	Widget& operator=(Widget&& other) = delete;

	virtual bool RenderUITitle(int idx) = 0;
	virtual void RenderUIElement(int idx) = 0;
};