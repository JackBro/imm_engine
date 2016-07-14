-- concrete_common.lua
-- read common information to immature engine

-- require
require "script\\library\\csv_dump"

-- csv
csv_audio_bgm    = read_csv("script\\scene_common\\audio_bgm.csv")
csv_audio_effect = read_csv("script\\scene_common\\audio_effect.csv")
csv_hit_box      = read_csv("script\\scene_common\\hit_box.csv")
csv_bound_offset = read_csv("script\\scene_common\\bound_offset.csv")
csv_avatar       = read_csv("script\\scene_common\\avatar.csv")
csv_weapon_map   = read_csv("script\\scene_common\\weapon_map.csv")
csv_skill        = read_csv("script\\scene_common\\skill.csv")
csv_status_data  = read_csv("script\\scene_common\\status_data.csv")
csv_action       = read_csv("script\\scene_common\\action.csv")
csv_solid_effect = read_csv("script\\scene_common\\solid_effect.csv")
