#!/bin/bash
# 同时推送 GitHub + 工蜂（含 LFS）
set -e
export https_proxy=http://172.19.176.1:7890
export http_proxy=http://172.19.176.1:7890

echo "==> 推送 LFS 到 GitHub..."
git lfs push --all https://github.com/renderbin/Warrior.git master

echo "==> 推送 LFS 到 工蜂..."
git lfs push --all https://git.code.tencent.com/renderbin/Warrior.git master

echo "==> 推送分支到两边..."
git push origin master

echo "==> 完成 ✅"
