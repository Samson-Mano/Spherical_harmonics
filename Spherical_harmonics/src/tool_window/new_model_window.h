#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"


class new_model_window
{
public:
	bool is_show_window = false;
	bool execute_create_model = false;
	int option_model_type = 0;
	std::vector<std::string> input_data;


	new_model_window();
	~new_model_window();
	void init();
	void render_window();
private:

};
