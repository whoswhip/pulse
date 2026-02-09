#include "disk.hpp"

#include <cstdint>
#include <string>
#include <sys/statvfs.h>

#include <fstream>
#include <set>
#include <sstream>

namespace pulse::proc {
    
    auto disk_reader::read() -> std::expected<std::vector<disk_info>, std::string> {
        std::ifstream file("/proc/mounts");
        if (!file.is_open()) {
            return std::unexpected("failed to open /proc/mounts");
        }

        static const std::set<std::string> real_fs = {
            "ext4", "ext3", "btrfs", "xfs", "zfs", "ntfs", "vfat", "f2fs", "bcachefs"
        };

        std::vector<disk_info> disks;
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string device, mount, fs;
            iss >> device >> mount >> fs;

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
