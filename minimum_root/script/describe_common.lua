-- describe_common.lua
-- read common information to immature engine

-- require
require "script\\library\\csv_dump"

-- csv
csv_audio_bgm    = read_csv("script\\scene_common\\audio_bgm.csv")
csv_audio_effect = read_csv("script\\scene_common\\audio_effect.csv")
csv_attack_box   = read_csv("script\\scene_common\\attack_box.csv")

