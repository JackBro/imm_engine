-- describe_object.lua
-- read object information to immature engine

-- require
require "script\\library\\csv_dump"

-- global
-- all name need to be unique
csv_model_basic = read_csv("script\\scene\\model_basic.csv")
csv_object_basic = read_csv("script\\scene\\object_basic.csv")
csv_model_skinned = read_csv("script\\scene\\model_skinned.csv")
csv_object_skinned = read_csv("script\\scene\\object_skinned.csv")
csv_model_geometry = read_csv("script\\scene\\model_geometry.csv")
csv_object_geometry = read_csv("script\\scene\\object_geometry.csv")
