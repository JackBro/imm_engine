////////////////
// control_atk.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015-2016 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#include "control_atk.h"
#include "imm_ptr.h"
namespace imm
{
////////////////
// skill_data
////////////////
////////////////
skill_data::skill_data()
{
	;
}
//
void skill_data::current_apply(skill_para &pa)
{
	// deactive the previous box
	if (pa.skill_ix > 0) PTR->m_Hit.set_active_box(pa.inst_ix, atk_box[pa.skill_ix-1], false);
	pa.is_judge = false;
	pa.count_down = frame_end[pa.skill_ix];
	PTR->m_Inst.m_Stat[pa.inst_ix].check_set_ClipName(atk[pa.skill_ix], true);
	PTR->m_Control.map_speed[pa.inst_ix].set(pa.inst_ix, inst_speed2[pa.skill_ix]);
	pa.is_busy = true;
	pa.current_ix = pa.skill_ix;
	if (get_skill_type(pa) == SKILL_TYPE_MAGIC) {
		PTR->m_Magic.invoke(specify[pa.current_ix], pa.inst_ix);
	}
}
//
void skill_data::chunk_over(skill_para &pa)
{
	pa.is_turn_next = false;
	pa.is_busy = false;
	pa.is_judge = false;
	pa.skill_ix = -1;
	pa.count_down = -1.0f;
	pa.is_execute = false;
	pa.is_adjust_dir = false;
	PTR->m_Control.atk.hits[pa.inst_ix].clear();
	PTR->m_Hit.deactive_box(pa.inst_ix);
	auto &tro = PTR->m_Inst.m_Troll[pa.inst_ix];
	tro.order |= ORDER_ENGAGE;
	tro.order |= ORDER_IDLE;
}
//
bool skill_data::is_required_ap(skill_para &pa)
{
	bool result = (PTR->m_AiAttr.is_required_ap(specify[pa.skill_ix], pa.inst_ix));
	if (result) {
		return true;
	}
	else {
		chunk_over(pa);
		return false;
	}
}
//
void skill_data::strike(skill_para &pa)
{
	assert(pa.skill_ix > -2);
	if (pa.skill_ix < 0 && pa.count_down > 0.0f) {
		// in first attack
		return;
	}
	if (pa.skill_ix == -1) {
		pa.is_turn_next = false;
		if (!chunk.count(pa.symbol)) {
			chunk_over(pa);
			return;
		}
		pa.skill_ix = chunk[pa.symbol];
		if (!is_required_ap(pa)) {
			return;
		}
		current_apply(pa);
		return;
	}
	// if skill only one hit
	if (next_ix[pa.skill_ix] == -1) {
		return;
	}
	// if trun next hit
	if (!pa.is_turn_next && pa.count_down > FPS_MIN_REQ_1DIV && pa.count_down > frame_turn[pa.skill_ix]) {
		pa.skill_ix = next_ix[pa.skill_ix];
		pa.is_turn_next = true;
		return;
	}
}
//
void skill_data::update(const float &dt, skill_para &pa)
{
	if (!pa.is_busy) return;
	if (PTR->m_Inst.m_Troll[pa.inst_ix].current_state == pose_Damage::instance()) {
		chunk_over(pa);
	}
	if (pa.count_down > 0.0f) pa.count_down -= dt;
	//
	if (!pa.is_judge) {
		if (frame_end[pa.current_ix] - pa.count_down > judge_start[pa.current_ix]) {
			PTR->m_Hit.set_active_box(pa.inst_ix, atk_box[pa.current_ix], true);
			pa.is_judge = true;
		}
	}
	if (pa.is_judge) {
		if (frame_end[pa.current_ix] - pa.count_down > judge_end[pa.current_ix]) {
			PTR->m_Hit.set_active_box(pa.inst_ix, atk_box[pa.current_ix], false);
			pa.is_judge = false;
		}
	}
	//
	if (pa.count_down < 0.0f) {
		chunk_over(pa);
		return;
	}
	if (pa.is_turn_next) {
		if (pa.skill_ix < 1 || pa.skill_ix > frame_turn.size()) {
			assert(pa.skill_ix > 0);
			assert(pa.skill_ix <= frame_turn.size());
			pa.is_turn_next = false;
			return;
		}
		if (pa.count_down < frame_turn[pa.skill_ix-1]) {
			if (frame_turn[pa.skill_ix-1]-pa.count_down > FPS_MIN_REQ_1DIV) {
				return;
			}
			
			if (!is_required_ap(pa)) {
				return;
			}
			current_apply(pa);
			pa.is_turn_next = false;
			if (next_ix[pa.skill_ix] == -1) {
				pa.skill_ix = -1;
			}
			return;
		}
	}
}
//
SKILL_TYPE skill_data::get_skill_type(const skill_para &pa)
{
	if (specify[pa.current_ix] > SKILL_MELEE_STANDARD) return SKILL_TYPE_MAGIC;
	return SKILL_TYPE_MELEE;
}
////////////////
// damage_data
////////////////
////////////////
damage_data::damage_data():
	ix_atk(0),
	ix_dmg(0),
	skill_ix(-2),
	count_down(-1.0f),
	delay(-1.0f),
	is_calculated(true),
	is_delay(false),
	box_center(nullptr),
	specify(SKILL_MELEE_STANDARD)
{
	;
}
//
void damage_data::update(const float &dt)
{
	if (count_down > 0.0f) count_down -= dt;
	if (!(PTR->m_Inst.m_Stat[ix_dmg].property & INST_IS_CONTROLLABLE)) {
		is_calculated = true;
		return;
	}
	if (specify <= SKILL_MELEE_STANDARD) {
		update_melee(dt);
	}
	else {
		update_magic(dt);
	}
}
//
void damage_data::update_melee(const float &dt)
{
	if (!is_calculated) {
		PTR->m_AiAttr.calc_skill_melee_immediately(specify, ix_atk, ix_dmg);
		math::set_inst_speed(ix_dmg, 0.0f);
		if (!PTR->m_Control.atk.para_ski[ix_atk].is_adjust_dir) {
			if (PTR->m_Inst.m_Probe.intersects_oblong(ix_atk, ix_dmg)) {
				PTR->m_Inst.m_Troll[ix_atk].focus = static_cast<int>(ix_dmg);
				math::set_face_to_face(ix_atk, ix_dmg);
				PTR->m_Control.atk.para_ski[ix_atk].is_adjust_dir = true;
			}
		}
		//
		is_delay = true;
		delay = 0.03f;
		is_calculated = true;
	}
	if (is_delay) {
		delay -= dt;
		if (delay < 0.0f) {
			assert(box_center);
			//hit postion roughly
			XMFLOAT3 box = *box_center;
			XMFLOAT3 center = PTR->m_Inst.m_BoundW.center(ix_dmg);
			center.y += (box.y-center.y)*0.8f;
			PTR->m_SfxSelect.play_effect(specify, ix_atk, ix_dmg, center);
			PTR->m_AiAttr.calc_skill_melee_delay(specify, ix_atk, ix_dmg);
			is_delay = false;
		}
	}
}
//
void damage_data::update_magic(const float &dt)
{
	if (!is_calculated) {
		is_delay = true;
		delay = 0.2f;
		is_calculated = true;
	}
	if (is_delay) {
		delay -= dt;
		if (delay < 0.0f) {
			math::set_inst_speed(ix_dmg, 0.0f);
			if (!PTR->m_Control.atk.para_ski[ix_atk].is_adjust_dir) {
				if (PTR->m_Inst.m_Probe.intersects_oblong(ix_atk, ix_dmg)) {
					PTR->m_Inst.m_Troll[ix_atk].focus = static_cast<int>(ix_dmg);
					math::set_face_to_face(ix_atk, ix_dmg);
					PTR->m_Control.atk.para_ski[ix_atk].is_adjust_dir = true;
				}
			}
			PTR->m_Scene.audio.play_effect(sfx::Punch);
			PTR->m_AiAttr.calc_skill_magic_delay(specify, ix_atk, ix_dmg);
			is_delay = false;
		}
	}
}
//
void damage_data::stamp()
{
	// avoid hits at one time
	if (count_down > 0.0f) {
		return;
	}
	else {
		count_down = PTR->m_Control.atk.para_ski[ix_atk].count_down;
		is_calculated = false;
		PTR->m_Inst.m_Steering[ix_atk].attack.push_back(ix_dmg);
		PTR->m_Inst.m_Steering[ix_dmg].damage.push_back(ix_atk);
	}
}
////////////////
// control_atk
////////////////
////////////////
template <typename T_app>
control_atk<T_app>::control_atk():
	app(nullptr)
{
	;
}
//
template <typename T_app>
void control_atk<T_app>::init(T_app *app_in)
{
	app = app_in;
	std::string concrete = IMM_PATH["script"]+"concrete_common.lua";
	lua_reader l_reader;
	l_reader.loadfile(concrete);
	std::vector<std::vector<std::string>> vec2d;
	l_reader.vec2d_str_from_table("csv_skill", vec2d);
	for (size_t ix = 1; ix < vec2d.size(); ++ix) {
		auto d_skill = &data_ski[vec2d[ix][0]];
		if (!d_skill->chunk.count(vec2d[ix][1][0])) {
			d_skill->chunk[vec2d[ix][1][0]] = static_cast<int>(data_ski[vec2d[ix][0]].atk.size());
		}
		d_skill->atk.push_back(vec2d[ix][2]);
		d_skill->frame_end.push_back(std::stof(vec2d[ix][3]) * FRAME_RATE_1DIV);
		d_skill->frame_turn.push_back( (std::stof(vec2d[ix][3])-std::stof(vec2d[ix][4])) * FRAME_RATE_1DIV);
		d_skill->judge_start.push_back(std::stof(vec2d[ix][5]) * FRAME_RATE_1DIV);
		d_skill->judge_end.push_back(std::stof(vec2d[ix][6]) * FRAME_RATE_1DIV);
		d_skill->next_ix.push_back(std::stoi(vec2d[ix][7]));
		d_skill->specify.push_back(skill_specify_str(vec2d[ix][8]));
		std::vector<std::string> box_name = csv_str_split(vec2d[ix][9], '~');
		if (box_name[0].size() == 0) box_name.clear();
		d_skill->atk_box.push_back(box_name);
		std::vector<std::string> speed_str = csv_str_split(vec2d[ix][10], '~');
		XMFLOAT2 speed;
		std::list<XMFLOAT2> speed_list;
		if (speed_str.size() % 2) {
			speed.x = 0.0f;
			speed.y = std::stof(speed_str[0]);
			speed_list.emplace_back(speed);
			d_skill->inst_speed2.emplace_back(speed_list);
		}
		else {
			for (size_t ind = 0; ind < speed_str.size(); ind += 2) {
				speed.x = std::stof(speed_str[ind]) * FRAME_RATE_1DIV;
				speed.y = std::stof(speed_str[ind+1]);
				speed_list.emplace_back(speed);
			}
			d_skill->inst_speed2.emplace_back(speed_list);
		}
		d_skill->impulse.push_back(std::stof(vec2d[ix][11]));
	}
}
//
template <typename T_app>
void control_atk<T_app>::rebuild_action()
{
	;
}
//
template <typename T_app>
void control_atk<T_app>::reset()
{
	para_ski.clear();
	damage.clear();
}
//
template <typename T_app>
void control_atk<T_app>::init_skill_para(const size_t &index_in)
{
	para_ski[index_in];
	para_ski[index_in].model_name = *app->m_Inst.m_Stat[index_in].get_ModelName();
	para_ski[index_in].inst_ix = index_in;
}
//
template <typename T_app>
void control_atk<T_app>::cause_damage(
	const size_t &inst_ix_atk,
	const size_t &inst_ix_dmg,
	const XMFLOAT3 &box_center,
	const SKILL_SPECIFY &specify)
{
	if (is_cannot_be_attacked(inst_ix_dmg)) return;
	assert(inst_ix_atk < 1000);
	assert(inst_ix_dmg < 1000);
	assert(para_ski[inst_ix_atk].current_ix < 100);
	assert(para_ski[inst_ix_atk].current_ix > -1);
	int index =
		para_ski[inst_ix_atk].current_ix +
		static_cast<int>(inst_ix_atk)*1000 +
		static_cast<int>(inst_ix_dmg)*1000000;
	if (!damage.count(index)) {
		// init
		damage[index].ix_atk = inst_ix_atk;
		damage[index].ix_dmg = inst_ix_dmg;
		damage[index].skill_ix = para_ski[inst_ix_atk].current_ix;
		damage[index].box_center = &box_center;
		damage[index].specify = specify;
	}
	damage[index].stamp();
	hits[inst_ix_atk].insert(inst_ix_dmg);
	//
}
//
template <typename T_app>
void control_atk<T_app>::execute(const size_t &index_in, const char &symbol)
{
	if (!data_ski.count(*app->m_Inst.m_Stat[index_in].get_ModelName())) {
		PTR->m_Inst.m_Troll[index_in].order |= ORDER_IDLE;
		return;
	}
	if (!para_ski.count(index_in)) init_skill_para(index_in);
	if (para_ski[index_in].skill_ix == -1) para_ski[index_in].symbol = symbol;
	para_ski[index_in].is_execute = true;
	para_ski[index_in].is_adjust_dir = false;
	data_ski[para_ski[index_in].model_name].strike(para_ski[index_in]);
}
//
template <typename T_app>
void control_atk<T_app>::update(const float &dt)
{
	for (auto &para_it: para_ski) {
		data_ski[para_it.second.model_name].update(dt, para_it.second);
	}
	for (auto &dmg: damage) {
		dmg.second.update(dt);
	}
}
//
template <typename T_app>
bool control_atk<T_app>::is_execute(const size_t &index_in)
{
	if (para_ski.count(index_in)) {
		if (para_ski[index_in].is_execute) {	
			return true;
		}
	}
	return false;
}
//
template <typename T_app>
bool control_atk<T_app>::is_cannot_be_attacked(const size_t &damage_ix)
{
	if (PTR->m_Inst.m_Troll[damage_ix].current_state == pose_FallDown::instance()) return true;
	return false;
}
//
template <typename T_app>
float control_atk<T_app>::current_impulse(const size_t &index_in)
{
	if (para_ski.count(index_in)) {
		if (para_ski[index_in].is_execute) {	
			return data_ski.at(*app->m_Inst.m_Stat[index_in].get_ModelName()).impulse[para_ski[index_in].current_ix];
		}
	}
	assert(para_ski.count(index_in));
	return 0.0f;
}
//
}