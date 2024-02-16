#pragma once
#include <glm/vec2.hpp>
#include "arcball_transformation.h"
#include "../geometry_store/geom_store.h"

class mouse_events
{
public:
	geom_store* geom = nullptr;

	glm::vec2 click_pt = glm::vec2(0);
	glm::vec2 curr_pt = glm::vec2(0);
	glm::vec2 prev_translation = glm::vec2(0);
	glm::vec2 total_translation = glm::vec2(0);
	glm::vec2 prev_rotation = glm::vec2(0);
	glm::vec2 total_rotation = glm::vec2(0);
	bool is_pan = false;
	bool is_rotate = false;
	bool is_rightbutton = false;
	bool is_select = false;
	double zoom_val = 1.0;

	mouse_events();
	~mouse_events();
	void init(geom_store& geom);

	void mouse_location(glm::vec2& loc);

	// Pan operation
	void pan_operation_start(glm::vec2& loc);
	void pan_operation_ends();

	// Rotate operation
	void rotation_operation_start(glm::vec2& loc);
	void rotation_operation_ends(glm::vec2& loc);

	// Select operation (Selection rectangle draw)
	void select_operation_start(glm::vec2& loc, bool is_rightbutton);
	void select_operation_ends(glm::vec2& current_loc);

	void zoom_operation(double& e_delta, glm::vec2& loc);
	void zoom_to_fit();
	void change_viewport();
	void left_mouse_click(glm::vec2& loc);
	void left_mouse_doubleclick(glm::vec2& loc);
	void right_mouse_click(glm::vec2& loc);
	void right_mouse_doubleclick(glm::vec2& loc);

	glm::vec2 intellizoom_normalized_screen_pt(glm::vec2 loc);
private:
	int viewType = 1;

	void pan_operation(glm::vec2& current_translataion);
	void rotation_operation(glm::vec2& loc);
	void select_operation(glm::vec2& click_loc, glm::vec2& current_loc);

	glm::vec2 get_rotation_screen_pt(glm::vec2& mouse_loc);

	arcball_transformation arcball;
};
