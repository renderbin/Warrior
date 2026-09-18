# -*- coding: utf-8 -*-
"""快速探针：骨骼层级 + API 可用性 + 单条动画的根位移情况"""
import unreal
import os

SKEL_PATH = "/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01_Skeleton"
MESH_PATH = "/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01"
ANIM_DIR = "/Game/Mesh/YinHU/Animtions"
OUT = "D:/Warrior/Saved/probe_out.txt"

L = []
def log(m):
    L.append(str(m))
    unreal.log(str(m))
    print(m, flush=True)

def dump():
    try:
        os.makedirs(os.path.dirname(OUT), exist_ok=True)
        open(OUT, "w", encoding="utf-8").write("\n".join(L))
    except Exception as e:
        print("write fail", e)

try:
    log("=== PROBE START ===")

    log("\n--- unreal 模块里跟骨骼/动画相关的 API ---")
    for mod in ("AnimationLibrary", "AnimationBlueprintLibrary", "AnimPoseExtensions",
                "SkeletonLibrary", "SkeletalMeshLibrary", "EditorSkeletalMeshLibrary",
                "AnimationDataController", "AnimationDataModel"):
        m = getattr(unreal, mod, None)
        log("  unreal.%-28s %s" % (mod, "MISSING" if m is None else "ok"))

    skel = unreal.load_asset(SKEL_PATH)
    log("\n--- skeleton ---")
    log("  load: %s" % (skel.get_path_name() if skel else "FAILED"))
    if skel:
        log("  class: %s" % skel.get_class().get_name())
        log("  editor props: %s" % [p for p in skel.get_editor_property.__self__.__class__.__name__] if False else "")

    mesh = unreal.load_asset(MESH_PATH)
    log("  mesh load: %s" % (mesh.get_path_name() if mesh else "FAILED"))

    # 尝试各种拿骨骼名的路子
    log("\n--- 找骨骼名 ---")
    got = None
    AL = getattr(unreal, "AnimationLibrary", None) or getattr(unreal, "AnimationBlueprintLibrary", None)
    if AL:
        log("  lib: %s" % AL)
        log("  has get_bone_names: %s" % hasattr(AL, "get_bone_names"))
        if hasattr(AL, "get_bone_names"):
            try:
                got = list(AL.get_bone_names(skel))
                log("  get_bone_names(skeleton) -> %d bones" % len(got))
            except Exception as e:
                log("  get_bone_names(skeleton) ERR: %s" % e)

    if got:
        log("\n--- 顶层 25 根骨骼（index 0 = skeleton root）---")
        for i, n in enumerate(got[:25]):
            log("    [%3d] %s" % (i, n))
        log("  total bones: %d" % len(got))

    # 列一条动画
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = ar.get_assets_by_path(ANIM_DIR, recursive=False)
    seqs = []
    for a in assets:
        if str(a.asset_class_path.asset_name) == "AnimSequence":
            o = a.get_asset()
            if o: seqs.append(o)
    log("\n--- anims found: %d ---" % len(seqs))

    if seqs and got and AL:
        s = sorted(seqs, key=lambda o: o.get_name())[0]
        log("  sample: %s" % s.get_name())
        log("  frames: %s" % AL.get_num_frames(s))
        log("  is_root_motion_enabled: %s" % AL.is_root_motion_enabled(s))
        log("  root_motion_lock: %s" % AL.get_root_motion_lock_type(s))
        log("  is_root_motion_lock_forced: %s" % AL.is_root_motion_lock_forced(s))
        nf = AL.get_num_frames(s)
        log("\n  --- 每根骨骼在 0/mid/end 帧的局部位移（看谁在动）---")
        for i, bn in enumerate(got[:8]):
            try:
                ts = [AL.get_bone_pose_for_frame(s, bn, f, False) for f in (0, nf // 2, nf)]
                tr = [(round(t.translation.x, 3), round(t.translation.y, 3), round(t.translation.z, 3)) for t in ts]
                log("    [%2d] %-28s %s" % (i, bn, tr))
            except Exception as e:
                log("    [%2d] %-28s ERR %s" % (i, bn, e))

    log("\n=== PROBE END ===")
except Exception:
    import traceback
    log("FATAL:\n" + traceback.format_exc())
finally:
    dump()
