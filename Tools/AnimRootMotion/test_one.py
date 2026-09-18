# -*- coding: utf-8 -*-
"""单条测试：开启 root motion 并存盘，验证持久化"""
import unreal, os

TEST = "/Game/Mesh/YinHU/Animtions/AS_player_hfm_hu_rush_loop_fwd"
OUT = "D:/Warrior/Saved/test_one.txt"
L = []
def log(m):
    L.append(str(m)); unreal.log(str(m)); print(m, flush=True)
def dump():
    open(OUT, "w", encoding="utf-8").write("\n".join(L))

AL = unreal.AnimationLibrary
EAL = unreal.EditorAssetLibrary

try:
    s = unreal.load_asset(TEST)
    log("asset      : %s" % (s.get_path_name() if s else "LOAD FAILED"))
    log("before  rm : %s" % AL.is_root_motion_enabled(s))
    log("before lock: %s" % AL.get_root_motion_lock_type(s))
    log("before frcd: %s" % AL.is_root_motion_lock_forced(s))

    s.modify()
    AL.set_root_motion_enabled(s, True)
    log("after set  : %s" % AL.is_root_motion_enabled(s))

    ok = EAL.save_asset(TEST, only_if_is_dirty=False)
    log("save_asset : %s" % ok)
    log("is_dirty   : %s" % s.get_package().is_dirty() if hasattr(s.get_package(), "is_dirty") else "n/a")
except Exception:
    import traceback
    log("FATAL:\n" + traceback.format_exc())
finally:
    dump()
