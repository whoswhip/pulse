#include "disk.hpp"

#include <cstdint>
#include <string>
#include <sys/statvfs.h>

#include <fstream>
#include <set>
#include <sstream>

namespace pulse::proc {
    
    namespace {

        auto unescape_mount_path(const std::string& path) -> std::string {
            std::string result;
            result.reserve(path.size());
            for (size_t i = 0; i < path.size(); ++i) {
                if (path[i] == '\\' && i + 3 < path.size()) {
                    char c = static_cast<char>((path[i + 1] - '0') * 64 + (path[i + 2] - '0') * 8 + (path[i + 3] - '0'));
                    result += c;
                    i += 3;
                } else {
                    result += path[i];
                }
            }
            return result;
        }
    }

    auto disk_reader::read() -> std::expected<std::vector<disk_info>, std::string> {
        std::ifstream file("/proc/mounts");
        if (!file.is_open()) {
            return std::unexpected("failed to open /proc/mounts");
        }

        static const std::set<std::string> real_fs = {
            "ext4", "ext3", "btrfs", "xfs", "zfs", "ntfs", "vfat", "f2fs", "bcachefs", "fuseblk"
        };

        std::vector<disk_info> disks;
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string device, mount, fs;
            iss >> device >> mount >> fs;

            mount = unescape_mount_path(mount);

            if (!real_fs.contains(fs)) continue;

            struct statvfs stat {};
            if (statvfs(mount.c_str(), &stat) != 0) continue;

            disk_info info;
            info.mount_point = mount;
            info.filesystem = fs;
            info.total_bytes = static_cast<uint64_t>(stat.f_blocks) * stat.f_frsize;
            info.available_bytes = static_cast<uint64_t>(stat.f_bavail) * stat.f_frsize;
            info.used_bytes = info.total_bytes - (static_cast<uint64_t>(stat.f_bfree) * stat.f_frsize);

            disks.push_back(info);
        }

        return disks;

    }
}
