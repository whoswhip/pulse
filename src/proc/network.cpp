#include "network.hpp"

#include <fstream>
#include <sstream>

namespace pulse::proc {

    auto network_reader::read() -> std::expected<std::vector<net_interface>, std::string> {
        std::ifstream file("/proc/net/dev");
        if (!file.is_open()) {
            return std::unexpected("failed to open /proc/net/dev");
        }

        auto now = std::chrono::steady_clock::now();
        double elapsed = 1.0;
        if (m_has_previous) {
            elapsed = std::chrono::duration<double>(now - m_last_read).count();
            if (elapsed <= 0.0) elapsed = 1.0;
        }

        std::string line;
        std::getline(file, line);
        std::getline(file, line);

        std::vector<net_interface> interfaces;

        while (std::getline(file, line)) {
            auto colon = line.find(':');
            if (colon == std::string::npos) continue;

            net_interface iface;
            iface.name = line.substr(0, colon);
            auto start = iface.name.find_first_not_of(' ');
            if (start != std::string::npos) iface.name = iface.name.substr(start);

            if (iface.name == "lo") continue;

            std::istringstream iss(line.substr(colon + 1));
            uint64_t rx_packets, rx_errs, rx_drop, rx_fifo, rx_frame, rx_compressed, rx_multicast;
            iss >> iface.rx_bytes >> rx_packets >> rx_errs >> rx_drop >> rx_fifo >> rx_compressed >> rx_multicast;

            uint64_t tx_packets, tx_errs, tx_drop, tx_fifo, tx_colls, tx_carrier, tx_compressed;
            iss >> iface.tx_bytes >> tx_packets >> tx_errs >> tx_drop >> tx_fifo >> tx_colls >> tx_carrier >> tx_compressed;

            if (m_has_previous && m_previous.contains(iface.name)) {
                auto& prev = m_previous[iface.name];
                iface.rx_rate_bytes_sec = static_cast<double>(iface.rx_bytes - prev.rx_bytes) / elapsed;
                iface.tx_rate_bytes_sec = static_cast<double>(iface.tx_bytes - prev.tx_byes) / elapsed;
            }

            m_previous[iface.name] = { iface.rx_bytes, iface.tx_bytes };
            interfaces.push_back(iface);
        }

        m_last_read = now;
        m_has_previous = true;
        return interfaces;
    }
}
