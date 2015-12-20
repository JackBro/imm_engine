////////////////
// control_atk_build.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
void skill_data::build(const std::string &name)
{
	// game date
	if (name == "sinon") {
		std::vector<std::string> box_name;
		type = SKILL_TYPE_WEAPON;
		//
		atk.push_back("Atk01");
		frame_end.push_back(9.0f);
		frame_turn.push_back(4.5f-2.0f);
		frame_speed.push_back(1.0f);
		box_name.clear();
		box_name.push_back("hand_L");
		atk_box.push_back(box_name);
		//
		atk.push_back("Atk02");
		frame_end.push_back(11.0f);
		frame_turn.push_back(5.5f-2.0f);
		frame_speed.push_back(0.5f);
		box_name.clear();
		box_name.push_back("foot_R");
		atk_box.push_back(box_name);
	}
	if (name == "pepper") {
		std::vector<std::string> box_name;
		type = SKILL_TYPE_WEAPON;
		//
		atk.push_back("Atk01");
		frame_end.push_back(9.0f);
		frame_turn.push_back(4.5f-2.0f);
		frame_speed.push_back(1.0f);
		box_name.clear();
		box_name.push_back("hand_L");
		atk_box.push_back(box_name);
		//
		atk.push_back("Atk02");
		frame_end.push_back(11.0f);
		frame_turn.push_back(5.5f-2.0f);
		frame_speed.push_back(0.5f);
		box_name.clear();
		box_name.push_back("foot_R");
		atk_box.push_back(box_name);
	}
	if (name == "pepper_B") {
		std::vector<std::string> box_name;
		type = SKILL_TYPE_WEAPON;
		//
		atk.push_back("Atk01");
		frame_end.push_back(9.0f);
		frame_turn.push_back(4.5f-2.0f);
		frame_speed.push_back(1.0f);
		box_name.clear();
		box_name.push_back("hand_L");
		atk_box.push_back(box_name);
		//
		atk.push_back("Atk02");
		frame_end.push_back(11.0f);
		frame_turn.push_back(5.5f-2.0f);
		frame_speed.push_back(0.5f);
		box_name.clear();
		box_name.push_back("foot_R");
		atk_box.push_back(box_name);
	}	
	for (auto &end: frame_end) end /= FRAME_RATE;
	for (auto &turn: frame_turn) turn /= FRAME_RATE;
}
//
template <typename T_app>
void control_atk<T_app>::init(T_app *app_in, const std::string suffix_in)
{
	app = app_in;
	suffix = suffix_in;
	data_ski["sinon"].build("sinon");
	data_ski["pepper"].build("pepper");
}