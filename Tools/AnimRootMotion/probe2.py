# -*- coding: utf-8 -*-
import unreal, os, traceback

SKEL_PATH = "/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01_Skeleton"
MESH_PATH = "/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01"
ANIM_DIR = "/Game/Mesh/YinHU/Animtions"
OUT = "D:/Warrior/Saved/probe_out.txt"

L = []
def log(m):
    L.append(str(m)); unreal.log(str(m)); print(m, flush=True)

def dump():
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    open(OUT, "w", encoding="utf-8").write("\n".join(L))

try:
    for mod in ("AnimationLibrary", "AnimPoseExtensions", "EditorSkeletalMeshLibrary",
                "AnimationDataController", "AnimationDataModel"):
        m = getattr(unreal, mod, None)
        if m is None:
            log("unreal.%s = MISSING" % mod); continue
        names = [x for x in dir(m) if not x.startswith("_")]
        log("\n=== unreal.%s (%d) ===" % (mod, len(names)))
        for x in names:
            log("   " + x)

    # Skeleton 对象自己暴露了什么
    skel = unreal.load_asset(SKEL_PATH)
    log("\n=== dir(skeleton obj) bone-ish ===")
    log(str([x for x in dir(skel) if not x.startswith("_") and ("bone" in x.lower() or "ref" in x.lower())]))

    # AnimPose 路线
    log("\n=== AnimPoseExtensions 路线 ===")
    AP = unreal.AnimPoseExtensions
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    seqs = [a.get_asset() for a in ar.get_assets_by_path(ANIM_DIR, recursive=False)
            if str(a.asset_class_path.asset_name) == "AnimSequence" and a.get_asset()]
    seqs.sort(key=lambda o: o.get_name())
    log("anims: %d" % len(seqs))
    s = seqs[0]
    log("sample: %s" % s.get_name())
    try:
        log("AnimPoseExtensions fns: %s" % [x for x in dir(AP) if not x.startswith("_")])
        pose = AP.get_anim_pose(s, 0.0, 1.0)
        log("pose obj: %s" % pose)
        bones = AP.get_bone_names(pose)
        log("bones (%d):" % len(bones))
        for i, b in enumerate(bones[:25]):
            log("   [%3d] %s" % (i, b))
    except Exception:
        log("AnimPose route failed:\n" + traceback.format_exc())

except Exception:
    log("FATAL:\n" + traceback.format_exc())
finally:
    dump()
